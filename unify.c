# include <assert.h>
# include <stdbool.h>
# include <ctype.h>

# undef NDEBUG   // FORCE ASSERT ACTIVATION

# include "term_variable.h"
# include "unify.h"


/*! Symbol key-word for a unification term. */
static char const * const symbol_unify = "unify" ;

/*! Symbol key-word for an equality term. */
static char const * const symbol_equal = "=" ;

/*! Symbol key-word for a solution term. */
static char const * const symbol_solution = "solution" ;

/*! Symbol key-word for a value of a variable in a solution. */
static char const * const symbol_val = "val" ;

/*! Symbol key-word for sting an incompatible term (no solution). */
static char const * const symbol_incompatible = "incompatible" ;



term term_unify ( term t ) {  
  return NULL ;
}

