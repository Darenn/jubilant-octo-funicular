#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "term_io.h"

/*!
 * To move pass spaces in a stream.
 * \param in input stream.
 */
static inline void skip_space(FILE *in) {
  char c = getc(in);
  while ((EOF != c) && (isspace(c))) {
    c = getc(in);
  };
  ungetc(c, in);
}

/*!
 * Initial size for scaning a symbol.
 */
#define SYMBOL_STRING_LENGHT_BASE 30

/*!
 * Get the next symbol in stream.
 * \param in FILE the stream.
 * \return sstring the next symbol.
 */
static sstring get_next_symbol(FILE *in) {
  assert(in != NULL);
  // Init
  char c;
  char symbol[SYMBOL_STRING_LENGHT_BASE];
  int i = 0;
  // Get symbol
  skip_space(in);
  while ((i < SYMBOL_STRING_LENGHT_BASE) && (EOF != (c = getc(in))) &&
         !isspace(c) && (c != '(') && (c != ')')) {
    symbol[i++] = c;
  }
  symbol[i] = 0;
  ungetc(c, in);
  return sstring_create_string(symbol);
}

/*!
 * Get the next separator in stream.
 * \param in FILE the stream.
 * \return sstring the next separator.
 */
static char get_next_separator(FILE *in) {
  assert(in != NULL);
  skip_space(in);
  char c = getc(in);
  if ((c == EOF) || (c == '(') || (c == ')')) {
    return c;
  } else {
    ungetc(c, in);
    return ' ';
  }
}

/*
 * Scan a sequence of non space and non parenthesis (symbol) of unbounded
 * length.
 * if next non space char is '(' then it scans arguments.
 */
term term_scan(FILE *in) {
  assert(in != NULL);
  // First term
  sstring s = get_next_symbol(in);
  term start = term_create(s);
  sstring_destroy(&s);

  // Get first parenthese
  char sep = get_next_separator(in);
  if (EOF != sep) {
    // Get arguments
    term t = start;
    while ((sep = get_next_separator(in)) != EOF && t != NULL) {
      if (sep == ')') {
        t = term_get_father(t);
      } else if (sep == '(') {
        t = term_get_argument(t, term_get_arity(t) - 1);
      } else {
        sstring s = get_next_symbol(in);
        term arg = term_create(s);
        sstring_destroy(&s);
        term_add_argument_last(t, arg);
      }
    }
  }
  return start;
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
      term_print_compact_rec(term_get_argument(t, i), out);
      fprintf(out, " ");
    }
    fprintf(out, ")");
  }
}

void term_print_compact(term t, FILE *out) {
  assert(NULL != t);
  assert(NULL != out);
  term_print_compact_rec(t, out);
}
