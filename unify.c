#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

#undef NDEBUG // FORCE ASSERT ACTIVATION

#include "term_variable.h"
#include "unify.h"

/*! Symbol key-word for a unification term. */
static char const *const symbol_unify = "unify";

/*! Symbol key-word for an equality term. */
static char const *const symbol_equal = "=";

/*! Symbol key-word for a solution term. */
static char const *const symbol_solution = "solution";

/*! Symbol key-word for a value of a variable in a solution. */
static char const *const symbol_val = "val";

/*! Symbol key-word for sting an incompatible term (no solution). */
static char const *const symbol_incompatible = "incompatible";

#define RETURN_TERM_WITH_TWO_ARGUMENTS(term_symbol, leftTerm, rightTerm)       \
  term t = term_create(sstring_create_string(term_symbol));                    \
  term_add_argument_first(t, rightTerm);                                       \
  term_add_argument_first(t, leftTerm);                                        \
  return t;

static term term_create_imcompatible(const term leftTerm,
                                     const term rightTerm) {
  RETURN_TERM_WITH_TWO_ARGUMENTS(symbol_incompatible, leftTerm, rightTerm);
}

static term term_create_val(const term variable, const term value) {
  RETURN_TERM_WITH_TWO_ARGUMENTS(symbol_val, variable, value);
}

static term term_create_equality(const term leftTerm, const term rightTerm) {
  RETURN_TERM_WITH_TWO_ARGUMENTS(symbol_equal, leftTerm, rightTerm);
}

#define SET_RES_INCOMPATIBLE(res, leftTerm, rightTerm, bool_incompatible)      \
  term_destroy(&res);                                                          \
  res = term_create_imcompatible(leftTerm, rightTerm);                         \
  bool_incompatible = true;

#define TEST_TERM_IS_UNIFY(term)                                               \
  sstring stringUnify = sstring_create_string(symbol_unify);                   \
  assert(sstring_compare(term_get_symbol(t), stringUnify) == 0);               \
  assert(term_get_arity(t) > 0);                                               \
  sstring_destroy(&stringUnify);

#define TEST_TERM_IS_EQUALITY(term)                                            \
  sstring stringEquality = sstring_create_string(symbol_equal);                \
  assert(!sstring_compare(term_get_symbol(equality), stringEquality));         \
  assert(term_get_arity(equality) == 2);                                       \
  sstring_destroy(&stringEquality);

#define CREATE_EQUALITIES_FOR_TERMS_AND_ADD_THEM_TO(termA, termB, sequence)    \
  for (int i = 0; i < term_get_arity(termA); i++) {                            \
    term tLeft = term_extract_argument(termA, i);                              \
    term tRight = term_extract_argument(termB, i);                             \
    term newEquality = term_create_equality(tLeft, tRight);                    \
    term_add_argument_last(sequence, newEquality);                             \
  }

term term_unify(const term t) {
  TEST_TERM_IS_UNIFY(t);
  term res = term_create(sstring_create_string(symbol_solution));
  term sequenceToUnify = term_copy(t);
  bool incompatible = false;
  term_argument_traversal equalityTraversal =
      term_argument_traversal_create(sequenceToUnify);
  while (term_argument_traversal_has_next(equalityTraversal) && !incompatible) {
    term equality = term_argument_traversal_get_next(equalityTraversal);
    TEST_TERM_IS_EQUALITY(equality);
    term leftTerm = term_get_argument(equality, 0);
    term rightTerm = term_get_argument(equality, 1);
    sstring leftTermSymbol = term_get_symbol(leftTerm);
    sstring rightTermSymbol = term_get_symbol(rightTerm);
    if (term_is_variable(leftTerm) || term_is_variable(rightTerm)) {
      if (term_compare(leftTerm, rightTerm) == 0) {
        // If terms are equals, it's obvioulsy true, continue to next equality
        continue;
      } else if (term_contains_symbol(leftTerm, rightTermSymbol) ||
                 term_contains_symbol(rightTerm, leftTermSymbol)) {
        // If the variable is contained into the other term, the equality is
        // incoherent, so it is incompatible
        SET_RES_INCOMPATIBLE(res, leftTerm, rightTerm, incompatible);
      } else { // term left not in term right and terms not equal
        // So we get the value of this variable
        term variable;
        term value;
        if (term_is_variable(leftTerm)) {
          variable = leftTerm;
          value = rightTerm;
        } else {
          variable = rightTerm;
          value = leftTerm;
        }
        term tVal = term_create_val(variable, value);
        term_replace_variable(sequenceToUnify, term_get_symbol(tVal),
                              term_get_argument(tVal, 0));
        term_replace_variable(res, term_get_symbol(tVal),
                              term_get_argument(tVal, 0));
        term_add_argument_last(t, tVal);
      }
    } else { // No variables
      if (sstring_compare(leftTermSymbol, rightTermSymbol) ||
          term_get_arity(leftTerm) != term_get_arity(rightTerm)) {
        // If symbols different or arity different, it's incompatible
        SET_RES_INCOMPATIBLE(res, leftTerm, rightTerm, incompatible);
      } else {
        // Create an equality for each couple of arguments between both terms
        // and add it to the end of the term to unify
        CREATE_EQUALITIES_FOR_TERMS_AND_ADD_THEM_TO(leftTerm, rightTerm,
                                                    sequenceToUnify);
      }
    }
    // Loop end
  }
  term_argument_traversal_destroy(&equalityTraversal);
  return res;
}
