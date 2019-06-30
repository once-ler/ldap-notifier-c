#include <getopt.h>
#include "ldap-notifier.h"

const char *host = "localhost", *base, *filter = "objectClass=*", *dn, *password;
int notifier_port = 7676, port = 389, protocol = LDAP_VERSION3, scope = LDAP_SCOPE_SUBTREE;

void printHelp() {
  printf(
    "--notifier-port <n>: Port used by notifier (Default: 7676)\n" 
    "--host <t>:          Host name (Default: localhost)\n"
    "--port <p>:          Port (Default: 389)\n"
    "--base <b>:          Base (No default.  Example: dc=eztier,dc=com)\n"
    "--filter <f>:        Filter (Example: objectClass=* or ObjectClass=* for Microsoft.  Default: objectClass=*)\n"
    "--protocol <l>:      Protocol (Default: LDAP_VERSION3)\n" 
    "--scope <s>:         Scope (Default: LDAP_SCOPE_SUBTREE)\n"
    "--dn <d>:            Distinguished Name (No default.  Example: admin@eztier.com)\n"
    "--password <a>:      Password (No default)\n"
    "--help <h>:          Show help\n"
  );
  exit(1);
}

void parseArgs(int argc, char *argv[]) {
  const char* const short_opts = "s:e:p:c:t:h";
  
  const char* long_opts[] = {
    {"notifier-port", required_argument, NULL, 'n'},
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
      case 'n':
        notifier_port = atoi(optarg);
        break;

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
    "Notifier set to %d\n"
    "Host set to %s\n"
    "Port set to %d\n"
    "Base set to %s\n"
    "Filter set to %s\n"
    "Protocol set to %d\n"
    "Scope set to %d\n"
    "DN set to %s\n"
    "Password set to %s\n",
    notifier_port,
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
