#ifndef SIMPLE_SOCKET
#define SIMPLE_SOCKET

#define IP_ADDR_REGEX "(\\b((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b)"

#ifdef _WIN32
#include <winsock2.h>
#endif
#ifndef _WIN32
#include <sys/socket.h>
#endif
#ifndef _WIN32
#include <netinet/in.h>
#endif
#ifndef _WIN32
#include <sys/select.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifndef _WIN32
#include <arpa/inet.h>
#endif
#ifndef _WIN32
#include <sys/time.h>
#endif
 
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <regex.h>     
#include <pthread.h>

unsigned long hostaddr;
int sock;
struct sockaddr_in sin, client;
struct hostent* hp;
int socket_opt, client_sock, c, *new_sock;

regex_t ip_regex;
int reti;

int createSocket(const char*, int);

int connectSocket();

void closeSocket();

void *connection_handler(void *socket_desc);

#endif