#include <stdio.h>

#include "expression.h"
#include "term.h"
#include "term_io.h"

#undef NDEBUG // FORCE ASSERT ACTIVATION

int main(void) {
  FILE *in = fopen("DATA/Terms/t_expression_0.term", "r");

  term t = term_scan(in);

  term_print_compact(t, stdout);
  fprintf(stdout, "%d", expression_valuate(t));
  printf("\n");

  term_destroy(&t);

  return 0;
}
