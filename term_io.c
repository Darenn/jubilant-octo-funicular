# include <stdlib.h>
# include <ctype.h>
# include <assert.h>

 
# include "term_io.h"


/*!
 * To move pass spaces in a stream.
 * \param in input stream.
 */
static inline void skip_space ( FILE * in ) {
  char c ;
  while ( ( EOF != ( c = getc ( in ) ) )
	  &&  ( isspace ( c ) ) ) ;
  ungetc ( c , in ) ;
}


/*!
 * Initial size for scaning a symbol.
 */
# define SYMBOL_STRING_LENGHT_BASE 10

/*
 * Scan a sequence of non space and non parenthesis (symbol) of unbounded length.
 * if next non space char is '(' then it scans arguments.
 */
term term_scan ( FILE * in ) { 
  return NULL ;
}


/*!
 * To add spaces to a stream.
 * \param n half the number of spaces to add.
 * \param out output stream to print to.
 */
static inline void add_space_prefix ( int n ,
				      FILE * out ) {
  while ( 0 < n -- ) {
    fputs ( "  " , out ) ;
  }
}



/*!
 * Recursive function called by \c  term_print_expanded .
 * \param t (sub-)term to print
 * \param out output stream to print to.
 * \param depth nesting inside the main term. It is used to handle indentation.
 */
static void term_print_expanded_rec ( term const t ,
				      FILE * const out ,
				      int const depth ) { 
}


void term_print_expanded ( term t ,
			   FILE * out ) { 
  assert ( NULL != t ) ;
  assert ( NULL != out ) ;
  term_print_expanded_rec ( t , out , 0 ) ;
}


/*!
 * Recursive function called by \c term_print_compact .
 * \param t (sub-)term to print
 * \param out output stream to print to.
 */
static void term_print_compact_rec ( term const t ,
				     FILE * const out ) { 
}


void term_print_compact ( term t ,
			  FILE * out ) {
  assert ( NULL != t ) ;
  assert ( NULL != out ) ;
  term_print_compact_rec ( t , out ) ;
}

