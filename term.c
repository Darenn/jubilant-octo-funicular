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
  return tl;
}

term term_create(sstring symbol) {
  term t = NULL;
  if (symbol_is_valild(symbol)) {
    t = malloc(sizeof(term_struct));
    assert(t != NULL);
    t->symbol = sstring_copy(symbol);
    t->arity = 0;
    t->father = NULL;
    t->argument_first = NULL;
    t->argument_last = NULL;
  }
  return t;
}

static void term_list_destroy(term_list *tl) {
  assert(tl != NULL);
  if (*tl != NULL) {
    term_list *next;
    while (*tl != NULL) {
      next = &(*tl)->next;
      term_destroy(&(*tl)->t);
      if ((*tl)->previous != NULL) {
        (*tl)->previous->next = NULL;
      }
      if ((*tl)->next != NULL) {
        (*tl)->next->previous = NULL;
      }
      free(tl);
      tl = next;
    }
    *tl = NULL;
  }
}

void term_destroy(term *t) {
  assert(t != NULL);
  if (*t != NULL) {
    if ((*t)->arity > 0) {
      term_list_destroy(&(*t)->argument_first);
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

static inline term_list term_add_argument(term t, term a) {
  assert(t != NULL);
  assert(a != NULL);
  // Create term_list
  a->father = t;
  t->arity += 1;
  term_list arg = term_list_create(a);
  if (t->argument_last != NULL && t->argument_first != NULL) {
    arg->next = t->argument_first;
    arg->previous = t->argument_last;
    // Add term_list
    t->argument_last->next = t->argument_first->previous = arg;
  } else {
    arg->next = arg->previous = arg;
    t->argument_last = t->argument_first = arg;
  }
  // Return arg
  return arg;
}

void term_add_argument_last(term t, term a) {
  assert(t != NULL);
  assert(a != NULL);
  term_list arg = term_add_argument(t, a);
  t->argument_last = arg;
}

void term_add_argument_first(term t, term a) {
  assert(t != NULL);
  assert(a != NULL);
  term_list arg = term_add_argument(t, a);
  t->argument_first = arg;
}

void term_add_argument_position(term t, term a, int pos) {
  assert(t != NULL);
  assert(a != NULL);
  assert(pos >= 0);
  assert(pos <= t->arity);
  a->father = t;
  t->arity += 1;
  if (pos == 0) {
    term_add_argument_first(t, a);
  } else if (pos == t->arity) {
    term_add_argument_last(t, a);
  } else {
    term_list current = t->argument_first;
    for (int i = 1; i <= pos; i++) {
      current = current->next;
    }
    // Create term_list
    term_list arg = term_list_create(a);
    arg->next = current->next;
    arg->previous = current;
    // Add term_list
    current->next = arg;
    current->next->previous = arg;
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

static inline term_list term_list_get(term t, int pos) {
  assert(t != NULL);
  assert(pos >= 0);
  assert(pos < t->arity);
  term_list current = t->argument_first;
  for (int i = 1; i < pos; i++) {
    current = current->next;
  }
  return current;
}

term term_get_argument(term t, int pos) { return term_list_get(t, pos)->t; }

term term_extract_argument(term t, int pos) {
  // Get term_list
  term_list arg = term_list_get(t, pos);
  arg->previous->next = arg->next;
  arg->next->previous = arg->previous;
  // Get term
  term tr = term_copy(arg->t);
  // Delete term_list
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
  tt->tls = t->argument_first;
  return tt;
}

void term_argument_traversal_destroy(term_argument_traversal *tt) {
  assert(tt != NULL);
  if (*tt != NULL) {
    term_list tl = (*tt)->tls;
    tl->previous->next = tl->next;
    tl->next->previous = tl->previous;
    (*tt)->tls = tl->next;
    term_list_destroy(&tl);
  }
}

bool term_argument_traversal_has_next(term_argument_traversal tt) {
  assert(tt != NULL);
  return tt->tls != tt->tls->next;
}

term term_argument_traversal_get_next(term_argument_traversal tt) {
  assert(tt != NULL);
  assert(term_argument_traversal_has_next(tt));
  term t = tt->tls->t;
  tt->tls = tt->tls->next;
  return t;
}
