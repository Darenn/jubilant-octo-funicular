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

/*!
* \brief Create a term, add leftTerm and rightTerm as arguments and return it.
*/
#define RETURN_TERM_WITH_TWO_ARGUMENTS(term_symbol, leftTerm, rightTerm)       \
  sstring symbol = sstring_create_string(term_symbol);                         \
  term t = term_create(symbol);                                                \
  sstring_destroy(&symbol);                                                    \
  term_add_argument_first(t, term_copy(rightTerm));                            \
  term_add_argument_first(t, term_copy(leftTerm));                             \
  return t;

/*!
* \brief Create an incompatible term with terms given as arguments.
* \param leftTerm The leftTerm of the incompatible term.
* \param rightTerm The rightTerm of the incompatible term.
* \return The term incompatible.
*/
static term term_create_imcompatible(const term leftTerm,
                                     const term rightTerm) {
  RETURN_TERM_WITH_TWO_ARGUMENTS(symbol_incompatible, leftTerm, rightTerm);
}

/*!
* \brief Create a val term with the given terms.
* \param variable The variable.
* \param value The value of the variable.
* \return A val term with value as argument of variable.
*/
static term term_create_val(const term variable, const term value) {
  RETURN_TERM_WITH_TWO_ARGUMENTS(symbol_val, variable, value);
}

/*!
* \brief Create an equality with the given terms.
* \param leftTerm The left term of the equality.
* \param rightTerm The right term of the equality.
* \return An equality with the given terms as arguments.
*/
static term term_create_equality(const term leftTerm, const term rightTerm) {
  RETURN_TERM_WITH_TWO_ARGUMENTS(symbol_equal, leftTerm, rightTerm);
}

/*!
* \brief Set the result in the unify function with an incompatible term.
*/
#define SET_RES_INCOMPATIBLE(res, leftTerm, rightTerm, bool_incompatible)      \
  term_destroy(&res);                                                          \
  res = term_create_imcompatible(term_copy(leftTerm), term_copy(rightTerm));   \
  bool_incompatible = true;

/*!
* \brief Make assertions to test if a term is a unify.
*/
#define TEST_TERM_IS_UNIFY(term)                                               \
  sstring stringUnify = sstring_create_string(symbol_unify);                   \
  assert(sstring_compare(term_get_symbol(t), stringUnify) == 0);               \
  assert(term_get_arity(t) > 0);                                               \
  sstring_destroy(&stringUnify);

/*!
* \brief Make assertions to test if a term is an equality.
*/
#define TEST_TERM_IS_EQUALITY(term)                                            \
  sstring stringEquality = sstring_create_string(symbol_equal);                \
  assert(!sstring_compare(term_get_symbol(equality), stringEquality));         \
  assert(term_get_arity(equality) == 2);                                       \
  sstring_destroy(&stringEquality);

/*!
* \brief Create equalities between each index corresponding arguments of both
* terms and add these equalities as argument of the given sequence (term).
*/
#define CREATE_EQUALITIES_FOR_TERMS_AND_ADD_THEM_TO(termA, termB, sequence)    \
  for (int i = 0; i < term_get_arity(termA); i++) {                            \
    term tLeft = term_copy(term_get_argument(termA, i));                       \
    term tRight = term_copy(term_get_argument(termB, i));                      \
    term newEquality = term_create_equality(tLeft, tRight);                    \
    term_add_argument_last(sequence, newEquality);                             \
  }

/*!
* \brief Check if one of the term is contained in the other.
* \param termA The first term.
* \param termB The second term.
* \return true if one the term is contained in the other, false otherwise.
*/
static bool one_term_contains_the_other(const term termA, const term termB) {
  return term_contains_symbol(termA, term_get_symbol(termB)) ||
         term_contains_symbol(termB, term_get_symbol(termA));
}

/*!
* \brief Check which term is a variable, and return a term with the variable
* symbol as symbol and the other term as argument
* \param termA The first term.
* \param termB The second term.
* \pre Just one of the term is a variable.
* \return The term with the variable symbol as symbol and the other terme as
* argument.
*/
static term term_create_val_for_variable(term termA, term termB) {
  term variable;
  term value;
  if (term_is_variable(termA)) {
    variable = termA;
    value = termB;
  } else {
    variable = termB;
    value = termA;
  }
  return term_create_val(term_copy(variable), term_copy(value));
}

term term_unify(const term t) {
  TEST_TERM_IS_UNIFY(t);
  sstring resSymbol = sstring_create_string(symbol_solution);
  term res = term_create(resSymbol);
  sstring_destroy(&resSymbol);
  term sequenceToUnify = term_copy(t);
  term nextSequenceToUnify = term_create(sstring_create_string(symbol_unify));
  bool incompatible = false;
  term_argument_traversal equalityTraversal =
      term_argument_traversal_create(sequenceToUnify);
  while (term_argument_traversal_has_next(equalityTraversal) && !incompatible) {
    term equality = term_argument_traversal_get_next(equalityTraversal);
    TEST_TERM_IS_EQUALITY(equality);
    term leftTerm = term_get_argument(equality, 0);
    term rightTerm = term_get_argument(equality, 1);
    if (term_is_variable(leftTerm) || term_is_variable(rightTerm)) {
      if (term_compare(leftTerm, rightTerm) == 0) {
        // If terms are equals, it's obvioulsy true, continue to next equality
        continue;
      } else if (one_term_contains_the_other(leftTerm, rightTerm)) {
        // If the variable is contained into the other term, the equality is
        // incoherent, so it is incompatible
        SET_RES_INCOMPATIBLE(res, leftTerm, rightTerm, incompatible);
      } else { // term left not in term right and terms not equal
        // So we get the value of this variable and replace it
        term tVal = term_create_val_for_variable(leftTerm, rightTerm);
        term_replace_variable(sequenceToUnify,
                              term_get_symbol(term_get_argument(tVal, 0)),
                              term_get_argument(tVal, 1));
        term_replace_variable(nextSequenceToUnify,
                              term_get_symbol(term_get_argument(tVal, 0)),
                              term_get_argument(tVal, 1));
        term_replace_variable(res, term_get_symbol(term_get_argument(tVal, 0)),
                              term_get_argument(tVal, 1));
        term_add_argument_last(res, tVal);
      }
    } else { // No variables
      if (sstring_compare(term_get_symbol(leftTerm),
                          term_get_symbol(rightTerm)) ||
          term_get_arity(leftTerm) != term_get_arity(rightTerm)) {
        // If symbols different or arity different, it's incompatible
        SET_RES_INCOMPATIBLE(res, leftTerm, rightTerm, incompatible);
      } else {
        // Create an equality for each couple of arguments between both terms
        // and add it to the end of the term to unify
        CREATE_EQUALITIES_FOR_TERMS_AND_ADD_THEM_TO(leftTerm, rightTerm,
                                                    nextSequenceToUnify);
      }
    }
    if (!term_argument_traversal_has_next(equalityTraversal) &&
        term_get_arity(nextSequenceToUnify) > 0) {
      sequenceToUnify = term_copy(nextSequenceToUnify);
      term_argument_traversal_destroy(&equalityTraversal);
      equalityTraversal = term_argument_traversal_create(sequenceToUnify);
      term_destroy(&nextSequenceToUnify);
      nextSequenceToUnify = term_create(sstring_create_string(symbol_unify));
    }
  }
  term_destroy(&sequenceToUnify);
  term_destroy(&nextSequenceToUnify);
  term_argument_traversal_destroy(&equalityTraversal);
  return res;
}
