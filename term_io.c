#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "term_io.h"

/*!
 * To move pass spaces in a stream.
 * \param in input stream.
 */
static inline void skip_space(FILE *in) {
  char c;
  while ((EOF != (c = getc(in))) && (isspace(c))) {
  };
  ungetc(c, in);
}

/*!
 * Initial size for scaning a symbol.
 */
#define SYMBOL_STRING_LENGHT_BASE 10

/*
 * Scan a sequence of non space and non parenthesis (symbol) of unbounded
 * length.
 * if next non space char is '(' then it scans arguments.
 */
term term_scan(FILE *in) {
  assert(in != NULL);
  // Create char* to store symbol
  char symbol[SYMBOL_STRING_LENGHT_BASE];
  // First term
  assert(fscanf(in, "%s", symbol) != EOF);
  term t = term_create(sstring_create_string(symbol));
  fscanf(in, "%s", symbol); // Get first parenthese
  // Arguments
  while (fscanf(in, "%s", symbol) != EOF) {
    if (symbol[0] == ')') {
      t = term_get_father(t);
    } else if (symbol[0] == '(') {
      t = term_get_argument(t, term_get_arity(t) - 1);
    } else {
      term_add_argument_last(t, term_create(sstring_create_string(symbol)));
    }
  }
  return t;
}

/*!
 * To add spaces to a stream.
 * \param n half the number of spaces to add.
 * \param out output stream to print to.
 */
static inline void add_space_prefix(int n, FILE *out) {
  while (0 < n--) {
    fputs("  ", out);
  }
}

/*!
 * Recursive function called by \c  term_print_expanded .
 * \param t (sub-)term to print
 * \param out output stream to print to.
 * \param depth nesting inside the main term. It is used to handle indentation.
 */
static void term_print_expanded_rec(term const t, FILE *const out,
                                    int const depth) {
  assert(t != NULL);
  add_space_prefix(depth, out);
  sstring_print(term_get_symbol(t), out);
  if (term_get_arity(t)) {
    fprintf(out, " (\n");
    for (int i = 0; i < term_get_arity(t); i++) {
      term_print_expanded_rec(term_get_argument(t, i), out, depth + 1);
    }
    add_space_prefix(depth, out);
    fprintf(out, ")");
  }
  fprintf(out, "\n");
}

void term_print_expanded(term t, FILE *out) {
  assert(NULL != t);
  assert(NULL != out);
  term_print_expanded_rec(t, out, 0);
}

/*!
 * Recursive function called by \c term_print_compact .
 * \param t (sub-)term to print
 * \param out output stream to print to.
 */
static void term_print_compact_rec(term const t, FILE *const out) {
  assert(t != NULL);
  sstring_print(term_get_symbol(t), out);
  if (term_get_arity(t)) {
    fprintf(out, " ( ");
    for (int i = 0; i < term_get_arity(t); i++) {
      term arg = term_get_argument(t, i);
      if (arg != NULL) {
        term_print_compact_rec(arg, out);
        fprintf(out, " ");
      }
    }
    fprintf(out, ")");
  }
}

void term_print_compact(term t, FILE *out) {
  assert(NULL != t);
  assert(NULL != out);
  term_print_compact_rec(t, out);
}
