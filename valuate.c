# include <assert.h>
# include <stdbool.h>
#include <ctype.h>

# undef NDEBUG   // FORCE ASSERT ACTIVATION

# include "term_variable.h"
# include "valuate.h"


/*! Symbol key-word for setting a variable term. */
static char const * const symbol_set = "set" ;


/*!
 * Used as a stack to record the values of the variable being replaced.
 */
typedef struct variable_definition_list_struct {
  /*! variable symbol */
  sstring variable ;
  /*! variable value */
  term value ;
  /*! link to next valuation */
  struct variable_definition_list_struct * next ;
}  * variable_definition_list ;


/*!
 * Used to store the symbol for setting a variable.
 * Global variable to avoid to have to redefined it at every call to \c term_valuate_inner
 */
static sstring ss_set = NULL ;



/*!
 * Recursive valuating function.
 * \param t term being valuated.
 * \param t_res valuated variable stack (list of active variables).
 */
static term term_valuate_inner ( term t ,
				 variable_definition_list var_list ) { 
  return NULL ;
}



term term_valuate ( term t ) {  
  return NULL ;
}

