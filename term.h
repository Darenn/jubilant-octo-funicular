# ifndef __TERM_H
# define __TERM_H

// # include <stdio.h>

# include "sstring.h"


/*! \file
 * \brief This module is used to encode terms.
 *
 * Each term is composed of:
 * \li an symbol. It is a non-empty sequence of letters (stored as a \c sstring) that does not contain any non space nor '(' nor ')'.
 * \li a (possibly empty) sequence of arguments. each argument is a term
 *
 * These are examples of valid terms:
 * \li x
 * \li f ( x )
 * \li T ( f ( x ) f ( x ) )
 * \li -> ( 'e T ( f ( x ) f ( x ) ) )
 *
 * Please note:
 * \li symbols can be non alphanumeric strings, and
 * \li arguments are only separated by spaces (i.e. no comma nor semi-column nor…).
 *
 * Arguments sub-arguments… are also called sub-terms.
 *
 * Note that there is non empty term.
 * NULL value for a term is not accepted by any argument of the function of the module, but it can be returned and means NO TERM.
 *
 * \c assert is enforced to test that all pre-conditions are valid.
 *
 * \author Jérôme DURAND-LOSE
 * \version 1
 * \date 2016
 */


/*!
 * Term are accessed through pointers / reference.
 * The exact structure type is hidden in the .c .
 */
typedef struct term_struct * term ;


/*!
 * Return a term composed of one symbol and no argument.
 * \param s symbol for the created term.
 * \pre s is non-empty and does not contains space nor parenthesis.
 * \return a newly created term with a copy of s as a symbol and no argument.
 */
extern term term_create ( sstring symbol ) ;


/*!
 * Destroy a term (including all arguments recursively)
 * \param t term to destroy.
 * \pre \c t is non NULL
 */
extern void term_destroy ( term * t ) ;


/*!
 * Return the symbol of a term (and not a copy).
 * No side effect, can be used in assert.
 * \param t term to query.
 * \pre t is non NULL.
 * \return the symbol of the term.
 */
extern sstring term_get_symbol ( term t ) ;


/*!
 * Return the symbol of a term.
 * No side effect, can be used in assert.
 * \param t queried term.
 * \pre t is non NULL
 * \return the number of arguments.
 */
extern int term_get_arity ( term t ) ;


/*!
 * Return the father of a term (term to which it as an argument if any).
 * No side effect, can be used in assert.
 * \param t queried term.
 * \return the term that directly hold this term (NULL if none)
 */
extern term term_get_father ( term t ) ;


/*!
 * Add a term as last argument without making any copy of it.
 * \param t term to an argument to
 * \param a argument to add
 * \pre \c t and \c a are non NULL
 */
extern void term_add_argument_last ( term t ,
				     term a ) ;


/*!
 * Add a term as fist argument without making any copy of it.
 * \param t term to an argument to
 * \param a argument to add
 * \pre \c t and \c a are non NULL
 */
extern void term_add_argument_first ( term t ,
				      term a ) ;


/*!
 * Add a term as an argument without making any copy of it.
 * The position is understood as following:
 * \li O for first
 * \li arity for last
 * \li after the \c pos th argument
 * \param t term to an argument to
 * \param a argument to add
 * \param pos position where to add
 * \pre \c t and \c a are non NULL
 * \pre 0 ≤ \c pos ≤ arity
 */
extern void term_add_argument_position ( term t ,
					 term a ,
					 int pos ) ;


/*!
 * To check whether an symbol appear in a term (in any sub_term).
 * No side effect, can be used in assert.
 * \param t queried term.
 * \param symbol queried symbol.
 * \pre \c t and \c symbol are non NULL
 * \return true if the symbol is present in the term
 */
extern bool term_contains_symbol ( term t ,
				   sstring symbol ) ;


/*!
 * Return an argument (without making any copy).
 * Arguments are numbered from 0.
 * \param t queried term.
 * \param pos number of queried argument.
 * \pre \c t is non NULL.
 * \pre 0 ≤ \c pos ≤ arity .
 * \return the arguments number i.
 */
extern term term_get_argument ( term t ,
				int pos ) ;


/*!
 * Return the ith argument of the term.
 * This argument is removed from the term.
 * Arguments are numbered from 0.
 * \pre \c t is non NULL.
 * \pre 0 ≤ \c pos ≤ arity .
 * \return the arguments number i.
 */
extern term term_extract_argument ( term t ,
				    int pos ) ;


/*!
 * Deep copy of term (eveything is copied).
 * \param t term to be copied.
 * \pre \c t is non NULL
 * \return independent copy of \c t
 */
extern term term_copy ( term t ) ;


/*!
 * Deep copy of term (eveything is copied).
 * \param t term to be copied.
 * \param loc if (*loc) term is found, then the value is changed to the corresponding term in the copy.
 * \pre \c t and \c loc are non NULL
 * \return independent copy of t
 */
extern term term_copy_translate_position ( term t ,
					   term * loc ) ;


/*!
 * Deep copy of term (eveything is copied).
 * The copy replace the designated term.
 * The designated term is destroyed.
 * \param t_loc term to be replaced.
 * \param t_src term to be copied.
 * \pre \c t_destination and \c t_source are non NULL
 */
extern void term_replace_copy ( term t_destination ,
				term t_source ) ;


/*!
 * Indicate how two \c term are ordered:
 * Compare symbol
 * if equal then lower arity is lower term
 * then compare first arguments
 * if equal move not next argument
 *
 * \param t1,t2 term to be compared.
 * \pre t1 and t2 are non NULL
 * \return 
 * \li 0 if ss1 == ss2
 * \li -1 if ss1 < ss2
 * \li 1 otherwise
 */
extern int term_compare ( term t1 ,
			  term t2 ) ;

 
/*!
 * This is used to visit all the argument of a term.
 */
typedef struct term_argument_traversal_struct * term_argument_traversal ;


/*!
 * \param t term to run through the arguments
 * \pre \c t is not NULL.
 */
extern term_argument_traversal term_argument_traversal_create ( term t ) ;


/*!
 * To delete an argument of the arguments of a term.
 * \param tt  traversal of the arguments of a term.
 * \pre tt is not NULL.
 */
extern void term_argument_traversal_destroy ( term_argument_traversal * tt ) ;


/*!
 * To test whether a traversal is finished or not.
 * \param tt traversal of the arguments of a term.
 * \pre tt is not NULL.
 * \return true if there are still argument to visit
 */
extern bool term_argument_traversal_has_next ( term_argument_traversal tt ) ;


/*!
 * To get an argument and move onto the next.
 * \param tt  traversal of the arguments of a term.
 * \pre tt is not NULL and not finished.
 * \return next argument.
 */
extern term term_argument_traversal_get_next ( term_argument_traversal tt ) ;


# endif
