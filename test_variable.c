# include <stdio.h>
# include <assert.h>

# include "sstring.h"
# include "term.h"
# include "term_io.h"
# include "term_variable.h"

# undef NDEBUG   // FORCE ASSERT ACTIVATION


/*!
 * \file
 * \brief Basic tests and examples of use of \c term.
 *
 * This should also be used to test for memory leak.
 *
 * \author Jérôme DURAND-LOSE
 * \version 1.0
 * \date 2016
 */


/*! create, print and delete term: t1 */
static void symbol_is_variable ( char const * const st ) {
  sstring s = sstring_create_string ( st ) ;
  term t = term_create ( s ) ;
  sstring_destroy ( & s ) ;

  printf ( "\"%s\" %s un symbole de variable\n" ,
	   st ,
	   ( term_is_variable ( t )
	     ? "est"
	     : "n'est pas" ) ) ;

  term_destroy ( & t ) ;
}



void do_replacement ( char const * const file_name ) {

  FILE * in = fopen ( file_name , "r" ) ; 
  assert ( NULL != in ) ;
  term t = term_scan ( in ) ; 
  fclose ( in ) ;  

  term_print_compact ( t , stdout ) ;
  putchar ( '\n' ) ;
  
  term tt = term_get_argument ( t , 0 ) ;
  term_replace_variable ( tt ,
			  term_get_symbol ( term_get_argument ( t , 1 ) ) ,
			  term_get_argument ( t , 2 ) ) ;
  
  term_print_compact ( tt , stdout ) ;
  putchar ( '\n' ) ;
  
  term_destroy ( & t ) ;
}

int main ( void ) {
  symbol_is_variable ( "'a" );
  symbol_is_variable ( "azer" );
  symbol_is_variable ( "'" );
  symbol_is_variable ( "'a_1" );
  symbol_is_variable ( "'1" );
  symbol_is_variable ( "'_1" );
  symbol_is_variable ( "'az°e" );

  do_replacement ( "DATA/Terms/t0var.term" ) ; 
  do_replacement ( "DATA/Terms/t1var.term" ) ; 

  return 0 ;
}
