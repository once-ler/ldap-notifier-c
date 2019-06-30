#include "simple-socket.h"

int connectImpl() {
  if (connect(sock, (struct sockaddr*)(&sin),
    sizeof(struct sockaddr_in)) != 0) {
    fprintf(stderr, "Failed to connect!\n");
    return 1;
  }
  return 0;
}

void closeSocket() {
  #ifdef _WIN32
    closesocket(*sock);
  #else
      close(sock);
  #endif
}

int createSocket() {
  reti = regcomp(&ip_regex, IP_ADDR_REGEX, 0);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }

  #ifdef WIN32
  WSADATA wsadata;
  int err;

  err = WSAStartup(MAKEWORD(2, 0), &wsadata);
  if (err != 0) {
    fprintf(stderr, "WSAStartup failed with error: %d\n", err);
    return 1;
  }
  #endif
      
  // if ip address was passed in
  reti = regexec(&ip_regex, hostname, 0, NULL, 0);
  if (!reti) {
    hostaddr = inet_addr(hostname);
    sin.sin_addr.s_addr = hostaddr;
  } else {
    if (!(hp = gethostbyname(hostname))){
      return 0;
    }          
    sin.sin_addr = *(struct in_addr*) hp->h_addr_list[0];
  }
  
  /* Ultra basic "connect to port 443 on host"
  * Your code is responsible for creating the socket establishing the
  * connection
  */
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    return 1;
  }
  
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  
  return 0;
}