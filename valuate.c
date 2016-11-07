#include "term_io.h"
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
/*!
 * pop variable of a list function
 * \param ls pointer on a variable stack being reduced.
 */
static void pop_variable(variable_definition_list *ls) {
  assert(ls != NULL);
  if (*ls != NULL) {
    term_destroy(&((*ls)->value));
    sstring_destroy(&((*ls)->variable));
    variable_definition_list next = (*ls)->next;
    free(*ls);
    *ls = next;
  }
}
/*!
 * push back in a list function
 * \param ls pointer on a variable stack being modified.
 * \param  variable sstring variable to add in the stack
 * \param  value term variable to add in the stack
 */
static void push_back_variable(variable_definition_list *ls, sstring variable,
                               term value) {
  variable_definition_list vdl = (variable_definition_list)malloc(
      sizeof(struct variable_definition_list_struct));
  vdl->value = term_copy(value);
  vdl->variable = sstring_copy(variable);
  vdl->next = *ls;
  *ls = vdl;
}
/*!
 * term is set function
 * \param t term to check
 * \return bool to know if term is a right set
 */
static bool term_is_set(term t) {
  return (sstring_compare(term_get_symbol(t), ss_set) == 0) &&
         (term_get_arity(t) == 3) &&
         (term_is_variable(term_get_argument(t, 0)));
}

/*!
 * Recursive valuating function.
 * \param t term being valuated.
 * \param t_res valuated variable stack (list of active variables).
 */
static term term_valuate_inner(term t, variable_definition_list var_list) {
  assert(t != NULL);
  if (term_is_set(t)) {
    term variable = term_extract_argument(t, 0);
    term value = term_extract_argument(t, 0);
    term arg = term_extract_argument(t, 0);

    push_back_variable(&var_list, term_get_symbol(variable), value);

    term_destroy(&variable);
    term_destroy(&value);
    term_destroy(&t);
    t = term_valuate_inner(arg, var_list);

    pop_variable(&var_list);
  }

  variable_definition_list vdl = var_list;
  while (vdl != NULL) {
    term_replace_if_variable(t, vdl->variable, vdl->value);
    vdl = vdl->next;
  }
  for (int i = 0; i < term_get_arity(t); i++) {
    term tmp = term_extract_argument(t, i);
    term arg = term_valuate_inner(tmp, var_list);
    term_add_argument_position(t, arg, i);
  }

  return t;
}

term term_valuate(term t) {
  assert(t != NULL);
  term t_copy = term_copy(t);
  ss_set = sstring_create_string(symbol_set);
  variable_definition_list va_list = NULL;
  t_copy = term_valuate_inner(t_copy, va_list);
  sstring_destroy(&ss_set);
  return t_copy;
}
