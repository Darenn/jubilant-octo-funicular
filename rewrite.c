#include "term_io.h"
#include <assert.h>

#undef NDEBUG // FORCE ASSERT ACTIVATION

#include "term_variable.h"
#include "unify.h"

/*! Symbol key-word for a rewriting term. */
static char const *const symbol_rewrite = "rewrite";
/*! Symbol key-word for a single rewriting rule. */
static char const *const symbol_rule = "->";
/*! Symbol key-word for the result term. */
static char const *const symbol_results = "results";

/*! Symbol key-word for the affectation of variables. */
static char const *const symbol_affectation = "affectation";
/*! Symbol key-word for the valuation of one variable. */
static char const *const symbol_valuation = "valuation";
static char const *const symbol_error = "ERROR";

static void term_clean_arguments(term t) {
  int arity = term_get_arity(t);
  for (size_t i = 0; i < arity; i++) {
    term arg = term_extract_argument(t, 0);
    term_destroy(&arg);
  }
}

static term term_create_valuation(term variable, term value) {
  sstring valuation_sstring = sstring_create_string(symbol_valuation);
  term valuation = term_create(valuation_sstring);
  sstring_destroy(&valuation_sstring);
  term_add_argument_first(valuation, variable);
  term_add_argument_last(valuation, value);
  return valuation;
}

static void copy_arguments_from_to(term from, term to) {
  term_argument_traversal iterator = term_argument_traversal_create(from);
  while (term_argument_traversal_has_next(iterator)) {
    term_add_argument_last(
        to, term_copy(term_argument_traversal_get_next(iterator)));
  }
}

static bool term_contains_variable(term t) {
  for (size_t i = 0; i < term_get_arity(t); i++) {
    if (term_is_variable(term_get_argument(t, i))) {
      return true;
    }
  }
  return false;
}

static term term_create_affectation() {
  sstring string_affectation = sstring_create_string(symbol_affectation);
  term affectation = term_create(string_affectation);
  sstring_destroy(&string_affectation);
  return affectation;
}

/*!
 * Add an argument to a term in sorted position.
 * Sort is done according to term_compare.
 * If the added argument is already present, then it is not added.
 * \param t term to check.
 * \param arg term to add as argument.
 * \pre t and arg are non NULL.
 */
static void term_add_arg_sort_unique(term t, term arg) {
  int arity = term_get_arity(t);
  for (int i = 0; i < arity; i++) {
    if (term_compare(arg, term_get_argument(t, i)) == 0) {
      term_destroy(&arg);
      return;
    }
    if (term_compare(arg, term_get_argument(t, i)) < 0) {
      term_add_argument_position(t, arg, i);
      return;
    }
  }
  term_add_argument_last(t, arg);
}

term term_copy_replace_at_loc(term t, term r, term *loc) {
  if (*loc == t) {
    return term_copy(r);
  }
  term new = term_create(term_get_symbol(t));
  for (int i = 0; i < term_get_arity(t); i++) {
    term arg = term_get_argument(t, i);
    // term copyarg = term_copy(arg);
    if (*loc == arg) {
      term_add_argument_last(new, r);
    } else {
      term_add_argument_last(new, term_copy_replace_at_loc(arg, r, loc));
    }
  }
  return new;
}

/*!
 * To check that a term correspond to a term pattern.
 * Terms \c t and \c pattern are not modified.
 * \param t term to check
 * \param pattern pattern to find
 * \param affectation term representing the variables set so far.
 * \pre t, pattern and affectation are non NULL.
 * \return true if \c pattern is matched. In such a case affectation is filled
 * accordingly.
 */
static bool term_is_pattern(term t, term pattern, term affectation) {
  // printf("Pattern : ");
  //////term_print_compact(pattern, stdout);
  // puts("\n");
  // printf("Term : ");
  // //////term_print_compact(t, stdout);
  // puts("\n");
  if (term_is_variable(pattern)) {
    if (term_contains_symbol(affectation, term_get_symbol(pattern))) {
      for (size_t i = 0; i < term_get_arity(affectation); i++) {
        term valuation = term_get_argument(affectation, i);
        if (sstring_compare(term_get_symbol(term_get_argument(valuation, 0)),
                            term_get_symbol(pattern)) == 0) {
          if (term_compare(term_get_argument(valuation, 1), t) != 0) {
            // printf("trouve una val diff pour une variable");
            sstring error_symbol = sstring_create_string(symbol_error);
            term error = term_create(error_symbol);
            sstring_destroy(&error_symbol);
            term_add_arg_sort_unique(affectation, error);
            // printf("retourne faux car double affection.");
            return false;
          }
        }
      }
    }
    term newValuation = term_create_valuation(term_copy(pattern), term_copy(t));
    term_add_argument_last(affectation, newValuation);
    return true;
  }
  if (term_get_arity(t) != term_get_arity(pattern)) {
    term_clean_arguments(affectation);
    return false;
  }
  if (term_compare(t, pattern) == 0) {
    // printf("Retourne true car le term est pareil que le pattern");
    return true;
  }
  if (sstring_compare(term_get_symbol(pattern), term_get_symbol(t)) == 0) {
    term_argument_traversal ttraversal = term_argument_traversal_create(t);
    term_argument_traversal patternTraversal =
        term_argument_traversal_create(pattern);
    bool patternFound = false;
    while (term_argument_traversal_has_next(ttraversal)) {
      if (!term_argument_traversal_has_next(patternTraversal)) {
        term_clean_arguments(affectation);
        return false;
      }
      term current_t = term_argument_traversal_get_next(ttraversal);
      term current_pattern = term_argument_traversal_get_next(patternTraversal);
      if (term_is_pattern(current_t, current_pattern, affectation)) {
        patternFound = true;
      } else {
        return false;
      }
    }
    if (patternFound) {
      sstring error_symbol = sstring_create_string(symbol_error);
      if (term_contains_symbol(affectation, error_symbol)) {
        term_clean_arguments(affectation);
        return false;
      } else {
        sstring_destroy(&error_symbol);
        return true;
      }
    }
  }
  if (term_compare(t, pattern) != 0) {
    // printf("Retourne faux car le pattern est différent du terme");
    return false;
  }
  // printf("Retourne true pas défault");
  return true;
}

