#pragma once
#include <string>
#include <stdio.h>

#define ASSERT(expect, actual, message)                               \
  {                                                                   \
    std::string actual_str(actual);                                   \
    std::string expect_str(expect);                                   \
      if (expect_str != actual_str) {                                 \
        fprintf(stderr,                                               \
                "Assertion failed at %s:%d: %s\n",                    \
                __FILE__, __LINE__, message);                         \
        fprintf(stderr, "Expected : %s\n", expect_str.c_str());       \
        fprintf(stderr, "Actual   : %s\n", actual_str.c_str());       \
        exit(1);                                                      \
      }                                                               \
  }

#define ASSERT_INT_EQUAL(expect, actual, message)                     \
  {                                                                   \
    size_t actual_v = (size_t)(actual);                               \
    size_t expect_v = (size_t)(expect);                               \
      if (expect_v != actual_v) {                                     \
        fprintf(stderr,                                               \
                "Assertion failed at %s:%d : %s\n",                   \
                __FILE__, __LINE__, message);                         \
        fprintf(stderr, "Expected : %ld\n", expect_v);                \
        fprintf(stderr, "Actual   : %ld\n", actual_v);                \
        exit(1);                                                      \
      }                                                               \
  }

#define ASSERT_NOT_NULL(actual, message)                              \
  {                                                                   \
    if (actual.get() == nullptr) {                                    \
        fprintf(stderr, "Assertion failedat %s:%d %s\n",              \
                __FILE__, __LINE__, message);                         \
        exit(1);                                                      \
      }                                                               \
  }

#define ASSERT_NULL(actual, message)                                  \
  {                                                                   \
    if (actual.get() != nullptr) {                                    \
        fprintf(stderr, "Assertion failedat %s:%d %s\n",              \
                __FILE__, __LINE__, message);                         \
        exit(1);                                                      \
      }                                                               \
  }


#define ASSERT_BOOL(expect, actual, message)                          \
  {                                                                   \
    bool expect_val = expect;                                         \
    bool actual_val = actual;                                         \
      if (expect_val != actual_val) {                                 \
        fprintf(stderr, "Assertion failed: %s %d\n", message, __LINE__);        \
        fprintf(stderr, "Expected : %s\n", expect_val ? "true" : "false"); \
        fprintf(stderr, "Actual   : %s\n", actual_val ? "true" : "false"); \
        exit(1);                                                      \
      }                                                               \
  }

