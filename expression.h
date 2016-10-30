#ifndef __EXPRESSION_H
#define __EXPRESSION_H

#include <stdio.h>

#include "term.h"
#include "term_variable.h"

/*! \file
 * \brief This module is used to evaluate an expression of term.
 *
 * The symbols are restricted to the arithmetic and logic operators:
 * \li +  : addition operator
 * \li -  : subtraction operator
 * \li *  : multiplication operator
 * \li /  : division operator
 * \li && : and operator
 * \li || : or operator
 * \li !  : not operator
 *
 * The variable type is restricted to integers and boolean
 *
 * \c assert is enforced to test that all pre-conditions are valid.
 *
 * \author Nicolas HIOT
 * \version 1
 * \date 2016
 */

/*!
 * Check if the term is a valid expression.
 * \param t term to check.
 * \pre \c t is non NULL.
 * \return true if term is valid.
 */
extern bool term_is_valid_expression(term t);

/*!
 * Return the value of expression
 * \param t expression to valuate.
 * \pre \c t is non NULL.
 * \pre \c t is a valid expression
 * \return value of expression.
 */
extern int expression_valuate(term t);

#endif
