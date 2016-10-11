# ifndef __REWRITE_H
# define __REWRITE_H

# include <stdio.h>


# include "term.h"
# include "term_variable.h"



/*! \file
 * \brief Rewrite a \c term according to rewriting rules
 */

/*!
 * The term rewriting system is given in the form
 * \verbatim rewrite ( -> ( <t1> <t1'> ) … -> ( <tn> <tn'> ) term ) \endverbatim
 * or
 * \verbatim rewrite ( n -> ( <t1> <t1'> ) … -> ( <tn> <tn'> ) term ) \endverbatim
 * where n is a decimal number. It is the number of times that the rules should be applied.
 * If n is 5, all results corresponds to 5 rewritings
 *
 * A rewriting step correspond to finding one copy of the left argument (pattern) as a sub-term and replacing it by the left term (replacement).
 *
 * Rewriting rules may contains variables. 
 * In such a case the should be defined in the let part.
 * Any variable appearing more than once on the left must always match the same term (otherwise there is a mismatched).
 * When replacing by the right member, variables must be replaces by their values.
 *
 * The initial term is not modified.
 *
 * The returned term is of the form:
 * \verbatim result ( term_1 term_2 … term_k )\endverbatim
 * where the k terms are:
 * \li each produced by a single application of one of the given rule,
 * \li sorted according to term_compare,
 * \li are not repeated.
 *
 * For example:
 * \verbatim rewrite ( -> ( t e ( a 10 ) ) u ( t t t ) ) \endverbatim
 * transformed into (there is 3 possible places to rewrite \c t
 * \verbatim results ( u ( e ( a 10 ) t t ) u ( t e ( a 10 ) t ) u ( t t e ( a 10 ) ) ) \endverbatim
 *
 * If no rule is applicable, then the \c result returned is empty.
 * \param t encode the terms to rewrite
 * \pre t should be of the correct form
 * \return term whose arguments are the result of the rewriting (if any).
 */
extern term term_rewrite ( term t ) ;



# endif
