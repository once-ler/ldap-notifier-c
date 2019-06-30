ldap-notifier-c
===============

### Install lib
```
gcc -g -Wall -I ./include -c src/ldap-notifier.c -shared -o build/libldap-notifier.so -lldap -llber -lcjson

```

### Install server
```
gcc -g -Wall -I ./include -c src/simple-socket.c -shared -o build/libsimple-socket.so
```

```
gcc -g -Wall -I ./include src/server.c -o build/ldap-notifier-server -Lbuild -lsimple-socket   -lldap-notifier -lldap -llber -lcjson -lpthread

```

### Test connection
```
nc -vz localhost 7676
```
