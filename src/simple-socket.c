#include "simple-socket.h"
#include "queue-supervisor.h"

void closeSocket() {
  #ifdef _WIN32
    closesocket(*sock);
  #else
      close(sock);
  #endif
}

void* connection_handler(void *socket_desc) {
  //Get the socket descriptor
  int sock = *(int*)socket_desc;

  struct client* cl;
  cl = malloc(sizeof(struct client));
  if (cl == NULL)
    err(1, "malloc failed");

  cl->fd = sock;

  /* Add the new client to the tailq. */
  add_to_queue(cl);

  // Continuously detect the client connection.
  fd_set read_sd;
  FD_ZERO(&read_sd);
  FD_SET(sock, &read_sd);

  while (1) {
    fd_set rsd = read_sd;

    int sel = select(sock + 1, &rsd, 0, 0, 0);

    if (sel > 0) {
      // client has performed some activity (sent data or disconnected?)
      char buf[1024] = {0};

      int bytes = recv(sock, buf, sizeof(buf), 0);

      if (bytes > 0) {
        // got data from the client.
      } else if (bytes == 0) {
        // client disconnected.
        break;
      } else {
        // error receiving data from client. You may want to break from
        // while-loop here as well.
        break;
      }
    } else if (sel < 0) {
      // grave error occurred.
      break;
    }
  }

  close(sock);
  fprintf(stdout, "Client %d disconnected\n", cl->fd);

  // Remove the client from the tailq. //
	remove_from_queue(cl);
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
  
  init_queue();

  fprintf(stdout, "Listening on address %s and port %d\n", hostname, port);

  c = sizeof(struct sockaddr_in);

  pthread_t broadcast_thread;
  pthread_create( &broadcast_thread, NULL, (void*) broadcast, NULL);
  
  while( (client_sock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
    puts("Connection accepted");

    pthread_t sniffer_thread;
    
    if( pthread_create( &sniffer_thread, NULL, (void*) connection_handler, (void*) &client_sock) < 0) {
      //ERROR
      puts("Allocation error");
    }
  }

  if (client_sock < 0) {
    //ERROR
    printf("Error\n");
  }

  return 0;
}