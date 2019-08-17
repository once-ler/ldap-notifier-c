#include "queue-supervisor.h"

void init_queue() {
  TAILQ_INIT(&client_tailq_head);
}

void remove_from_queue(struct client* client) {
  TAILQ_REMOVE(&client_tailq_head, client, entries);
}

void add_to_queue(struct client* client) {
  TAILQ_INSERT_TAIL(&client_tailq_head, client, entries);
}

void broadcast() {
  for (;;) {
    // Send data to all connected clients.
    char *message;
    struct client* item = TAILQ_FIRST(&head);
    
    // TAILQ_FOREACH(item, &head, entries) {
    TAILQ_FOREACH(item, &client_tailq_head, entries) {
			int sock = item->fd;
      // bufferevent_write(client->buf_ev, data,  n);
      // if (sock != NULL) {
      message = "Greetings! I am your connection handler\n";
      size_t r;
      r = write(sock , message , strlen(message));
      
      if (r == -1)
        continue;

      message = "Spinning... \n";
      write(sock , message , strlen(message));
      // }
    }

    sleep(1);
  }
}
