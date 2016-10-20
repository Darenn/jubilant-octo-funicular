#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "term.h"

#undef NDEBUG // FORCE ASSERT ACTIVATION

/*!
 * This structure is used to have a both way linked list of terms.
 * It is used to record the arguments of terms.
 * It is linked in both way to ensure simple navigation.
 */
typedef struct term_list_struct {
  /*! Current term / argument */
  term t;
  /*! Link to previous term / argument */
  struct term_list_struct *previous;
  /*! Link to next term / argument */
  struct term_list_struct *next;
} * term_list;

/*!
 * This structure is used to record a term.
 * All sub-term / arguments are stored in a both way linked list of terms.
 * Arguments can be accessed from both side.
 */
typedef struct term_struct {
  /*! Symbol of the term */
  sstring symbol;
  /*! Number of arguments, stored for efficiency */
  int arity;
  /*! Father term if set then this term is an argument of the father. */
  term father;
  /*! First argument in the first link of the doubled chain. */
  term_list argument_first;
  /*! Last argument in the first link of the doubled chain. */
  term_list argument_last;
} term_struct;

/*!
 * To test whether a \c sstring is a valid symbol.
 * This means not empty, no space nor parenthesis.
 * \param symbol sstring to test the validity as an symbol.
 * \return true if \c symbol is valid as a term symbol.
 */
static bool symbol_is_valild(sstring const symbol) {
  assert(symbol != NULL);
  if (sstring_is_empty(symbol)) {
    return false;
  }
  for (int i = 0; i < sstring_get_length(symbol); i++) {
    char c = sstring_get_char(symbol, i);
    if (isspace(c) || c == ')' || c == '(') {
      return false;
    }
  };
  return true;
}

static term_list term_list_create(term t) {
  assert(t != NULL);
  term_list tl = malloc(sizeof(struct term_list_struct));
  assert(tl != NULL);
  tl->t = t;
  tl->previous = tl->next = NULL;
  return tl;
}

term term_create(sstring symbol) {
  assert(symbol_is_valild(symbol));
  term t = malloc(sizeof(term_struct));
  assert(t != NULL);
  t->symbol = sstring_copy(symbol);
  t->arity = 0;
  t->father = NULL;
  t->argument_first = NULL;
  t->argument_last = NULL;
  return t;
}

static void term_list_destroy(term_list *tl) {
  assert(tl != NULL);
  if (*tl != NULL) {
    term_destroy(&(*tl)->t);
    free(*tl);
    *tl = NULL;
  }
}

void term_destroy(term *t) {
  assert(t != NULL);
  if (*t != NULL) {
    term_list current = (*t)->argument_first;
    term_list next = NULL;
    while (current != NULL) {
      next = current->next;
      term_list_destroy(&current);
      current = next;
    }
    sstring_destroy(&(*t)->symbol);
    free(*t);
    *t = NULL;
  }
}

sstring term_get_symbol(term t) {
  assert(NULL != t);
  return t->symbol;
}

int term_get_arity(term t) {
  assert(NULL != t);
  return t->arity;
}

term term_get_father(term t) {
  assert(NULL != t);
  return t->father;
}

static inline term_list term_add_argument_empty(term t, term a) {
  assert(t != NULL);
  assert(a != NULL);
  // Create term_list
  a->father = t;
  t->arity = 1;
  term_list arg = term_list_create(a);
  t->argument_last = t->argument_first = arg;
  // Return arg
  return arg;
}

static inline term_list term_list_get(term t, int pos) {
  assert(t != NULL);
  assert(pos >= 0);
  assert(pos < t->arity);
  term_list arg = t->argument_first;
  for (int i = 1; i <= pos; i++) {
    arg = arg->next;
  }
  return arg;
}

void term_add_argument_last(term t, term a) {
  assert(t != NULL);
  assert(a != NULL);
  if (t->arity == 0) {
    term_add_argument_empty(t, a);
  } else {
    a->father = t;
    term_list arg = term_list_create(a);
    arg->previous = t->argument_last;
    t->argument_last->next = arg;
    t->argument_last = arg;
    t->arity++;
  }
}

void term_add_argument_first(term t, term a) {
  assert(t != NULL);
  assert(a != NULL);
  if (t->arity == 0) {
    term_add_argument_empty(t, a);
  } else {
    a->father = t;
    term_list arg = term_list_create(a);
    arg->next = t->argument_first;
    t->argument_first->previous = arg;
    t->argument_first = arg;
    t->arity++;
  }
}