static term term_create_result() {
  sstring string_result = sstring_create_string(symbol_results);
  term results = term_create(string_result);
  sstring_destroy(&string_result);
  return results;
}

/*!
 * To make operate a single rewriting rule on a term.
 * The products of rewriting are added to the results term as argument.
 * They are added sorted without duplicate.
 * Rewriting process is local, but the whole structure has to output.
 * \param t_whole whole term
 * \param t_current current sub-term being looked for a match
 * \param pattern pattern to match
 * \param replace term to replace matches
 * \param results terms already generated by previous rules and the current
 * rule.
 * \pre none of the term is NULL.
 */
static void term_rewrite_rule(term t_whole, term t_current, term pattern,
                              term replace, term results) {
  term affectation = term_create_affectation();
  // If the term is a pattern, replace the variables in it and add the
  // possibility to results.
  if (term_is_pattern(t_current, pattern, affectation)) {
    // printf("it's a pattern!");
    // term_print_compact(t_current, stdout);
    // puts("\n");
    // printf("of that :");
    // term_print_compact(pattern, stdout);
    // puts("\n");
    // printf("i replace by:");
    // term_print_compact(replace, stdout);
    // puts("\n");
    term r = term_copy(replace);
    term_argument_traversal affectationTraversal =
        term_argument_traversal_create(affectation);
    bool valuesSet = false;
    while (term_argument_traversal_has_next(affectationTraversal)) {
      term valuation = term_argument_traversal_get_next(affectationTraversal);
      term_replace_variable(r, term_get_symbol(term_get_argument(valuation, 0)),
                            term_get_argument(valuation, 1));
      valuesSet = true;
    }
    // If the pattern contains a variable but no affectation was made, it's
    // wrong, we stop here and don't add to results
    if (term_contains_variable(pattern) && !valuesSet) {
      return;
    }
    // add to results the possibility
    term *loc = &t_current;
    term copy = term_copy_replace_at_loc(t_whole, r, loc);
    // printf("copy :");
    // term_print_compact(copy, stdout);
    // puts("\n");
    term_add_arg_sort_unique(results, copy);
  } else {
    // printf("not pattern");
    // Else, the term is not a pattern, so we try to rewrite its arguments with
    // the pattern
    term_argument_traversal t_current_traversal =
        term_argument_traversal_create(t_current);
    while (term_argument_traversal_has_next(t_current_traversal)) {
      term_rewrite_rule(t_whole,
                        term_argument_traversal_get_next(t_current_traversal),
                        pattern, replace, results);
    }
    term_argument_traversal_destroy(&t_current_traversal);
  }
}

/*!
 * Check that rules are well formed.
 * It is supposed that:
 * \li any number first argument has been removed
 * \li last argument is the term to rewrite
 *
 * Should be used for assert only
 */
static bool rules_are_well_formed(term t) { return true; }

term term_rewrite(term t) {
  assert(rules_are_well_formed(t));
  // Here I suppose the rule is well formed
  int factor = 1;
  // Check if there is a factor for the rules then affect it
  term firstArgument = term_get_argument(t, 0);
  if (!term_is_variable(firstArgument) && term_get_arity(firstArgument) == 0) {
    sstring_is_integer(term_get_symbol(firstArgument), &factor);
  }
  term termToRewrite = term_get_argument(t, term_get_arity(t) - 1);

  term results = term_create_result();
  term newResults = term_create_result();
  term_add_argument_last(results, term_copy(termToRewrite));

  for (int i = 0; i < factor; i++) {
    term_argument_traversal rewriteTraversal =
        term_argument_traversal_create(t);
    // factor > 1 means that there is a factor as first argument, so we pass it
    if (factor > 1) {
      term_argument_traversal_get_next(rewriteTraversal);
    }
    // printf("Results :");
    // term_print_compact(results, stdout);
    // I loop through rules
    while (term_argument_traversal_has_next(rewriteTraversal)) {
      //   printf("azdazdazdazda");
      term rule = term_argument_traversal_get_next(rewriteTraversal);
      if (!term_argument_traversal_has_next(rewriteTraversal)) {
        // We are at the end, we stop
        break;
      }
      //   printf("Ihyaaaaa");
      term termToReplace = term_get_argument(rule, 0);
      term replaceWith = term_get_argument(rule, 1);
      term_argument_traversal args = term_argument_traversal_create(results);
      // I Loop trough args of the results
      while (term_argument_traversal_has_next(args)) {
        // printf("Allo?");
        term termToRewrite = term_argument_traversal_get_next(args);
        // For each args of the term to rewrite, i rewrite it
        // The possibilities are set in results
        term_rewrite_rule(termToRewrite, termToRewrite, termToReplace,
                          replaceWith, newResults);
      }
    }
    // printf("New Results :");
    // term_print_compact(newResults, stdout);
    // // term_destroy(results);
    results = term_copy(newResults);
    // // term_destroy(newResults);
    newResults = term_create_result();
  }
  return results;
}
