#ifndef RELISH_HANDLER
#define RELISH_HANDLER

#include "avl_tree.h"

typedef struct {
  int          client;
  avl_tree_t * db;
} handler_t;

void handlers_init();
void handlers_deinit();
void handle_connection(handler_t * handler);

#endif
