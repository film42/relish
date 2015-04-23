#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "handler.h"
#include "parser.h"
#include "types.h"

pthread_mutex_t lock;

// read_once reads from the socket a single time
static int read_once(int client, char * buffer, size_t length) {
  int bytes_read = recv(client , buffer, (length - 1), 0);
  if (bytes_read < 0) {
    if (errno == EINTR) {
      // the socket call was interrupted -- try again
      return -1;
    } else {
      // an error occurred, so break out
      return -1;
    }

  } else if (bytes_read == 0) {
    // the socket is closed
    return -2;
  }

  return bytes_read;
}

static int write_all(int client, char * message) {
  // prepare to send request
  int bytes_left = strlen(message);
  int bytes_written;
  // loop to be sure it is all sent
  while (bytes_left) {
    if ( (bytes_written = send(client, message, bytes_left, 0) ) < 0) {
      if (errno == EINTR) {
        // the socket call was interrupted -- try again
        continue;
      } else {
        // an error occurred, so break out
        perror("write");
        return -1;
      }
    } else if (bytes_written == 0) {
      // the socket is closed
      return -2;
    }
    bytes_left -= bytes_written;
    // message += bytes_written;
  }

  return 0;
}

static char * get_until_newline(int client) {
  char buffer[1025];
  int bytes_read = read_once(client, buffer, 1024);

  if(bytes_read < 0) {
    return NULL;
  }

  int i = 0;
  for(; i < bytes_read; ++i) {
    if(buffer[i] == '\n' ) {
      goto found;
    }
  }

  return "";

 found: {
    char * complete_buffer = (char *)malloc(sizeof(char) * (i + 1));
    strncpy(complete_buffer, buffer, (i + 1));
    return complete_buffer;
  }
}

static int exec_command(int client, command_t * command, avl_tree_t * db) {
  pthread_mutex_lock(&lock);

  switch(command->action) {
  case INSERT: {
    size_t size_of_value = (sizeof(char) * strlen(command->value));
    avl_insert(db, command->key, command->value, size_of_value);
    write_all(client, "(ok!)\n");
    break;
  }
  case GET: {
    char * value = (char *)avl_get(db, command->key);

    if(value == NULL) {
      write_all(client, "(null)\n");
    } else {
      char response[strlen(value) + 2];
      sprintf(response, "%s\n", value);
      write_all(client, response);
    }
    break;
  }
  case DELETE:
    avl_delete(db, command->key);
    write_all(client, "(ok!)\n");
    break;
  default:
    write_all(client, "(error)\n");
    pthread_mutex_unlock(&lock);
    return -1;
  }

  pthread_mutex_unlock(&lock);
  return 0;
}

/* ENTER */
void handle_connection(handler_t * handler) {
  int client = handler->client;
  avl_tree_t * db = handler->db;

  while(1) {
    char * raw_command = get_until_newline(client);

    if(raw_command == NULL) {
      printf("Client %d disconnected\n", client);
      break;
    }

    command_t * command = command_init();
    int status = parse_message(command, raw_command);

    if(status != -1) {
      int exec_status = exec_command(client, command, db);

      if(exec_status < 0) {
        printf("Command not executed!\n");
      }
    } else {
      write_all(client, "Command error!\n");
    }

    free(raw_command);
    free(command);
  }

  // Close connection
  close(client);
}

/* MUTEX INIT */
void handlers_init() {
  if(pthread_mutex_init(&lock, NULL) != 0){
    printf("Mutex init failed\n");
    return;
  }
}

void handlers_deinit() {
  pthread_mutex_destroy(&lock);
}
