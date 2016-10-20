#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "term_variable.h"

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

void term_replace_variable(term t, sstring variable, term value) {
  assert(t != NULL);
  assert(value != NULL);
  assert(variable_is_valide(variable));
  for (int i = 0; i < term_get_arity(t); i++) {
    term arg = term_get_argument(t, i);
    term_replace_variable(arg, variable, value);
  }
  if (sstring_compare(term_get_symbol(t), variable)) {
    term_replace_copy(t, value);
  }
}
