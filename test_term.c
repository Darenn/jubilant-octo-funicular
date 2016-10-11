# include <stdio.h>
# include <assert.h>

# include "sstring.h"
# include "term.h"
# include "term_io.h"

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


char const * const st_a_trouver = "A_TROUVER" ;

static void test_affiche ( term t ) {
  static int nbr = 0 ;
  printf ( "######### term %d #######\n" , ++ nbr ) ;
  term_print_expanded ( t , stdout ) ;
  printf ( "top SYMBOL: " ) ;
  sstring_print ( term_get_symbol ( t ) , stdout ) ;
  int n = term_get_arity ( t ) ;
  printf ( "\ntop nbr argument: %d\n" , n ) ;
  if ( 0 < n ) {
    printf ( "\ntop argument #%d: \n" , n-1  ) ;
    term_print_expanded ( term_get_argument ( t , n - 1 ) , stdout ) ;
    int i = n / 2 ;
    term_print_compact ( t , stdout ) ;
    printf ( "\nextracting argument #%d: \n" , i  ) ;
    term arg = term_extract_argument ( t , i ) ;
    term_print_compact ( arg , stdout ) ;
    term_destroy ( & arg ) ;
    printf ( "\nremains: \n" ) ;
    term_print_compact ( t , stdout ) ;
    printf ( "\n" ) ;
  }
  if ( 0 <  term_get_arity ( t ) ) {
    printf ( "\nextracting premier argument\n" ) ;
    term arg = term_extract_argument ( t , 0 ) ;
    term_print_compact ( arg , stdout ) ;
    term_destroy ( & arg ) ;
    printf ( "\nremains: \n" ) ;
    term_print_compact ( t , stdout ) ;
    printf ( "\n" ) ;
  }
  while ( 0 < term_get_arity ( t ) ) {
    printf ( "\nextracting dernier argument\n" ) ;
    term arg = term_extract_argument ( t , term_get_arity ( t ) - 1 ) ;
    term_print_compact ( arg , stdout ) ;
    term_destroy ( & arg ) ;
    printf ( "\nremains: \n" ) ;
    term_print_compact ( t , stdout ) ;
    printf ( "\n" ) ;
  }
  sstring symbol = sstring_create_string ( st_a_trouver ) ; // has to be destroyed
  printf ( "symbol \"%s\" IS%s present\n" ,
	   st_a_trouver ,
	   ( term_contains_symbol ( t , symbol ) ? "" : " NOT" ) ) ;

  sstring_destroy ( & symbol ) ;
}

/*! create, print and delete term: t1 */
static void test_example_1 () {
  sstring s = sstring_create_string ( "t1" ) ;
  term t = term_create ( s ) ;
  sstring_destroy ( & s ) ;

  term_print_compact ( t , stdout ) ;
  putchar ( '\n' ) ;

  term_destroy ( & t ) ;
}

/*! create, print and delete term: f ( a bb ccc ) */
static void test_example_2 () {
  sstring s = sstring_create_string ( "f" ) ;
  term t = term_create ( s ) ;
  sstring_destroy ( & s ) ;
  s = sstring_create_string ( "a" ) ;
  term tt = term_create ( s ) ;
  sstring_destroy ( & s ) ;
  term_add_argument_last ( t , tt ) ;
  s = sstring_create_string ( "bb" ) ;
  tt = term_create ( s ) ;
  sstring_destroy ( & s ) ;
  term_add_argument_last ( t , tt ) ;
  s = sstring_create_string ( "ccc" ) ;
  tt = term_create ( s ) ;
  sstring_destroy ( & s ) ;
  term_add_argument_last ( t , tt ) ;
  
  term_print_compact ( t , stdout ) ;
  putchar ( '\n' ) ;
  term_print_expanded ( t , stdout ) ;
  
  term_destroy ( & t ) ;
}

