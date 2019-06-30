lday-notifier
=============

```
gcc -g -Wall -I ./include -c src/ldap-notifier.c -shared -o build/libldap-notifier.so -lldap -llber -lcjson

```

```
gcc -g -Wall -I ./include src/server.c -o build/ldap-notifier-server -Lbuild -lldap-notifier -lldap -llber -lcjson

```