void term_add_argument_position(term t, term a, int pos) {
  assert(t != NULL);
  assert(a != NULL);
  assert(pos >= 0);
  assert(pos <= t->arity);
  if (pos == 0) {
    term_add_argument_first(t, a);
  } else if (pos == t->arity) {
    term_add_argument_last(t, a);
  } else {
    term_list arg = term_list_get(t, pos - 1);
    // Create term_list
    term_list new_arg = term_list_create(a);
    new_arg->next = arg->next;
    new_arg->previous = arg;
    // Add term_list
    new_arg->previous->next = new_arg;
    new_arg->next->previous = new_arg;
    a->father = t;
    t->arity++;
  }
}

bool term_contains_symbol(term t, sstring symbol) {
  assert(t != NULL);
  assert(t->symbol != NULL);
  if (sstring_compare(t->symbol, symbol) == 0) {
    return true;
  } else {
    for (int i = 0; i < t->arity; i++) {
      term arg = term_get_argument(t, i);
      if (term_contains_symbol(arg, symbol) == 0) {
        return true;
      }
    }
  }
  return false;
}

term term_get_argument(term t, int pos) {
  assert(t != NULL);
  assert(pos >= 0);
  assert(pos < t->arity);
  // If no argument return NULL
  if (t->arity == 0) {
    return NULL;
  }
  return term_list_get(t, pos)->t;
}

term term_extract_argument(term t, int pos) {
  assert(t != NULL);
  assert(pos >= 0);
  assert(pos < t->arity);
  // If no argument return NULL
  if (t->arity == 0) {
    return NULL;
  }
  term_list arg = term_list_get(t, pos);
  if (t->arity >= 2) {
    if (pos == 0) {
      arg->next->previous = NULL;
      t->argument_first = arg->next;
    } else if (pos == t->arity - 1) {
      arg->previous->next = NULL;
      t->argument_last = arg->previous;
    } else {
      arg->next->previous = arg->previous;
      arg->previous->next = arg->next;
    }
    t->arity--;
  } else { // If is the last argument
    t->argument_first = NULL;
    t->argument_last = NULL;
    t->arity = 0;
  }
  term tr = term_copy(arg->t);
  term_list_destroy(&arg);
  return tr;
}

term term_copy(term t) {
  assert(t != NULL);
  term new = term_create(t->symbol);
  for (int i = 0; i < t->arity; i++) {
    term_add_argument_last(new, term_copy(term_get_argument(t, i)));
  }
  return new;
}

term term_copy_translate_position(term t, term *loc) {
  assert(t != NULL);
  assert(loc != NULL);
  return NULL;
}

void term_replace_copy(term t_loc, term t_src) {
  assert(t_loc != NULL);
  assert(t_src != NULL);
  term_list current = t_loc->argument_first;
  while (current != NULL) {
    term_list next = current->next;
    term_list_destroy(&current);
    current = next;
  }
  sstring_destroy(&t_loc->symbol);
  t_loc->symbol = sstring_copy(t_src->symbol);
  t_loc->arity = 0;
  t_loc->argument_first = NULL;
  t_loc->argument_last = NULL;
  // Add src args
  term_argument_traversal tat = term_argument_traversal_create(t_src);
  while (term_argument_traversal_has_next(tat)) {
    term_add_argument_last(t_loc,
                           term_copy(term_argument_traversal_get_next(tat)));
  }
  term_argument_traversal_destroy(&tat);
}

int term_compare(term t1, term t2) {
  int compare = sstring_compare(t1->symbol, t2->symbol);
  if (compare == 0) {
    compare = t1->arity - t2->arity;
    if (compare == 0) {
      int i = 0;
      while (i < t1->arity && compare == 0) {
        compare = sstring_compare(term_get_argument(t1, i)->symbol,
                                  term_get_argument(t2, i)->symbol);
        i++;
      }
    }
  }
  return compare;
}

struct term_argument_traversal_struct {
  term_list tls;
};

term_argument_traversal term_argument_traversal_create(term t) {
  assert(t != NULL);
  term_argument_traversal tt =
      malloc(sizeof(struct term_argument_traversal_struct));
  assert(tt != NULL);
  tt->tls = t->argument_first;
  return tt;
}

void term_argument_traversal_destroy(term_argument_traversal *tt) {
  assert(tt != NULL);
  if (*tt != NULL) {
    term_list_destroy(&(*tt)->tls);
    free(*tt);
  }
  *tt = NULL;
}

bool term_argument_traversal_has_next(term_argument_traversal tt) {
  assert(tt != NULL);
  return tt->tls != NULL;
}

term term_argument_traversal_get_next(term_argument_traversal tt) {
  assert(tt != NULL);
  assert(term_argument_traversal_has_next(tt));
  term t = tt->tls->t;
  tt->tls = tt->tls->next;
  return t;
}
