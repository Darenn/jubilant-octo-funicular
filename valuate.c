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
static void destroyVDL(variable_definition_list *ls) {
  if (*ls != NULL) {
    variable_definition_list last = *ls;
    variable_definition_list current = ((*ls)->next);
    while (current != NULL) {
      term_destroy(&(current->value));
      sstring_destroy(&(current->variable));
      free(last);
      last = current;
      current = (current->next);
    }
    free(last);
  }
}
void pushBackVDL(variable_definition_list *ls, term variable, term value) {
  variable_definition_list ils = (variable_definition_list)malloc(
      sizeof(struct variable_definition_list_struct));
  ils->value = (value);
  ils->variable = (term_get_symbol(variable));
  ils->next = NULL;
  if (*ls != NULL) {
    variable_definition_list tmp = *ls;
    while (tmp->next != NULL) {
      tmp = tmp->next;
    }
    tmp->next = ils;
  } else {
    *ls = ils;
  }
}
variable_definition_list *getLast(variable_definition_list *ls) {
  int i = 0;
  variable_definition_list *tmp = ls;
  while ((*tmp)->next != NULL) {
    (*tmp) = (*tmp)->next;
    i++;
  }
  for (int j = 0; j < i; j++) {
    (*ls) = (*ls)->next;
  }
  (*ls)->next = NULL;

  return tmp;
}

/*!
 * Recursive valuating function.
 * \param t term being valuated.
 * \param t_res valuated variable stack (list of active variables).
 */
static term term_valuate_inner(term t, variable_definition_list var_list) {
  if (sstring_compare(term_get_symbol(t), ss_set) == 0) {
    if (term_get_arity(t) == 3) {
      term variable = term_get_argument(t, 0);
      term value = term_get_argument(t, 1);
      if (term_is_variable(variable)) {
        pushBackVDL(&var_list, variable, value);
      }
    }
  }

  for (int i = 0; i < term_get_arity(t); i++) {
    term tmp = term_extract_argument(t, i);
    term arg = term_valuate_inner(tmp, var_list);
    term_add_argument_position(t, arg, i);
  }
  if (sstring_compare(term_get_symbol(t), ss_set) == 0) {
    if (term_get_arity(t) == 3) {
      term variable = term_get_argument(t, 0);
      term arg = term_get_argument(t, 2);
      if (term_is_variable(variable)) {
        variable_definition_list *vdl = getLast(&var_list);
        term_replace_variable(arg, (*vdl)->variable, (*vdl)->value);
        destroyVDL(vdl);
        return arg;
      }
    }
  }
  return t;
}

term term_valuate(term t) {
  assert(t != NULL);
  term new_t = term_copy(t);
  ss_set = sstring_create_string(symbol_set);
  variable_definition_list va_list = NULL;
  new_t = term_valuate_inner(new_t, va_list);
  destroyVDL(&va_list);
  sstring_destroy(&ss_set);
  return new_t;
}
