# ifndef __UNIFY_H
# define __UNIFY_H

# include <stdio.h>


# include "term.h"


/*! \file
 * \brief Perform the unification on a system of terms.
 *
 * The idea is to find values for variables so that pairs of term are equal.
 *
 * For example, the following system has only one variable \c 'a and one equality (\c z('a)=z(f(r)) ):
 * \verbatim
unify (
  = ( z ( 'a ) z ( f(r) ) )
) \endverbatim
 * The solution is \c 'a=f(r). It is returned as
 * \verbatim solution ( val ( 'a f ( r ) ) ) \endverbatim
 * The algorithm deals a sequence of equalities.
 * It removes the first equality and works as follows:
 * \li if either side is a variable (say 'a=t):
 * - just discard it if t is also 'a
 * - failure if 'a also appear in t
 * - store 'a=t in solution and replace 'a by t everywhere (including in solutions)
 * \li if symbols on both side are equal and terms have same arity
 * add at the end of the the sequence of equalities that first terms have to be equal, second term too and…
 * \li failure otherwise
 */


/*!
 * The term system is given in the form
 * \verbatim unify ( = ( <t1> <t1'> ) … = ( <tn> <tn'> ) ) \endverbatim
 * The initial term is not modified.
 * A new term is generated with the form
 * \verbatim solution ( val ( <variable> <value> ) … val ( <variable> <value> ) ) \endverbatim
 * If no solution is possible, then the output is of the form 
 * \verbatim incompatible ( <term> <term> ) \endverbatim
 * \param t encode the terms to unify.
 * \pre t is not NULL and should be of the correct form.
 * \return solution term or impossible term.
 */
extern term term_unify ( term t ) ;



# endif