/*! create, print and delete term: @(x !(y) ::(z z)) */
static void test_example_3 () {
  sstring s = sstring_create_string ( "@" ) ;
  term t = term_create ( s ) ;
  sstring_destroy ( & s ) ;
  
  s = sstring_create_string ( "x" ) ;
  term tt = term_create ( s ) ;
  term_add_argument_last ( t , tt ) ;
  sstring_destroy ( & s ) ;

  s = sstring_create_string ( "!" ) ;
  tt = term_create ( s ) ;
  sstring_destroy ( & s ) ;
  s = sstring_create_string ( "y" ) ;
  term_add_argument_last ( tt , term_create ( s ) ) ;
  sstring_destroy ( & s ) ;
  term_add_argument_last ( t , tt ) ;

  s = sstring_create_string ( "::" ) ;
  tt = term_create ( s ) ;
  sstring_destroy ( & s ) ;
  s = sstring_create_string ( "z" ) ;
  term_add_argument_last ( tt , term_create ( s ) ) ;
  term_add_argument_last ( tt , term_create ( s ) ) ;
  term_add_argument_last ( t , tt ) ;
  sstring_destroy ( & s ) ;
  
  term_print_compact ( t , stdout ) ;
  putchar ( '\n' ) ;
  term_print_expanded ( t , stdout ) ;
  
  term_destroy ( & t ) ;
}


static void test_example_4 () {
  sstring s = sstring_create_string ( "e_t::u/" ) ;
  term t = term_create ( s ) ;
  term_add_argument_last ( t , term_create ( s ) ) ;
  term_add_argument_last ( t , term_create ( s ) ) ;
  term_add_argument_last ( t , term_create ( s ) ) ;
  term_add_argument_last ( t , term_create ( s ) ) ;
  sstring_destroy ( & s ) ;
  test_affiche ( t ) ;  
  term_destroy ( & t ) ;
}


static void test_example_5 () {
  FILE * in = fopen ( "DATA/Terms/t0.term" , "r" ) ;
  assert ( NULL != in ) ;
  term t = term_scan ( in ) ;
  fclose ( in ) ;  
  test_affiche ( t ) ;
  term copy = term_copy ( t ) ; 
  printf ( "Comparaison avec la copie %d\n" , term_compare ( t , copy ) ) ;
  term_print_compact ( t , stdout ) ;  
  putchar ( '\n' ) ;
  term_print_compact ( copy , stdout ) ;
  putchar ( '\n' ) ;
  term_add_argument_last ( copy , term_copy ( copy ) ) ;
  term_print_compact ( copy , stdout ) ;
  putchar ( '\n' ) ;
  printf ( "Comparaison avec la copie modifiée %d\n" , term_compare ( t , copy ) ) ;
  printf ( "Comparaison avec la copie modifiée (dans l'autre sens) %d\n" , term_compare ( copy , t ) ) ;
  term_destroy ( & t ) ;
  term_destroy ( & copy ) ;
}


static void test_ajout_i () {
  sstring s = sstring_create_string ( "/" ) ;
  term t = term_create ( s ) ;
  term_add_argument_last ( t , term_create ( s ) ) ;
  term_add_argument_last ( t , term_create ( s ) ) ;
  sstring_destroy ( & s ) ;
  s = sstring_create_string ( "O" ) ;
  term_add_argument_position ( t , term_create ( s ) , 0 ) ;
  sstring_destroy ( & s ) ;
  s = sstring_create_string ( "2" ) ;
  term_add_argument_position ( t , term_create ( s ) , 2 ) ;
  sstring_destroy ( & s ) ;
  s = sstring_create_string ( "4" ) ;
  term_add_argument_position ( t , term_create ( s ) , 4 ) ;
  sstring_destroy ( & s ) ;
  term_print_compact ( t , stdout ) ;  
  putchar ( '\n' ) ;
  term_destroy ( & t ) ;
}

static void test_traversal () {
  FILE * in = fopen ( "DATA/Terms/t1.term" , "r" ) ;
  assert ( NULL != in ) ;
  term t = term_scan ( in ) ;
  fclose ( in ) ;  
  term_print_compact ( t , stdout ) ;
  putchar ( '\n' ) ;
  term_argument_traversal tat = term_argument_traversal_create ( t ) ;
  while ( term_argument_traversal_has_next ( tat ) ) {
    term t_loc = term_argument_traversal_get_next ( tat ) ;
    term_print_compact ( t_loc , stdout ) ;  
    putchar ( '\n' ) ;
  }
  term_argument_traversal_destroy ( & tat ) ; 
  term_destroy ( & t ) ;
}


int main ( void ) {
  test_example_1 () ;
  test_example_2 () ;
  test_example_3 () ;
  test_example_4 () ;
  test_example_5 () ;
  test_ajout_i () ;
  test_traversal () ;
  return 0 ;
}
