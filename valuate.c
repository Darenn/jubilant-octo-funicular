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
 * Recursive valuating function.
 * \param t term being valuated.
 * \param t_res valuated variable stack (list of active variables).
 */
static term term_valuate_inner(term t, variable_definition_list var_list) {
  // Chercher caractère " ' " dans t->symbol->chars
  if (sstring_compare(term_get_symbol(t), ss_set)) {
    if (term_get_arity(t)) {
      for (int i = 0; i < term_get_arity(t); i++) {
        term_valuate_inner(term_get_argument(t, i), var_list);
      }
    }

    return t;
  }

  /*Si la même variable est redéfinie par un set dans un sous-terme, cela
entraîne le remplacement par la nouvelle valeur pour le sous-terme. Pour gérer
cela, les variables à remplacer sont empilées : une nouvelle déclaration
masque
alors les plus anciennes. Attention dans la valuation, il faut traiter le cas
où
un remplacement introduit de nouvelles variables à remplacer ou de nouvelles
dénitions. Ces variables ne sont pas remplacées quand on découvre le set
mais quand on fait le remplacement de la variable. Pour éviter les
remplacements
à l'infini, on doit s'assurer que < variable > n'apparaît pas dans < valeur
>
(donc pas de set ( 'a R( 'a ) BB ) ).*/

  return NULL;
}

term term_valuate(term t) {
  assert(t != NULL);
  term new_t = t;
  ss_set = sstring_create_string(symbol_set);
  variable_definition_list va_list = (variable_definition_list)malloc(
      sizeof(struct variable_definition_list_struct));
  sstring_destroy(&ss_set);
  return term_valuate_inner(new_t, va_list);
}
