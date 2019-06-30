#include <getopt.h>
#include "ldap-notifier.h"

const char *host = "127.0.0.1", *base, *filter = "ObjectClass=*", *dn, *password;
int port = 389, protocol = LDAP_VERSION3, scope = LDAP_SCOPE_SUBTREE;

void printHelp() {
  printf(
    "--host <t>:          Host name (Default: 127.0.0.1)\n"
    "--port <p>:          Port (Default: 389)\n"
    "--base <b>:          Base (No default.  Example: dc=github,dc=com)\n"
    "--filter <f>:        Filter (Example: objectClass=* or ObjectClass=* for Microsoft.  Default: ObjectClass=*)\n"
    "--protocol <l>:      Protocol (Default: LDAP_VERSION3)\n" 
    "--scope <s>:         Scope (Default: LDAP_SCOPE_SUBTREE)\n"
    "--dn <d>:            Distinguished Name (No default.  Example: joeschmoe@github.com)\n"
    "--password <a>:      Password (No default)\n"
    "--help <h>:          Show help\n"
  );
  exit(1);
}

void parseArgs(int argc, char *argv[]) {
  const char* const short_opts = "s:e:p:c:t:h";
  
  const char* long_opts[] = {
    {"host", required_argument, NULL, 'h'},
    {"port", required_argument, NULL, 'p'},
    {"base", required_argument, NULL, 'b'},
    {"filter", required_argument, NULL, 'f'},
    {"protocol", required_argument, NULL, 'l'},
    {"scope", required_argument, NULL, 's'},
    {"dn", required_argument, NULL, 'd'},
    {"password", required_argument, NULL, 'a'},
    {"help", no_argument, NULL, 'h'},
    {NULL, no_argument, NULL, 0}
  };
  
  while (1) {
    const auto opt = getopt_long(argc, argv, short_opts, long_opts, NULL);

    if (-1 == opt)
      break;

    switch (opt) {
      case 't':
        host = optarg;
        break;

      case 'p':
        port = atoi(optarg);
        break;

      case 'b':
        base = optarg;
        break;
        
      case 'f':
        filter = optarg;
        break;

      case 'l':
        protocol = atoi(optarg);
        break;
      
      case 's':
        scope = atoi(optarg);
        break;
      
      case 'd':
        dn = optarg;
        break;

      case 'a':
        password = optarg;
        break;
  
      case 'h': // -h or --help
      case '?': // Unrecognized option
      default:
        printHelp();
        break;
    }
  }
  
  fprintf(stdout, 
    "Host set to %s",
    "Port set to %d",
    "Base set to %s",
    "Filter set to %s",
    "Protocol set to %d",
    "Scope set to %d",
    "DN set to %s",
    "Password set to %s",
    host,
    port,
    base,
    filter,
    protocol,
    scope,
    dn,
    password
  );

}

int main(int argc, const char* argv[]) {
  parseArgs(argc, argv);
}
