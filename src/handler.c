#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "handler.h"
#include "parser.h"
#include "types.h"

// read_once reads from the socket a single time
size_t read_once(int client, char * buffer, size_t length) {
  ssize_t bytes_read = recv(client , buffer, (length - 1), 0);
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
    return -1;
  }

  return bytes_read;
}

static char * get_until_newline(int client) {
  char buffer[1025];
  size_t bytes_read = read_once(client, buffer, 1024);

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

/* ENTER */
void handle_connection(int client) {
  //  parse_message(NULL, NULL);
  char * raw_command = get_until_newline(client);
  printf("Recvd: %s", raw_command);

  command_t * command = command_init();
  int status = parse_message(command, raw_command);

  if(status != -1) {
    if(command->action == INSERT) {
      printf("Parsed: %d %s %s\n", command->action, command->key, command->value);
    } else {
      printf("Parsed: %d %s\n", command->action, command->key);
    }
  }

  free(raw_command);
  free(command);
  close(client);
}
