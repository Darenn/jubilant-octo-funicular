#ifndef __PEANO_H
#define __PEANO_H
#include "expression.h"
#include "term.h"
#include "term_variable.h"

extern bool term_is_number(term t, int *n_pt);
extern term peano_valuate(term t);

#endif
