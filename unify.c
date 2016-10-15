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
// TODO Terminate term_unify (create termsAreEqual, termInTerm)
term term_unify(term t) {
  sstring stringUnify = sstring_create_string(symbol_unify);
  sstring stringEquality = sstring_create_string(symbol_equal);
  assert(!sstring_compare(term_get_symbol(t), stringUnify));
  assert(term_get_arity(t) > 0);
  term_argument_traversal equalityTraversal = term_argument_traversal_create(t);
  while (term_argument_traversal_has_next(t)) {
    term equality = term_argument_traversal_get_next(t);
    assert(!sstring_compare(term_get_symbol(equality), stringEquality));
    assert(term_get_arity(equality) == 2);
    term leftTerm = term_get_argument(equality, 0);
    term rightTerm = term_get_argument(equality, 1);
    if (term_is_variable(leftTerm) || term_is_variable(rightTerm)) {
      if (termsAreEqual(leftTerm, rightTerm)) {
        continue;
      } else if (termInTerm(leftTerm, rightTerm)) {
        // Create a term incompatible with the two terms and return it
        // be careful, we have to terminate the loop (to free).
      } else { // term left not in term right and terms not equal
        // Create a term val with the variable and his value the other term
        // and add it to the result (term solution)
      }
    } else { // No variables
      if (sstring_compare(term_get_symbol(leftTerm),
                          term_get_symbol(rightTerm)) != 0 ||
          term_get_arity(leftTerm) != term_get_arity(rightTerm)) {
        // Create a term incompatible with the two terms and return it
        // be careful, we have to terminate the loop (to free).
      } else {
        // Create an equality for each couple of arguments between both terms
        // and add it to the end of the term to unify
      }
    }
  }
  sstring_destroy(&stringUnify);
  sstring_destroy(&stringEquality);
}
