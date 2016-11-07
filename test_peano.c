#include <stdio.h>

#include "peano.h"
#include "term.h"
#include "term_io.h"

#undef NDEBUG // FORCE ASSERT ACTIVATION

static void test_file(char const *const file_name) {
  FILE *in = fopen(file_name, "r");
  term t = term_scan(in);
  term t_copy = peano_valuate(t);
  term_print_compact(t_copy, stdout);
  printf("\n");
  term_destroy(&t);
  term_destroy(&t_copy);
  fclose(in);
}

int main(void) {
  test_file("DATA/Terms/t_peano_0.term");
  test_file("DATA/Terms/t_peano_1.term");

  return 0;
}
