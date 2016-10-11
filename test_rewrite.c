# include <stdio.h>

# include "term.h"
# include "term_io.h"
# include "rewrite.h"

# undef NDEBUG   // FORCE ASSERT ACTIVATION


/*!
 * \file
 * \brief Run rewrite on input term.
 *
 * This should also be used to test for memory leak.
 *
 * \author Jérôme DURAND-LOSE
 * \version 1.0
 * \date 2016
 */


int main ( void ) {
  term t = term_scan ( stdin ) ;
  term_print_expanded ( t , stdout ) ;
  term_print_compact ( t , stdout ) ;
  putchar ( '\n' ) ;
  term t_e = term_rewrite ( t ) ;
  term_destroy ( & t ) ;
  term_print_compact ( t_e , stdout ) ;
  putchar ( '\n' ) ;
  term_destroy ( & t_e ) ;
  return 0 ;
}
