#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "sstring.h"
#undef NDEBUG // FORCE ASSERT ACTIVATION

/*! Used to test the validity in asserts */
#define ASSERT_SSTRING_OK(ss)                                                  \
  assert(NULL != ss);                                                          \
  assert((0 == ss->length) == (NULL == ss->chars))

/*!
 * Structure to store a sstring.
 * There is no room for the final \c '\0' of C-string.
 * \param length  length of the string, the number of char exactly
 * \param chars pointer to the the sequence of char
 */
typedef struct sstring_struct {
  unsigned int length;
  char *chars;
} sstring_struct;

bool sstring_is_empty(sstring ss) {
  ASSERT_SSTRING_OK(ss);
  return 0 == ss->length;
}

sstring sstring_create_empty(void) {
  sstring res = malloc(sizeof(sstring_struct));
  assert(NULL != res);
  res->length = 0;
  res->chars = NULL;
  ASSERT_SSTRING_OK(res);
  return res;
}

sstring sstring_create_string(char const *st) {
  if (strlen(st) < 1)
    return sstring_create_empty();
  else {
    sstring ch = (sstring)malloc(sizeof(struct sstring_struct));
    assert(ch != NULL);
    ch->length = strlen(st);
    ch->chars = (char *)malloc(sizeof(char) * strlen(st));
    assert(ch->chars != NULL);
    for (int i = 0; (unsigned)i < ch->length; i++) {
      ch->chars[i] = st[i];
    }
    return ch;
  }
}

void sstring_destroy(sstring *ss) {
  assert(NULL != ss);
  ASSERT_SSTRING_OK((*ss));
  // NB free ( NULL ) is legal
  free((*ss)->chars);
  free(*ss);
  *ss = NULL;
}

void sstring_print(sstring ss, FILE *f) {
  ASSERT_SSTRING_OK(ss);
  assert(NULL != f);
  if (!sstring_is_empty(ss)) {
    for (unsigned int i = 0; i < ss->length; i++) {
      fputc(ss->chars[i], f);
    }
  }
}

void sstring_concatenate(sstring ss1, sstring ss2) {
  if (sstring_is_empty(ss1) && !sstring_is_empty(ss2)) {
    ss1->length = ss2->length;
    ss1->chars = (char *)malloc(sizeof(char) * (ss1->length));
    for (unsigned int i = 0; i < ss1->length; i++) {
      ss1->chars[i] = ss2->chars[i];
    }
  } else if (!sstring_is_empty(ss1)) {
    int lengthss1 = sstring_get_length(ss1),
        lengthss2 = sstring_get_length(ss2), i;
    ss1->chars =
        (char *)realloc(ss1->chars, sizeof(char) * (lengthss1 + lengthss2));
    ss1->length = (lengthss1 + lengthss2);
    for (i = 0; i < lengthss2; i++) {
      ss1->chars[i + lengthss1] = ss2->chars[i];
    }
  }
}

sstring sstring_copy(sstring ss) {
  ASSERT_SSTRING_OK(ss);
  if (sstring_is_empty(ss)) {
    return sstring_create_empty();
  } else {
    sstring res = malloc(sizeof(sstring_struct));
    assert(NULL != res);
    res->length = ss->length;
    res->chars = malloc(res->length * sizeof(char));
    assert(NULL != res->chars);
    for (unsigned int i = 0; i < res->length; i++) {
      res->chars[i] = ss->chars[i];
    }
    ASSERT_SSTRING_OK(res);
    return res;
  }
}

int sstring_compare(sstring ss1, sstring ss2) {
  ASSERT_SSTRING_OK(ss1);
  ASSERT_SSTRING_OK(ss2);
  int i = 0;
  if (!sstring_is_empty(ss1) && !sstring_is_empty(ss2)) {
    do {
      if ((ss1->chars[i] > ss2->chars[i]) &&
          (ss1->chars[i] != ss2->chars[i] + 32)) {
        return 1;
      } else if ((ss1->chars[i] < ss2->chars[i]) &&
                 (ss1->chars[i] != ss2->chars[i] - 32)) {
        return -1;
      }
      i++;
    } while (i < sstring_get_length(ss1) || i < sstring_get_length(ss2));

    return 0;
  } else if (sstring_is_empty(ss1) && !sstring_is_empty(ss2)) {
    return -1;
  } else if (sstring_is_empty(ss2) && !sstring_is_empty(ss1)) {
    return 1;
  } else {
    return 0;
  }
}

int sstring_get_length(sstring ss) {
  ASSERT_SSTRING_OK(ss);
  return ss->length;
}

char sstring_get_char(sstring ss, int i) { return ss->chars[i]; }

bool sstring_is_integer(sstring ss, int *n_pt) {
  ASSERT_SSTRING_OK(ss);
  bool is_digit = false;
  for (int i = 0; i < sstring_get_length(ss); i++) {
    is_digit = isdigit(ss->chars[i]);
  }
  if (is_digit) {
    *n_pt = 0;
    for (int i = 0; i < sstring_get_length(ss); i++) {
      *n_pt = (ss->chars[i] - '0') + *n_pt * 10;
      is_digit = isdigit(ss->chars[i]);
    }
  }
  return is_digit;
}
