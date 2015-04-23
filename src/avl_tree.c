#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avl_tree.h>

#define bool size_t
#define true 1
#define false 0

#ifndef max
#define max(a, b) (a > b ? a : b)
#endif

static avl_node_t * avl_node_init() {
  avl_node_t * node = (avl_node_t *)malloc(sizeof(avl_node_t));
  node->key = NULL;
  node->value = NULL;
  node->child_left = NULL;
  node->child_right = NULL;
  node->height = 0;
  return node;
}

avl_tree_t * avl_init( void ) {
  avl_tree_t * tree = (avl_tree_t *)malloc(sizeof(avl_tree_t));
  tree->head = NULL;
  tree->count = 0;
  return tree;
}

static void avl_node_deinit( avl_node_t * node ) {
  free(node->key);
  free(node->value);
  free(node);
}

static void deinit_recur( avl_node_t * node ) {
  if(node->child_left != NULL) {
    deinit_recur(node->child_left);
  }
  if(node->child_right != NULL) {
    deinit_recur(node->child_right);
  }

  avl_node_deinit( node );
}

void avl_deinit( avl_tree_t * tree ) {
  if(tree == NULL) {
    return;
  }
  if(tree->head != NULL) {
    deinit_recur(tree->head);
  }

  free(tree);
}

static ssize_t get_height( avl_node_t * node ) {
  return node == NULL ? 0 : node->height;
}

static void set_height( avl_node_t * node ) {
  size_t child_left_height = get_height(node->child_left);
  size_t child_right_height = get_height(node->child_right);
  node->height = max( child_left_height, child_right_height );
  /* Now increment */
  node->height++;
}

static avl_node_t * rotate_right( avl_node_t * node ) {
  avl_node_t * temp_node = node->child_left;

  node->child_left = temp_node->child_right;
  temp_node->child_right = node;
  node = temp_node;

  set_height(node->child_right);
  set_height(node);
  return node;
}

static avl_node_t * rotate_left( avl_node_t * node ) {
  avl_node_t * temp_node = node->child_right;

  node->child_right = temp_node->child_left;
  temp_node->child_left = node;
  node = temp_node;

  set_height(node->child_left);
  set_height(node);
  return node;
}

static avl_node_t * balance_node( avl_node_t * node ) {
  ssize_t balance = get_height(node->child_left) - get_height(node->child_right);

  if( balance > 1 ) {
    /* Balance right */
    if( get_height(node->child_left->child_right) > get_height(node->child_left->child_left) ) {
      node->child_left = rotate_left( node->child_left );
    }

    return rotate_right( node );

  } else if( balance < -1 ) {
    /* Blanace left */
    if( get_height(node->child_right->child_left) > get_height(node->child_right->child_right) ) {
      node->child_right = rotate_right( node->child_right );
    }

    return rotate_left( node );
  }

  return node;
}

static avl_node_t * get_recur( avl_node_t * node, char * key ) {
  if(node == NULL) {
    return NULL;
  }

  int comparison = strcmp(node->key, key);

  /* Matching */
  if( comparison == 0 ) {
    return node;
  }

  /* Walk down a specific branch */
  if( comparison > 0) {
    return get_recur( node->child_left, key );
  }
  else {
    return get_recur( node->child_right, key );
  }
}

void * avl_get( avl_tree_t * tree, void * key ) {
  avl_node_t * node = get_recur( tree->head, key );

  if( node != NULL ) {
    return node->value;
  } else {
    return NULL;
  }
}

/* I don't like this implementation, so fix it later */
static avl_node_t * insert_recur( avl_node_t * node, char * key, void * value ) {
  /* Return new node if node is NULL */
  if(node == NULL) {
    /* Create new node */
    avl_node_t * new_node = avl_node_init();

    new_node->key = key;
    new_node->value = value;
    return new_node;
  }

  /* Otherwise recursively assign the new values */
  else if( strcmp(node->key, key) >= 1 ) {
    node->child_left = insert_recur( node->child_left, key, value );
  } else {
    node->child_right = insert_recur( node->child_right, key, value );
  }

  /* Finish insert by balancing the tree at `node' */
  set_height(node);
  node = balance_node(node);
  return node;
}

void avl_insert( avl_tree_t * tree, char * key, void * value, int value_size) {
  /* Copy key/ value because we can't use stack pointers */
  size_t length = strlen(key);
  char * m_key = malloc( sizeof(char) * (length + 1) );
  strcpy(m_key, key);
  void * m_value = malloc( value_size );
  memcpy(m_value, value, value_size);

  /* Check to see if we already have the string */
  avl_node_t * node = get_recur( tree->head, key );
  if(node != NULL) {
    free(node->value);
    node->value = m_value;
    return;
  }

  tree->head = insert_recur(tree->head, m_key, m_value);
  tree->count++;
}

/* Get rid of this crap */
static avl_node_t * find_small_in_right( avl_node_t * node ) {
  if(node == NULL) return NULL;
  if(find_small_in_right(node->child_left) != NULL) {
    return find_small_in_right(node->child_left);
  }

  return node;
}

static avl_node_t * delete_recur( avl_tree_t * tree, avl_node_t * node, char * key ) {
  int comparison = strcmp(node->key, key);
  avl_node_t * marked_node;

  /* Matching */
  if( comparison == 0 ) {
    /* Check for leaf node */
    if( node->child_left == NULL && node->child_right == NULL ) {
      /* Remove */
      marked_node = node;
      node = NULL;
    }
    /* Is parent to a left tree */
    else if( node->child_right == NULL && node->child_left != NULL ) {
      marked_node = node;
      node = node->child_left;
    }
    /* Is parent to a right tree */
    else if( node->child_right != NULL && node->child_left == NULL ) {
      marked_node = node;
      node = node->child_right;
    }
    /* Is parent to both left and right trees */
    else {
      marked_node = find_small_in_right(node->child_right);

      node->key = marked_node->key;
      node->value = marked_node->value;
      node->child_right = delete_recur(tree, node->child_right, node->key);
    }

    /* Free the node */
    avl_node_deinit( marked_node );
    tree->count--;
  }

  /* Walk down a specific tree */
  else if( comparison >= 1 && node->child_left != NULL ) {
    node->child_left = delete_recur(tree, node->child_left, key );
  }
  else if( comparison <= -1 && node->child_right != NULL ) {
    node->child_right = delete_recur(tree, node->child_right, key );
  }

  if(node != NULL) {
    set_height(node);
    node = balance_node(node);
  }

  return node;
}

void avl_delete( avl_tree_t * tree, char * key ) {
  if(tree->head == NULL) return;

  tree->head = delete_recur(tree, tree->head, key);
}

size_t avl_contains( avl_tree_t * tree, char * key ) {
  if(avl_get(tree, key)) {
    return 1;
  } else {
    return 0;
  }
}

size_t avl_count( avl_tree_t * tree ) {
  return tree->count;
}

static void print_recur( avl_node_t * node ) {
  if( node != NULL ) {
    printf("%zu  %s\n", node->height, node->key);
    print_recur( node->child_left );
    print_recur( node->child_right );
  }
}

void avl_print( avl_tree_t * tree ) {
  if(!tree->head) {
    printf("Tree is empty\n");
  } else {
    print_recur( tree->head );
  }
}
