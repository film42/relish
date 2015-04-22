#include <stdio.h>
#include <stdlib.h>
#include <avl_tree.h>
#include <string.h>
#include "min_unit.h"

int tests_run = 0;

static void running_test(char * test_name) {
  printf("Running test: %s\n", test_name);
}

static char * avl_insert_tests() {
  avl_tree_t * tree  = avl_init();

  mu_assert( "Count is 0", avl_count(tree) == 0 );

  avl_insert(tree, "Hello!", "test", (sizeof(char) * 5));
  mu_assert( "Count is 1", avl_count(tree) == 1 );
  mu_assert( "Contains 'Hello!'", avl_contains(tree, "Hello!") );

  avl_insert(tree, "Hello!", "test", (sizeof(char) * 5));
  mu_assert( "Count is still 1", avl_count(tree) == 1 );

  avl_insert(tree, "Play!", "test", (sizeof(char) * 5));
  mu_assert( "Count is 2", avl_count(tree) == 2 );
  mu_assert( "Contains 'Play!'", avl_contains(tree, "Play!") );

  avl_insert(tree, "Mark", "test", (sizeof(char) * 5));
  avl_insert(tree, "fun", "test", (sizeof(char) * 5));
  avl_insert(tree, "hannah", "test", (sizeof(char) * 5));
  avl_insert(tree, "garrett", "test", (sizeof(char) * 5));

  mu_assert( "Count is 6", avl_count(tree) == 6 );

  avl_deinit(tree);
  return 0;
}

static char * avl_insert_weird_things_tests() {
  avl_tree_t * tree  = avl_init();

  typedef struct {
    int number;
  } example_t;

  example_t * thing = (example_t *)malloc(sizeof(example_t));
  thing->number = 1337;
  avl_insert(tree, "struct", thing, sizeof(example_t));
  free(thing);

  example_t * example = (example_t *)avl_get(tree, "struct");
  mu_assert( "Can access thing after free'd by callee", example->number == 1337 );

  avl_deinit(tree);
  return 0;
}

static char * avl_contains_tests() {
  avl_tree_t * tree  = avl_init();
  avl_insert(tree, "Hello!", "test", (sizeof(char) * 5));
  avl_insert(tree, "3", "test", (sizeof(char) * 5));
  avl_insert(tree, "4", "test", (sizeof(char) * 5));
  avl_insert(tree, "5", "test123", (sizeof(char) * 8));

  mu_assert( "Contains 'Hello!'", avl_contains(tree, "Hello!") );
  mu_assert( "Contains '3'", avl_contains(tree, "3") );
  mu_assert( "Contains '4'", avl_contains(tree, "4") );
  mu_assert( "Contains '5'", avl_contains(tree, "5") );
  mu_assert( "Correct key for '4' => 'test'", strcmp(avl_get(tree, "4"), "test") == 0);
  mu_assert( "Correct key for '5' => 'test123'", strcmp(avl_get(tree, "5"), "test123") == 0);
  mu_assert( "Incorrect key for '3' !=> '12test'", strcmp(avl_get(tree, "3"), "12test") != 0);
  mu_assert( "Doesn't contain 'Hllo'", !avl_contains(tree, "Hllo") );
  mu_assert( "Doesn't contain '7'", !avl_contains(tree, "7") );

  avl_deinit(tree);
  return 0;
}

static char * avl_delete_tests() {
  avl_tree_t * tree  = avl_init();
  avl_insert(tree, "1", "test", (sizeof(char) * 5));
  avl_insert(tree, "2", "test", (sizeof(char) * 5));
  avl_insert(tree, "3", "test", (sizeof(char) * 5));
  avl_insert(tree, "4", "test", (sizeof(char) * 5));
  avl_insert(tree, "5", "test", (sizeof(char) * 5));

  mu_assert( "Contains '3'", avl_contains(tree, "3") );
  avl_delete(tree, "3");
  mu_assert( "Value '3' was removed", !avl_contains(tree, "3") );

  mu_assert( "Contains '1'", avl_contains(tree, "1") );
  avl_delete(tree, "1");
  mu_assert( "Value '1' was removed", !avl_contains(tree, "1") );

  mu_assert( "Contains '5'", avl_contains(tree, "5") );
  avl_delete(tree, "5");
  mu_assert( "Value '5' was removed", !avl_contains(tree, "5") );

  mu_assert( "Contains '2'", avl_contains(tree, "2") );
  avl_delete(tree, "2");
  mu_assert( "Value '2' was removed", !avl_contains(tree, "2") );

  mu_assert( "Contains '4'", avl_contains(tree, "4") );
  avl_delete(tree, "4");
  mu_assert( "Value '4' was removed", !avl_contains(tree, "4") );

  mu_assert( "Tree is now empty", avl_count(tree) == 0);

  avl_deinit(tree);
  return 0;
}

static char * avl_endurance_tests() {
  avl_tree_t * tree  = avl_init();

  size_t i = 0;
  for(; i < 100000; ++i) {
    size_t length = (sizeof(char) * 6);
    char new_key[length];
    snprintf(new_key, length, "%zu", i);
    avl_insert(tree, new_key, "test", (sizeof(char) * 5));
  }

  mu_assert( "Contains '99000'", avl_contains(tree, "99000") );
  avl_delete(tree, "99000");
  mu_assert( "Value '99000' was removed", !avl_contains(tree, "99000") );

  avl_deinit(tree);
  return 0;
}

static char * avl_all_tests() {
  running_test("Insert Tests");
  mu_run_test(avl_insert_tests);
  running_test("Insert Weird Things Tests");
  mu_run_test(avl_insert_weird_things_tests);
  running_test("Contains Tests");
  mu_run_test(avl_contains_tests);
  running_test("Delete Tests");
  mu_run_test(avl_delete_tests);
  running_test("Endurance Tests");
  mu_run_test(avl_endurance_tests);
  return 0;
}

int main(int argc, char ** argv) {
  /* Run tests */
  char * result = avl_all_tests();
  if (result != 0) {
     printf("TEST FAILED: %s\n", result);
  }
  else {
     printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
