#include "ldap-notifier.h"

static inline int is_ascii_string (char *blob, int len) {
  int i;

  if (len <= 0)
    return 0;

  for (i = 0; i < len; i++)
    if (!isascii (blob[i]))
      return 0;

  return 1;
}

int init(struct LdapNotifier* ldapNotifier) {
  if ((ldapNotifier->cnx = ldap_init(ldapNotifier->hostname, ldapNotifier->port)) == NULL) {
    perror( "ldap_init failed" );
    exit(1);
  } else {
    printf("Generated LDAP handle.\n");
  }
  
  fprintf(stdout, "%d\n", (ldapNotifier->cnx == NULL));

  ldapNotifier->ret = ldap_set_option(ldapNotifier->cnx, LDAP_OPT_PROTOCOL_VERSION, &ldapNotifier->protocol);

  if (ldapNotifier->ret != LDAP_OPT_SUCCESS) {
    ldap_perror(ldapNotifier->cnx, "ldap_set_option failed!");
    exit(1);
  } else {
    printf("Set LDAPv3 client version.\n");
  }
  
  return ldapNotifier->ret;
}

int bind(struct LdapNotifier* ldapNotifier) {
  ldapNotifier->ret = ldap_simple_bind_s(ldapNotifier->cnx, ldapNotifier->ldap_dn, ldapNotifier->ldap_pw );

  if (ldapNotifier->ret != LDAP_SUCCESS) {
    fprintf(stderr, "ldap_simple_bind_s: %s\n", ldap_err2string(ldapNotifier->ret));
    exit(1);
  } else {
    printf("LDAP connection successful.\n");
  }

  return ldapNotifier->ret;
}

int search(struct LdapNotifier* ldapNotifier, char* attrs[]) {
  //  Set up the control to search the directory for objects that have 
  //  changed from a previous state.
  ldapNotifier->notifyControl.ldctl_oid = LDAP_SERVER_NOTIFICATION_OID_W;
  ldapNotifier->notifyControl.ldctl_iscritical = 1;

  //  Set up the control to specify a BER-encoded sequence of parameters to the
  //  length of the binary data and to initialize a pointer.
  ldapNotifier->notifyControl.ldctl_value.bv_len = 0;
  ldapNotifier->notifyControl.ldctl_value.bv_val = NULL;

  //  Initialize the control array values.
  ldapNotifier->controlArray[0] = &ldapNotifier->notifyControl;
  ldapNotifier->controlArray[1] = NULL;

  ldapNotifier->msgid = 0;
  
  ldapNotifier->ret = ldap_search_ext (
    ldapNotifier->cnx,
    ldapNotifier->base,
    ldapNotifier->scope,
    ldapNotifier->filter,
    attrs,      // attrs       /
    0,         // attrsonly   /
    (LDAPControl **) &ldapNotifier->controlArray,
    0,      // clientctrls /
    0,      // timeout     /
    LDAP_NO_LIMIT,
    &ldapNotifier->msgid);

  if (ldapNotifier->ret != LDAP_SUCCESS) {
    fprintf (stderr, "Error querying with base='%s' and filter='%s': %s\n",
      ldapNotifier->base, ldapNotifier->filter, ldap_err2string (ldapNotifier->ret));

    ldap_unbind_ext (ldapNotifier->cnx, NULL, NULL);

    exit(1);
  }

  return ldapNotifier->ret;
}

void listen(struct LdapNotifier* ldapNotifier, LdapNotifierDataHandler* on_data, LdapNotifierErrorHandler* on_error) {
  ldapNotifier->timeout.tv_sec  = 1;
  ldapNotifier->timeout.tv_usec = 0;
  ldapNotifier->entries = NULL;

  while ((ldapNotifier->ret = ldap_result(
    ldapNotifier->cnx,
    LDAP_RES_ANY, //  Message identifier.
    LDAP_MSG_ONE, //  Retrieve one message at a time.
    NULL,         //  No timeout.
    &ldapNotifier->entries)
  ) >= 0) {
    LDAPMessage  *entry;
    
    if (!ldapNotifier->ret) {
      #ifdef DEBUG
      printf ("Timed out\n");
      #endif
      continue;
    }
    
    entry = ldap_first_entry (ldapNotifier->cnx, ldapNotifier->entries);
    
    while (entry != NULL) {
      
      BerElement* berptr;
      const char* attr;
      cJSON* root = cJSON_CreateObject();

      #ifdef DEBUG
      printf ("dn: %s\n", ldap_get_dn (ldapNotifier->cnx, entry));
      #endif

      berptr = NULL;
      attr = ldap_first_attribute (ldapNotifier->cnx, entry, &berptr);
      while (attr != NULL) {
        struct berval **values;
        int i;

        values = ldap_get_values_len (ldapNotifier->cnx, entry, attr);

        for (i = 0; values[i] != NULL; i++) {
          if (is_ascii_string (values[i]->bv_val, values[i]->bv_len)) {
            cJSON_AddItemToObject(attr, "carID", cJSON_CreateString(values[i]->bv_val));

            #ifdef DEBUG
            printf ("%s: %s\n", attr, values[i]->bv_val);
            #endif
          }
        }

        ldap_value_free_len (values);

        attr = ldap_next_attribute (ldapNotifier->cnx, entry, berptr);
      }

      if (berptr != NULL)
        ber_free (berptr, 0);

      #ifdef DEBUG
      printf ("\n\n");
      #endif
      
      (*on_data)(cJSON_Print(root));

      cJSON_Delete(root);

      entry = ldap_next_entry (ldapNotifier->cnx, entry);
    }

    ldap_msgfree (ldapNotifier->entries);
  }

  // Report errors.
  if (ldapNotifier->ret < 0) {
    int err;

    ldapNotifier->ret = ldap_parse_result(ldapNotifier->cnx, ldapNotifier->entries, &err, NULL, NULL, NULL, NULL, 0);

    char* errorMessage = (char*)malloc(255);
    sprintf (errorMessage, "Error fetching result from query: %s\n", ldap_err2string (err));
    
    (*on_error)(errorMessage);
    
    free(errorMessage);

    #ifdef DEBUG
    fprintf (stderr, "%s", errorMessage);
    #endif

    ldap_msgfree (ldapNotifier->entries);
    ldap_unbind_ext (ldapNotifier->cnx, NULL, NULL);

    return ldapNotifier->ret;
  }

  ldap_unbind_ext (ldapNotifier->cnx, NULL, NULL);

  return ldapNotifier->ret;
}
