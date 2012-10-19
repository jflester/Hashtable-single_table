#include <stdio.h>
#include <string.h>
#include "hashtable.c"

static unsigned long correct_hash(const char *str);
static void test_hash(const char *str);

int main() {
  test_hash("ice\0");
  test_hash(NULL);
  test_hash("");
  return 0;
}

static unsigned long correct_hash(const char *str) {
  if (str == NULL || strcmp("\0", str) == 0){
    return 0;
  }
  else {
    return 451845518507;
  }
}

static void test_hash(const char *str) {
  unsigned long mine, correct;

  correct = correct_hash(str);
  mine = hash_code(str);
  if (mine == correct) {
    printf("PASSED!\n");
  }
  else {
    printf("FAILED. (Got %ld, should be %ld)\n", mine, correct);
  }
}