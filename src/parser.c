#include "parser.h"
#include <stdio.h>

static int set_action(command_t * command, char * message) {
  if(strncmp(message, "get", 3) == 0) {
    command->action = GET;
    return 4;
  } else if(strncmp(message, "key", 3) == 0) {
    command->action = GET;
    return 4;
  } else if(strncmp(message, "set", 3) == 0) {
    command->action = INSERT;
    return 4;
  } else if(strncmp(message, "insert", 6) == 0) {
    command->action = INSERT;
    return 7;
  } else if(strncmp(message, "del", 3) == 0) {
    command->action = DELETE;
    return 4;
  } else {
    return -1;
  }
}

static int set_key(command_t * command, char * message, int index) {
  int next_space = index + 1;
  for(; next_space < strlen(message); ++next_space) {
    if(message[next_space] == ' ' || message[next_space] == '\n') {
      goto found;
    }
  }

  return -1;

 found: {
    size_t key_length = (next_space - index);
    char * key_buffer = (char *)malloc(sizeof(char) * (key_length + 1));
    strncpy(key_buffer, (message + index), key_length);
    command->key = key_buffer;
    // printf("key buffer: [%d, %d : %d] '%s'\n", index, next_space, key_length, key_buffer);
    return next_space + 1;
  }
}

static int set_value(command_t * command, char * message, int index) {
  int next_space = index + 1;
  for(; next_space < strlen(message); ++next_space) {
    if(message[next_space] == '\n') {
      goto found;
    }
  }

  return -1;

 found: {
    size_t value_length = (next_space - index);
    char * value_buffer = (char *)malloc(sizeof(char) * (value_length + 1));
    strncpy(value_buffer, (message + index), value_length);
    command->value = value_buffer;
    // printf("value buffer: [%d, %d : %d] '%s'\n", index, next_space, value_length, value_buffer);
    return next_space + 1;
  }
}

command_t * command_init() {
  command_t * command = (command_t *)malloc(sizeof(command_t));
  command->action = IDK;
  command->key = NULL;
  command->value = NULL;

  return command;
}

int parse_message(command_t * command, char * message) {
  int cursor;

  cursor = set_action(command, message);
  if(cursor < 0) {
    printf("Error setting action\n");
    return -1;
  }

  cursor = set_key(command, message, cursor);
  if(cursor < 0) {
    printf("Error setting key\n");
    return -1;
  }

  if(command->action == INSERT) {
    cursor = set_value(command, message, cursor);
    if(cursor < 0) {
      printf("Error setting value\n");
      return -1;
    }
  }

  return 0;
}
