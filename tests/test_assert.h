#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*test_fn_t)(void);

typedef struct {
    const char *name;
    test_fn_t fn;
} test_case_t;

#define TEST_ASSERT(cond)                                                                 \
    do {                                                                                  \
        if (!(cond)) {                                                                    \
            fprintf(stderr, "assertion failed at %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            return 1;                                                                     \
        }                                                                                 \
    } while (0)

#define TEST_ASSERT_INT_EQ(expected, actual)                                              \
    do {                                                                                  \
        int expected_ = (expected);                                                       \
        int actual_ = (actual);                                                           \
        if (expected_ != actual_) {                                                       \
            fprintf(stderr,                                                               \
                    "assertion failed at %s:%d: expected %d, got %d\n",               \
                    __FILE__, __LINE__, expected_, actual_);                              \
            return 1;                                                                     \
        }                                                                                 \
    } while (0)

#define TEST_ASSERT_STR_EQ(expected, actual)                                              \
    do {                                                                                  \
        const char *expected_ = (expected);                                               \
        const char *actual_ = (actual);                                                   \
        if (strcmp(expected_, actual_) != 0) {                                            \
            fprintf(stderr,                                                               \
                    "assertion failed at %s:%d: expected \"%s\", got \"%s\"\n", \
                    __FILE__, __LINE__, expected_, actual_);                              \
            return 1;                                                                     \
        }                                                                                 \
    } while (0)

#define TEST_ASSERT_FLOAT_EQ(expected, actual, epsilon)                                   \
    do {                                                                                  \
        float expected_ = (expected);                                                     \
        float actual_ = (actual);                                                         \
        float epsilon_ = (epsilon);                                                       \
        if (fabsf(expected_ - actual_) > epsilon_) {                                      \
            fprintf(stderr,                                                               \
                    "assertion failed at %s:%d: expected %f, got %f\n",               \
                    __FILE__, __LINE__, expected_, actual_);                              \
            return 1;                                                                     \
        }                                                                                 \
    } while (0)

static int run_test_cases(const test_case_t *cases, size_t count) {
    size_t i;
    int failed = 0;

    for (i = 0; i < count; i++) {
        int rc = cases[i].fn();
        if (rc != 0) {
            failed++;
            fprintf(stderr, "FAIL: %s\n", cases[i].name);
        } else {
            printf("PASS: %s\n", cases[i].name);
        }
    }

    return failed == 0 ? 0 : 1;
}

#endif
