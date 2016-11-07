#include "peano.h"
#include "term_io.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

bool term_is_number(term t, int *n_pt) {
  sstring symbol = term_get_symbol(t);
  return sstring_is_integer(symbol, n_pt);
}
/*!
 * Recursive valuating function.
 * \param t term being valuated.
 * \return t peano term if its possible
 */
static term peano_valuate_inner(term t) {
  int *n_pt = (int *)malloc(sizeof(int));
  if (term_is_number(t, n_pt)) {
    sstring symbol = sstring_create_string("S");
    term_set_symbol(t, symbol);
    sstring_destroy(&symbol);
    symbol = sstring_create_string("0");
    term tmp = t;
    for (int i = 0; i < *n_pt; i++) {
      term_add_argument_first(tmp, term_create(term_get_symbol(t)));
      tmp = term_get_argument(tmp, 0);
    }
    term_set_symbol(tmp, symbol);
    sstring_destroy(&symbol);
  } else {
    term_argument_traversal tat = term_argument_traversal_create(t);
    while (term_argument_traversal_has_next(tat)) {
      term tmp = term_argument_traversal_get_next(tat);
      peano_valuate_inner(tmp);
    }
    term_argument_traversal_destroy(&tat);
  }
  free(n_pt);
  return t;
}
sstring symbol_if = NULL;
sstring symbol_zero = NULL;
/*!
* evaluate term with if
* \param t term to evaluate
* \return t term with replacement
*/
static term peano_compare(term t) {
  if (sstring_compare(term_get_symbol(t), symbol_if) == 0) {
    term first = term_get_argument(t, 0);
    if (sstring_compare(term_get_symbol(first), symbol_zero) == 0) {
      term_replace_copy(term_get_argument(t, 1), first);
    } else {
      term_replace_copy(term_get_argument(t, 2), first);
    }
    term_argument_traversal tat = term_argument_traversal_create(t);
    while (term_argument_traversal_has_next(tat)) {
      term tmp = term_argument_traversal_get_next(tat);
      peano_valuate_inner(tmp);
    }
    term_argument_traversal_destroy(&tat);
    return t;
  }
  return t;
}
/*!
 * int to char* function
 * \param val int to transform in char*
 * \return char* correspond to the int
 */
static char *int_to_pchar(int val) {
  char *char_symbol = (char *)malloc(sizeof(char) * 2);
  char_symbol[0] = val + '0';
  char_symbol[1] = '\0';
  return char_symbol;
}

term peano_valuate(term t) {
  assert(t != NULL);
  symbol_if = sstring_create_string("if");
  symbol_zero = sstring_create_string("0");
  int val = expression_valuate(t);
  char *char_symbol = int_to_pchar(val);
  sstring symbol = sstring_create_string(char_symbol);
  term t_copy = term_create(symbol);
  t_copy = peano_compare(t_copy);
  peano_valuate_inner(t_copy);
  sstring_destroy(&symbol);
  sstring_destroy(&symbol_if);
  sstring_destroy(&symbol_zero);
  free(char_symbol);
  return t_copy;
}
