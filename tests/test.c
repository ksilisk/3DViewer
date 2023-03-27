#include <check.h>
#include <stdlib.h>

int test_parser();

int main() {
  int no_failed = 0;

  no_failed |= test_parser();

  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
