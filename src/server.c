#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "handler.h"
#include "avl_tree.h"

static int setup_connection(size_t port) {
  struct sockaddr_in server_address;
  // setup socket address structure
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;

  // create socket
  int server  = socket(PF_INET, SOCK_STREAM, 0);
  if(!server ) {
    perror("socket");
    exit(-1);
  }

  // set socket to immediately reuse port when the application closes
  int reuse = 1;
  if (setsockopt(server , SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    perror("setsockopt");
    exit(-1);
  }

  // call bind to associate the socket with our local address and port
  if (bind(server, (const struct sockaddr *)&server_address, sizeof(server_address) ) < 0) {
    perror("bind");
    exit(-1);
      }

  // convert the socket to listen for incoming connections
  if (listen(server, SOMAXCONN) < 0) {
    perror("listen");
    exit(-1);
  }

  return server;
}

static handle_connections(int server, avl_tree_t * db) {
  int client;
  struct sockaddr_in client_address;
  socklen_t client_length = sizeof(client_address);

  // accept connections
  while( (client = accept(server, (struct sockaddr *)&client_address, &client_length) ) > 0 ) {
    pthread_t new_thread;
    handler_t handler;
    handler.db = db;
    handler.client = client;

    pthread_create(&new_thread, NULL, &handle_connection, (void *)&handler);
  }
}

int main(int argc, char ** argv) {
  printf("Starting server on port 0.0.0.0:3400\n");
  int server = setup_connection(3400);

  avl_tree_t * db = avl_init();
  handlers_init();
  handle_connections(server, db);
  handlers_deinit();
  avl_deinit(db);
  return 0;
}
