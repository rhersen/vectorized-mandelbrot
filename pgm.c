/*The Computer Language Benchmarks Game
  http://shootout.alioth.debian.org/

  contributed by Paolo Bonzini
  further optimized by Jason Garrett-Glaser
  pthreads added by Eckehard Berns
  further optimized by Ryan Henszey
  modified by Samy Al Bahra (use GCC atomic builtins)
  modified by Kenneth Jonsson
*/

#include <stdint.h>
#include <stdio.h>
#include "pgm.h"

const v2df zero = { 0.0, 0.0 };
const v2df one = { 1.0, 1.0 };
const v2df four = { 4.0, 4.0 };

int calc_1(int x, int y, int limit, double inverse_w, double inverse_h) {
    double Crv = x*inverse_w-1.5;
    double Civ = y*inverse_h-1.0;
    double Zrv = 0.;
    double Ziv = 0.;
    double Trv = 0.;
    double Tiv = 0.;
    int i;

    for (i = 0; i < limit && Trv + Tiv < 4.; ++i) {
        Ziv = (Zrv*Ziv) + (Zrv*Ziv) + Civ;
        Zrv = Trv - Tiv + Crv;
        Trv = Zrv * Zrv;
        Tiv = Ziv * Ziv;
    }

    return i == limit ? 0 : i;
}

static inline int isNotDone(v2df rv) {
    return __builtin_ia32_movmskpd(__builtin_ia32_cmpeqpd(rv, zero));
}

static inline v2df and(v2df v1, v2df v2, v2df v3) {
    return __builtin_ia32_andpd(__builtin_ia32_andpd(v1, v2), v3);
}

void calc_2(int x, int y,
            int limit,
            double inverse_w, double inverse_h,
            int* r) {
    const v2df Civ_init = { y*inverse_h-1.0, y*inverse_h-1.0 };
    v2df Crv = { (x)*inverse_w-1.5, (x+1.0)*inverse_w-1.5 };
    v2df Civ = Civ_init;
    v2df Zrv = zero;
    v2df Ziv = zero;
    v2df Trv = zero;
    v2df Tiv = zero;
    v2df rv = { 0. };

    for (v2df i = zero; i[0] < limit && isNotDone(rv); i += one) {
        Ziv = (Zrv*Ziv) + (Zrv*Ziv) + Civ;
        Zrv = Trv - Tiv + Crv;
        Trv = Zrv * Zrv;
        Tiv = Ziv * Ziv;

        v2df escaped = __builtin_ia32_cmpgtpd(Trv + Tiv, four);
        v2df isRvZero = __builtin_ia32_cmpeqpd(rv, zero);
        rv = __builtin_ia32_orpd(rv, and(isRvZero, escaped, i));
    }

    r[0] = rv[0] ? rv[0] + 1 : 0;
    r[1] = rv[1] ? rv[1] + 1 : 0;
}
