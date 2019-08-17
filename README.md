ldap-notifier-c
===============

### Install ldap lib
```
gcc -g -Wall -I ./include -c src/ldap-notifier.c -shared -o build/libldap-notifier.so -lldap -llber -lcjson

```

### Install queue lib
```
gcc -g -Wall -I ./include -c src/queue-supervisor.c -shared -o build/libqueue-supervisor.so
```

### Install server (using simple-socket)
```
gcc -g -Wall -I ./include -c src/simple-socket.c -shared -o build/libsimple-socket.so
```

```
gcc -DUSE_SOCKET=SIMPLE  -g -Wall -I ./include src/server.c -o build/ldap-notifier-server -Lbuild -lsimple-socket -lqueue-supervisor -lldap-notifier -lldap -llber -lcjson -lpthread

```

### Install server (using libevent)
```
gcc -g -Wall -I ./include -c src/libevent-socket.c -shared -o build/libevent-socket.so
```

```
gcc -g -Wall -I ./include src/server.c -o build/ldap-notifier-server -Lbuild -levent-socket -lqueue-supervisor -levent -lldap-notifier -lldap -llber -lcjson -lpthread

```

### Test connection
```
nc -vz localhost 7676
```
