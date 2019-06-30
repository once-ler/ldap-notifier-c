#include "simple-socket.h"

/*
int connectSocket() {
  if (connect(sock, (struct sockaddr*)(&sin),
    sizeof(struct sockaddr_in)) != 0) {
    fprintf(stderr, "Failed to connect!\n");
    return 1;
  }
  return 0;
}
*/

void closeSocket() {
  #ifdef _WIN32
    closesocket(*sock);
  #else
      close(sock);
  #endif
}

void *connection_handler(void *socket_desc) {
  //Get the socket descriptor
  int sock = *(int*)socket_desc;
}

int createSocket(const char* hostname, int port) {
  socket_opt = 1;

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
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
    perror("Create socket failed.\n"); 
    exit(1); 
  }

  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socket_opt, sizeof(socket_opt))) { 
      perror("Call to setsockopt failed.\n"); 
      exit(1); 
  }
  
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY; 
  sin.sin_port = htons(port);

  if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) { 
      perror("Call to bind socket failed.\n"); 
      exit(1); 
  } 
  
  if (listen(sock, 5) < 0) { 
      perror("Call to listen socket failed.\n"); 
      exit(1); 
  } 
  
  fprintf(stdout, "Listening on address %s and port %d\n", hostname, port);

  c = sizeof(struct sockaddr_in);

  while( (client_sock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
      puts("Connection accepted");

      pthread_t sniffer_thread;
      new_sock = (int*) malloc(1);
      *new_sock = client_sock;

      if( pthread_create( &sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0) {
          //ERROR
      }

      printf("Handler assigned");
  }

  if (client_sock < 0) {
      //ERROR
  }

  return 0;
}