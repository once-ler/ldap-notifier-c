#ifndef LDAP_NOTIFIER
#define LDAP_NOTIFIER

#define LDAP_NOTIFIER_VERSION "0.1.2"

#define LDAP_SERVER_NOTIFICATION_OID_W  "1.2.840.113556.1.4.528"

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>
#include <ldap.h>
#include "cjson/cJSON.h"

struct LdapNotifier {
  const char* hostname;
  int port;
  const char* base;
  const char* filter;
  int protocol;
  int scope;
  LDAP* cnx;
  LDAPMessage* entries;
  struct timeval timeout;
  int msgid;
  int ret;

  char* ldap_dn;
  char* ldap_pw;

  LDAPControl notifyControl; //  Structure used to represent both client and server controls.
  LDAPControl* controlArray[3]; //  Pointer to the LDAPControl.
};

// Create a LDAP connection and set up preferences.
int init(struct LdapNotifier*);

// Bind to the LDAP server.
int bind(struct LdapNotifier*);

//  Set up the control to search the directory for objects that have 
//  changed from a previous state.
int search(struct LdapNotifier*, char* []);

// Handler that receives the parsed BerElement in JSON format.
typedef void (*LdapNotifierDataHandler) (const char* message);
// extern LdapNotifierDataHandler handleData(const char* message, LdapNotifierErrorHandler handler);

// Handler that captures connection context errors.
typedef void (*LdapNotifierErrorHandler) (const char* message);
// extern  LdapNotifierErrorHandler handleError(const char* message, LdapNotifierErrorHandler handler);

// Asynchronously capture a message indicating a change.
void listen(struct LdapNotifier*, LdapNotifierDataHandler*, LdapNotifierErrorHandler*);

#endif
