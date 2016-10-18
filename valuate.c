#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#undef NDEBUG // FORCE ASSERT ACTIVATION

#include "term_variable.h"
#include "valuate.h"

/*! Symbol key-word for setting a variable term. */
static char const *const symbol_set = "set";

/*!
 * Used as a stack to record the values of the variable being replaced.
 */
typedef struct variable_definition_list_struct {
  /*! variable symbol */
  sstring variable;
  /*! variable value */
  term value;
  /*! link to next valuation */
  struct variable_definition_list_struct *next;
} * variable_definition_list;

/*!
 * Used to store the symbol for setting a variable.
 * Global variable to avoid to have to redefined it at every call to \c
 * term_valuate_inner
 */
static sstring ss_set = NULL;

static variable_definition_list add_var_list(variable_definition_list var_list,
                                             term term_var, term term_value) {
  sstring symbol = term_get_symbol(term_var);
  while (var_list != NULL) {
    if (sstring_compare(var_list->variable, symbol) == 0) {
      return var_list;
    }
    var_list = var_list->next;
  }
  var_list = (variable_definition_list)malloc(
      sizeof(struct variable_definition_list_struct));
  var_list->variable = term_get_symbol(term_var);
  var_list->value = term_value;
  return var_list;
}
/*!
 * Recursive valuating function.
 * \param t term being valuated.
 * \param t_res valuated variable stack (list of active variables).
 */
static term term_valuate_inner(term t, variable_definition_list var_list) {
  if (sstring_compare(term_get_symbol(t), ss_set) == 0) {
    if (term_get_arity(t) > 0) {
      for (int i = 0; i < term_get_arity(t); i++) {
        term arg = term_get_argument(t, i);
        if (sstring_compare(term_get_symbol(term_get_father(arg)), ss_set) ==
            0) {
          if (term_is_variable(arg)) {
            var_list = add_var_list(var_list, arg, term_get_argument(t, i + 1));
            term_replace_variable(t, var_list->variable, var_list->value);
          }
        }
        term_valuate_inner(term_get_argument(t, i), var_list);
      }

      return t;
    }
  }
  return t;
}

term term_valuate(term t) {
  assert(t != NULL);
  term new_t = t;
  ss_set = sstring_create_string(symbol_set);
  variable_definition_list va_list = NULL;
  sstring_destroy(&ss_set);
  return term_valuate_inner(new_t, va_list);
}
