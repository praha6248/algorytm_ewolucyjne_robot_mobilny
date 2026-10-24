#pragma once
// Minimalne makra testowe (bez zewnetrznych bibliotek).
#include <cmath>
#include <cstdio>
#include <cstdlib>

inline int g_failures = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::fprintf(stderr, "%s:%d: CHECK(%s) nie spelniony\n", __FILE__,    \
                         __LINE__, #cond);                                       \
            ++g_failures;                                                        \
        }                                                                        \
    } while (0)

#define CHECK_NEAR(a, b, tol)                                                    \
    do {                                                                         \
        const double va_ = (a), vb_ = (b);                                       \
        if (std::fabs(va_ - vb_) > (tol)) {                                      \
            std::fprintf(stderr, "%s:%d: %s = %g, oczekiwano %g (tol %g)\n",      \
                         __FILE__, __LINE__, #a, va_, vb_, (double)(tol));       \
            ++g_failures;                                                        \
        }                                                                        \
    } while (0)

#define TEST_RESULT()                                                            \
    (g_failures == 0 ? (std::printf("OK\n"), 0)                                  \
                     : (std::fprintf(stderr, "%d blad(ow)\n", g_failures), 1))
