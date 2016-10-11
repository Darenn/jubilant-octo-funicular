# ifndef __TERM_VARIABLE_H
# define __TERM_VARIABLE_H

# include <stdio.h>

# include "term.h"


/*! \file
 * \brief This module is used to deal with specials terms : variable
 *
 * A term is a variable if:
 * \li its symbol starts with '
 * \li the next symbol belongs to 'a'-'z' 'A'-'Z'  '_'
 * \li all remaining char in the symbol belongs to  'a'-'z' 'A'-'Z' '0'-'9' '_'
 *
 * Variables should have no argument.
 *
 * \c assert is enforced to test that all pre-conditions are valid.
 *
 * \author Jérôme DURAND-LOSE
 * \version 1
 * \date 2016
 */

/*!
 * To test whether a term is a variable.
 * A variable has an symbol of the following form:
 * \li first char is  '\''
 * \li the next symbol belongs to 'a'-'z' 'A'-'Z'  '_'
 * \li every other char in 'a'-'z' 'A'-'Z' '0'-'9' '_'
 * A variable should not have any argument.
 *
 * No side effect, can be used in assert.
 * \post it is asserted that if it is a variable, then the arity is 0
 * \return true if the term is a variable.
 */
extern bool term_is_variable ( term t ) ;



/*!
 * Replace all occurence of a variable by its value.
 * Each time value is copied.
 * The initial terms is modified according to the replacement.
 * \pre variable is a legal name of variable
 * \pre t and value are correct
 */
extern void term_replace_variable ( term t ,
				    sstring variable ,
				    term value ) ;



# endif
