# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include <assert.h>

# include "sstring.h"

# undef NDEBUG    // FORCE ASSERT ACTIVATION


/*! Used to test the validity in asserts */
# define ASSERT_SSTRING_OK( ss )					\
  assert ( NULL != ss ) ;						\
  assert ( ( 0 == ss -> length ) == ( NULL == ss -> chars ) ) 

/*! 
 * Structure to store a sstring.
 * There is no room for the final \c '\0' of C-string.
 * \param length  length of the string, the number of char exactly
 * \param chars pointer to the the sequence of char
 */
typedef struct sstring_struct {
  unsigned int length ;
  char * chars ;
} sstring_struct ;


bool sstring_is_empty ( sstring ss ) {
  ASSERT_SSTRING_OK ( ss ) ;
  return 0 == ss -> length ;
}

sstring sstring_create_empty ( void ) {
  sstring res = malloc ( sizeof ( sstring_struct ) ) ;
  assert ( NULL != res ) ;
  res -> length = 0 ;
  res -> chars = NULL ;
  ASSERT_SSTRING_OK ( res ) ;
  return res ;
}

sstring sstring_create_string ( char const * st ) { 
  return NULL ;
}


void sstring_destroy ( sstring * ss ) {
  assert ( NULL != ss ) ;
  ASSERT_SSTRING_OK ( ( * ss ) ) ;
  // NB free ( NULL ) is legal
  free ( ( * ss ) -> chars ) ;
  free ( * ss ) ;
  * ss = NULL ;
}


void sstring_print ( sstring ss ,
		     FILE * f ) {
  ASSERT_SSTRING_OK ( ss ) ;
  assert ( NULL != f ) ;
  if ( ! sstring_is_empty ( ss ) ) {
    for ( unsigned int i = 0 ; i < ss -> length ; i ++ ) {
      fputc ( ss -> chars [i] , f ) ;
    }
  }
}


void sstring_concatenate ( sstring ss1,
			   sstring ss2 ) { 
}


sstring sstring_copy ( sstring ss ) 
{
  ASSERT_SSTRING_OK ( ss ) ;
  if ( sstring_is_empty ( ss ) ) {
    return sstring_create_empty () ;
  } else {
    sstring res = malloc ( sizeof ( sstring_struct ) ) ;
    assert ( NULL != res ) ;
    res -> length = ss -> length ;
    res -> chars = malloc ( res -> length * sizeof ( char ) ) ;
    assert ( NULL != res -> chars ) ;
    for ( unsigned int i = 0 ; i < res -> length ; i ++ ) {
      res -> chars [ i ] = ss -> chars [ i ] ;
    }
    ASSERT_SSTRING_OK ( res ) ;
    return res ;
  }
}



int sstring_compare ( sstring ss1 ,
		      sstring ss2 ) {  
  return 0 ;
}



int sstring_get_length ( sstring ss ) {
  ASSERT_SSTRING_OK ( ss ) ;
  return ss -> length ;
}


int sstring_get_char ( sstring ss ,
		       int i ) { 
  return 0 ;
}


bool sstring_is_integer ( sstring ss ,
			  int * n_pt ) { 
  return false ;
}
