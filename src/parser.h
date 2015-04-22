#ifndef RELISH_PARSER
#define RELISH_PARSER

typedef enum {IDK=-1, INSERT, GET, DELETE} action_t;

typedef struct {
  action_t action;
  char   * key;
  char   * value;
} command_t;

command_t * command_init(void);
int parse_message(command_t * command, char * message);

#endif
