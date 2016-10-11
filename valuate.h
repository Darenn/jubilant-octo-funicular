# ifndef __VALUATE_H
# define __VALUATE_H

# include <stdio.h>


# include "term.h"



/*! \file
 * \brief reduce all \verbatim set ( variable value expression )\endverbatim
 * For example, the term
 * \verbatim set ( 'x 10 + ( 'x * ( 'y 'x )  ) \endverbatim
 * when processed will replace all appearance of variable \c 'x in the term \c +('x*('y \c'x)) ;
 * The output term is:
 * \verbatim + ( 10 * ( 'y 10 ) ) \endverbatim
 *
 * The variable MUST NOT appear in the value.
 *
 * More set symbols may appear in the term and a variable may be redefined.
 */


/*!
 * The initial term is not modified.
 * A new term is generated with all set simplified.
 * If a replacement provide a new variable (or a new \c set), it should be treated.
 *
 * It is not possible to replace all occurrences  of a variable at once since the variable may be redefined on the way.
 *
 * \param t term to simplify
 * \return simplified term
 */
extern term term_valuate ( term t ) ;



# endif
