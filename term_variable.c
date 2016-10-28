#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "term_io.h"
#include "term_variable.h"

/*!
 * Check if variable is in valid format.
 * \param variable sstring the variable to test.
 * \return true if \c variable is in correct format to be a valid variable.
 */
static bool variable_is_valide(sstring variable) {
  if (sstring_get_length(variable) > 1) {
    if (sstring_get_char(variable, 0) == '\'') {
      for (int i = 1; i < sstring_get_length(variable); i++) {
        char c = sstring_get_char(variable, i);
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9' && i > 1) || c == '_')) {
          return false;
        }
      }
      return true;
    }
  }
  return false;
}

bool term_is_variable(term t) {
  bool result = variable_is_valide(term_get_symbol(t));
  if (result)
    assert(term_get_arity(t) == 0);
  return result;
}

void term_replace_if_variable(term t, sstring variable, term value) {
  assert(t != NULL);
  assert(value != NULL);
  assert(variable_is_valide(variable));
  if (sstring_compare(term_get_symbol(t), variable) == 0) {
    term_replace_copy(t, value);
  }
}

void term_replace_variable(term t, sstring variable, term value) {
  assert(t != NULL);
  assert(value != NULL);
  assert(variable_is_valide(variable));

  term_replace_if_variable(t, variable, value);

  term_argument_traversal tat = term_argument_traversal_create(t);
  while (term_argument_traversal_has_next(tat)) {
    term arg = term_argument_traversal_get_next(tat);
    term_replace_variable(arg, variable, value);
  }
  term_argument_traversal_destroy(&tat);
}
