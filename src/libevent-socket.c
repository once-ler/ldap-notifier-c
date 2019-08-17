#include "libevent-socket.h"
#include "queue-supervisor.h"

/**
 * Set a socket to non-blocking mode.
 */
int setnonblock(int fd) {
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)
		return -1;

	return 0;
}

/**
 * Called by libevent when there is data to read.
 */
void buffered_on_read(struct bufferevent *bev, void *arg) {
	// struct client *this_client = (struct client *) arg;
	// struct client *client;
	uint8_t data[8192];
	size_t n;

	/* Read 8k at a time and send it to all connected clients. */
	for (;;) {
		n = bufferevent_read(bev, data, sizeof(data));
		if (n <= 0) {
			/* Done. */
			break;
		}

		printf("%s", (char*)data);
		
		// Send data to all connected clients except for the
		// client that sent the data.
		/*
		TAILQ_FOREACH(client, &client_tailq_head, entries) {
			if (client != this_client) {
				bufferevent_write(client->buf_ev, data,  n);
			}
		}
		*/
	}

}

/**
 * Called by libevent when there is an error on the underlying socket
 * descriptor.
 */
void buffered_on_error(struct bufferevent *bev, short what, void *arg) {
	struct client *client = (struct client *)arg;

	if (what & BEV_EVENT_EOF) {
		/* Client disconnected, remove the read event and the
		 * free the client structure. */
		printf("Client disconnected.\n");
	}
	else {
		warn("Client socket error, disconnecting.\n");
	}

	/* Remove the client from the tailq. */
	remove_from_queue(client);

	bufferevent_free(client->buf_ev);
	close(client->fd);
	free(client);
}

/**
 * This function will be called by libevent when there is a connection
 * ready to be accepted.
 */
void on_accept(int fd, short ev, void *arg) {
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct client *client;

	client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		warn("accept failed");
		return;
	}

	/* Set the client socket to non-blocking mode. */
	if (setnonblock(client_fd) < 0)
		warn("failed to set client socket non-blocking");

	/* We've accepted a new client, create a client object. */
	client = (struct client*) calloc(1, sizeof(*client));
	if (client == NULL)
		err(1, "malloc failed");
	client->fd = client_fd;

	client->buf_ev = bufferevent_socket_new(evbase, client_fd, 0);
	
	bufferevent_setcb(
		client->buf_ev, 
		buffered_on_read, 
		NULL,
	  buffered_on_error,
		client
	);

	/* We have to enable it before our callbacks will be
	 * called. */
	bufferevent_enable(client->buf_ev, EV_READ);

	/* Add the new client to the tailq. */
	add_to_queue(client);

	printf("Accepted connection from %s\n", 
	    inet_ntoa(client_addr.sin_addr));
}

void closeSocket() {
  #ifdef _WIN32
    closesocket(*sock);
  #else
      close(sock);
  #endif
}

int createSocket(const char* hostname, int port) {
  reti = regcomp(&ip_regex, IP_ADDR_REGEX, 0);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }

	pthread_t broadcast_thread;
  pthread_create( &broadcast_thread, NULL, (void*) broadcast, NULL);
  
	/* Initialize libevent. */
  evbase = event_base_new();

	/* Initialize the tailq. */
	init_queue();

  // if ip address was passed in
  reti = regexec(&ip_regex, hostname, 0, NULL, 0);
  
  /* Create our listening socket. */
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
		err(1, "listen failed");
  
  memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	
	if (!reti) {
    hostaddr = inet_addr(hostname);
    listen_addr.sin_addr.s_addr = hostaddr;
  } else {
    if (!(hp = gethostbyname(hostname))){
      return 0;
    }          
    listen_addr.sin_addr = *(struct in_addr*) hp->h_addr_list[0];
  }
  
  listen_addr.sin_port = htons(port);
	
  if (bind(listen_fd, (struct sockaddr *)&listen_addr,
		sizeof(listen_addr)) < 0)
		err(1, "bind failed");
	if (listen(listen_fd, 5) < 0)
		err(1, "listen failed");
	
  reuseaddr_on = 1;
	setsockopt(
    listen_fd, 
    SOL_SOCKET, 
    SO_REUSEADDR, 
    &reuseaddr_on, 
	  sizeof(reuseaddr_on)
  );

	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (setnonblock(listen_fd) < 0)
		err(1, "failed to set server socket to non-blocking");

	/* We now have a listening socket, we create a read event to
	 * be notified when a client connects. */
  event_assign(
    &ev_accept, 
    evbase, 
    listen_fd, 
    EV_READ|EV_PERSIST, 
	  on_accept, 
    NULL
  );
	
  event_add(&ev_accept, NULL);

	/* Start the event loop. */
	event_base_dispatch(evbase);

  return 0;
}
