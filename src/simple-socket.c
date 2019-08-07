#include "simple-socket.h"
#include "queue.h"
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

TAILQ_HEAD(tailhead, client) head;
struct tailhead* headp; /* Tail queue head. */

struct client {
	/* The clients socket. */
	int fd;

	/*
	 * This holds the pointers to the next and previous entries in
	 * the tail queue.
	 */
	TAILQ_ENTRY(client) entries;
};


/*
  Function used by ldap notifier to stream updates to.
*/
void broadcast() {
  for (;;) {
    // Send data to all connected clients.
    char *message;
    struct client* item = TAILQ_FIRST(&head);
    TAILQ_FOREACH(item, &head, entries) {
      int sock = item->fd;
      // bufferevent_write(client->buf_ev, data,  n);
      message = "Greetings! I am your connection handler\n";
      write(sock , message , strlen(message));
        
      message = "Spinning... \n";
      write(sock , message , strlen(message));

    }

    sleep(1);
  }
}

/*
void on_client_disconnect() {
  // Remove the client from the tailq. //
	TAILQ_REMOVE(&client_tailq_head, client, entries);
}
*/

/**
 * Set a socket to non-blocking mode.
 * Not working for this setup.
 */
int
setnonblock(int fd) {
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)
		return -1;

	return 0;
}

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

  struct client* cl;
  cl = malloc(sizeof(struct client));
  if (cl == NULL)
    err(1, "malloc failed");

  cl->fd = sock;

  /* Add the new client to the tailq. */
  TAILQ_INSERT_TAIL(&head, cl, entries);  
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

  TAILQ_INIT(&head);                      /* Initialize the queue. */
  struct client* n1 = malloc(sizeof(struct client));      /* Insert at the head. */
  TAILQ_INSERT_HEAD(&head, n1, entries);
  
  fprintf(stdout, "Listening on address %s and port %d\n", hostname, port);

  c = sizeof(struct sockaddr_in);

  pthread_t broadcast_thread;
  pthread_create( &broadcast_thread, NULL, broadcast, NULL);
  
  while( (client_sock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
      puts("Connection accepted");

      pthread_t sniffer_thread;
      
      if( pthread_create( &sniffer_thread, NULL, connection_handler, (void*) &client_sock) < 0) {
        //ERROR
        puts("Allocation error");
      }
  }

  if (client_sock < 0) {
      //ERROR
  }

  return 0;
}