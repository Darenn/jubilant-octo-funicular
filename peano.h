#ifndef __PEANO_H
#define __PEANO_H
#include "expression.h"
#include "term.h"
#include "term_variable.h"

/*! \file
 * \brief This module is used to evaluate and transform in peano arithmetics a
 * term.
 *
 * The variable type is restricted to integers
 *
 * \c assert is enforced to test that all pre-conditions are valid.
 *
 * \author Virgil PIA
 * \version 1
 * \date 2016
 */

/* Check if term is a number
 * \return true if symbol is valid
 */
extern bool term_is_number(term t, int *n_pt);
/*!
 * The initial term is not modified.
 * A new term is generated with peano arithmetic term.
 * \param t term to modified
 * \pre t is not null
 * \pre all term in t must be + or * and a number
 * \return peano term
 */
extern term peano_valuate(term t);

#endif
