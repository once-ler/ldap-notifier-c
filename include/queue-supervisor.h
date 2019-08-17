#ifndef QUEUE_SUPERVISOR
#define QUEUE_SUPERVISOR

#include <stdlib.h>
#include <string.h>
#include <unistd.h> // socket->write()
#include "queue.h"

struct bufferevent;

TAILQ_HEAD(tailhead, client) head;
struct tailhead* headp; /* Tail queue head. */

struct client {
	/* The clients socket. */
	int fd;

	/* The bufferedevent for this client. */
	struct bufferevent *buf_ev;

	/*
	 * This holds the pointers to the next and previous entries in
	 * the tail queue.
	 */
	TAILQ_ENTRY(client) entries;
};

/**
 * The head of our tailq of all connected clients.  This is what will
 * be iterated to send a received message to all connected clients.
 */
TAILQ_HEAD(, client) client_tailq_head;

void init_queue();

void remove_from_queue(struct client* client);

void add_to_queue(struct client* client);

/*
  Function used by ldap notifier to stream updates to.
*/
void broadcast();

#endif
