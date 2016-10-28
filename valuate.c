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
    term_destroy(&((*ls)->value));
    sstring_destroy(&((*ls)->variable));
    destroyVDL(&(*ls)->next);
    free(*ls);
    *ls = NULL;
  }
}
static variable_definition_list pushBackVDL(variable_definition_list ls,
                                            sstring variable, term value) {
  variable_definition_list vdl = (variable_definition_list)malloc(
      sizeof(struct variable_definition_list_struct));
  vdl->value = term_copy(value);
  vdl->variable = sstring_copy(variable);
  vdl->next = ls;
  return vdl;
}

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
  if (term_is_set(t)) {
    term variable = term_extract_argument(t, 0);
    term value = term_extract_argument(t, 0);
    term arg = term_extract_argument(t, 0);
    var_list = pushBackVDL(var_list, term_get_symbol(variable), value);
    term_destroy(&variable);
    term_destroy(&value);
    term_destroy(&t);
    t = term_valuate_inner(arg, var_list);
  }

  variable_definition_list vdl = var_list;
  while (vdl != NULL) {
    term_replace_variable(t, vdl->variable, vdl->value);
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
  variable_definition_list var_list = NULL;
  t_copy = term_valuate_inner(t_copy, var_list);
  destroyVDL(&var_list);
  sstring_destroy(&ss_set);
  return t_copy;
}
