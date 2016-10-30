#include <stdio.h>

#include "expression.h"
#include "term.h"
#include "term_io.h"

#undef NDEBUG // FORCE ASSERT ACTIVATION

static void test_file(char const *const file_name) {
  FILE *in = fopen(file_name, "r");
  term t = term_scan(in);
  term_print_compact(t, stdout);
  printf("\n");
  fprintf(stdout, "%d", expression_valuate(t));
  printf("\n");
  term_destroy(&t);
  fclose(in);
}

int main(void) {
  test_file("DATA/Terms/t_expression_0.term");
  test_file("DATA/Terms/t_expression_1.term");
  return 0;
}
