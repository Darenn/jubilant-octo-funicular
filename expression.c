#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "expression.h"

#undef NDEBUG // FORCE ASSERT ACTIVATION

// valid symbols
static char const *const symbol_plus = "+";
static char const *const symbol_minus = "-";
static char const *const symbol_product = "*";
static char const *const symbol_divided = "/";
static char const *const symbol_and = "&&";
static char const *const symbol_or = "||";
static char const *const symbol_not = "!";
static char const *const symbol_bool_T = "true";
static char const *const symbol_bool_F = "false";

static int const nb_symbols = 7;
static char const *const symbols[] = {"+", "-", "*", "/", "&&", "||", "!"};

/* Check if symbol is valid
 * \return true if symbol is valid
 */
static bool symbol_is_valid(term t) {
  bool res = term_get_arity(t) > 0;

  for (int i = 0; i < nb_symbols; i++) {
    sstring ss = sstring_create_string(symbols[i]);
    res = res || (sstring_compare(term_get_symbol(t), ss) == 0);
    sstring_destroy(&ss);
  }

  return res;
}

/* Check if symbol is a boolean
 * \return true if symbol is a boolean
 */
static bool symbol_is_boolean(sstring s) {
  sstring T = sstring_create_string(symbol_bool_T);
  sstring F = sstring_create_string(symbol_bool_F);

  bool res = (sstring_compare(s, T) == 0) || (sstring_compare(s, F) == 0);

  sstring_destroy(&T);
  sstring_destroy(&F);

  return res;
}

bool term_is_valid_expression(term t) {
  assert(t != NULL);

  sstring s = term_get_symbol(t);
  bool res = false;
  int i = 0;

  if (symbol_is_valid(t) || sstring_is_integer(s, &i) || symbol_is_boolean(s)) {
    res = true;
    term_argument_traversal tat = term_argument_traversal_create(t);
    while (term_argument_traversal_has_next(tat)) {
      if (!term_is_valid_expression(term_argument_traversal_get_next(tat))) {
        res = false;
      }
    }
    term_argument_traversal_destroy(&tat);
  }

  return res;
}

// SString symbols
sstring plus;
sstring minus;
sstring product;
sstring divided;
sstring and;
sstring or ;
sstring not;
sstring T;

/* Return valuate term */
int expression_valuate_inner(term t) {
  assert(t != NULL);
  assert(term_is_valid_expression(t));

  sstring s = term_get_symbol(t);
  int res = 0;
  if (sstring_is_integer(s, &res))
    return res;

  if (symbol_is_boolean(s)) {
    if (sstring_compare(s, T) == 0) {
      return 1;
    } else {
      return 0;
    }
  }

  int arg_res[term_get_arity(t)];
  for (int i = 0; i < term_get_arity(t); i++) {
    arg_res[i] = expression_valuate_inner(term_get_argument(t, i));
  }

  if (sstring_compare(s, plus) == 0) {
    for (int i = 0; i < term_get_arity(t); i++) {
      res += arg_res[i];
    }
  } else if (sstring_compare(s, minus) == 0) {
    for (int i = 0; i < term_get_arity(t); i++) {
      res -= arg_res[i];
    }
  } else if (sstring_compare(s, product) == 0) {
    res = 1;
    for (int i = 0; i < term_get_arity(t); i++) {
      res *= arg_res[i];
    }
  } else if (sstring_compare(s, divided) == 0) {
    res = arg_res[0];
    for (int i = 1; i < term_get_arity(t); i++) {
      res /= arg_res[i];
    }
  } else if (sstring_compare(s, and) == 0) {
    res = 1;
    for (int i = 0; i < term_get_arity(t); i++) {
      res = res && arg_res[i];
    }
  } else if (sstring_compare(s, or) == 0) {
    for (int i = 0; i < term_get_arity(t); i++) {
      res = res || arg_res[i];
    }
  } else if (sstring_compare(s, not) == 0) {
    res = !arg_res[0];
  }

  return res;
}

int expression_valuate(term t) {
  assert(t != NULL);
  assert(term_is_valid_expression(t));

  plus = sstring_create_string(symbol_plus);
  minus = sstring_create_string(symbol_minus);
  product = sstring_create_string(symbol_product);
  divided = sstring_create_string(symbol_divided);
  and = sstring_create_string(symbol_and);
  or = sstring_create_string(symbol_or);
  not = sstring_create_string(symbol_not);
  T = sstring_create_string(symbol_bool_T);

  int res = expression_valuate_inner(t);

  sstring_destroy(&plus);
  sstring_destroy(&minus);
  sstring_destroy(&product);
  sstring_destroy(&divided);
  sstring_destroy(&and);
  sstring_destroy(& or);
  sstring_destroy(&not);
  sstring_destroy(&T);

  return res;
}
