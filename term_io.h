# ifndef __TERM_IO_H
# define __TERM_IO_H

# include <stdio.h>

# include "term.h"


/*! \file
 * \brief This module provide I/O function for terms.
 *
 * All terms are read and written as these examples with any addition of space (or suppression around parenthesis):
 * \li x
 * \li f ( x )
 * \li T ( f ( x ) f ( x ) )
 * \li -> ( 'e T ( f ( x ) f ( x ) ) )
 *
 * \c assert is enforced to test that all pre-conditions are valid.
 *
 * \author Jérôme DURAND-LOSE
 * \version 1
 * \date 2016
 */

/*!
 * Read a term from a stream.
 * \param in stream to read from.
 * \pre \c in is non NULL.
 * \return read term.
 */
extern term term_scan ( FILE * in ) ;


/*!
 * Print a term on a stream.
 * It is printed is expanded format: line breaking and indentation like in:
 * \verbatim
12 (
  er
  ty (
    er
    A_TROUVER
  )
  ->
)
\endverbatim
 * \param t term to print.
 * \param out stream to print to.
 * \pre \c t and \c out are non NULL.
 */
extern void term_print_expanded ( term t ,
				  FILE * out ) ;


/*!
 * Print a term on a stream.
 * It is printed is compact format: no line breaking nor indentation like in:
 * \verbatim ert ( dq + / 12 ( er ty ( er A_TROUVER ) -> ) ) \endverbatim
 * \param t term to print.
 * \param out stream to print to.
 * \pre \c t and \c out are non NULL.
 */
extern void term_print_compact ( term t ,
				 FILE * out ) ;


# endif
