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

void calc_2(int x, int y,
            int limit,
            double inverse_w, double inverse_h,
            int* r) {
    const v2df zero = { 0.0, 0.0 };
    const v2df one = { 1.0, 1.0 };
    const v2df four = { 4.0, 4.0 };

    inline int isNotDone(v2df rv) {
        return __builtin_ia32_movmskpd(__builtin_ia32_cmpeqpd(rv, zero));
    }

    inline v2df and(v2df v1, v2df v2, v2df v3) {
        return __builtin_ia32_andpd(__builtin_ia32_andpd(v1, v2), v3);
    }

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

void calc_4(int x, int y,
            int limit,
            double inverse_w, double inverse_h,
            int* r) {
    const v4df zero = { 0.0 };
    const v4df one = { 1.0, 1.0, 1.0, 1.0 };
    const v4df four = { 4.0, 4.0, 4.0, 4.0 };

    inline int isNotDone(v4df rv) {
        return __builtin_ia32_movmskpd256(__builtin_ia32_cmppd256(rv, zero, 0));
    }

    inline v4df and(v4df v1, v4df v2, v4df v3) {
        return __builtin_ia32_andpd256(__builtin_ia32_andpd256(v1, v2), v3);
    }

    v4df Civ_init;
    v4df Crv;
    v4df Zrv = zero;
    v4df Ziv = zero;
    v4df Trv = zero;
    v4df Tiv = zero;
    v4df rv = { 0. };

    for (int i = 0; i < 4; ++i) {
        Crv[i] = (x + i) * inverse_w - 1.5;
        Civ_init[i] = y * inverse_h - 1.0;
    }

    v4df Civ = Civ_init;

    for (v4df i = zero; i[0] < limit && isNotDone(rv); i += one) {
        Ziv = (Zrv*Ziv) + (Zrv*Ziv) + Civ;
        Zrv = Trv - Tiv + Crv;
        Trv = Zrv * Zrv;
        Tiv = Ziv * Ziv;

        v4df escaped = __builtin_ia32_cmppd256(four, Trv + Tiv, 1);
        v4df isRvZero = __builtin_ia32_cmppd256(rv, zero, 0);
        rv = __builtin_ia32_orpd256(rv, and(isRvZero, escaped, i));
    }

    for (int i = 0; i < 4; ++i) {
        r[i] = rv[i] ? rv[i] + 1 : 0;
    }
}

void calc_8(int x, int y,
            int limit,
            double inverse_w, double inverse_h,
            int* r) {
    const v8sf zero = { 0.0 };
    const v8sf one = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
    const v8sf four = { 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0 };

    inline int isNotDone(v8sf rv) {
        return __builtin_ia32_movmskps256(__builtin_ia32_cmpps256(rv, zero, 0));
    }

    inline v8sf and(v8sf v1, v8sf v2, v8sf v3) {
        return __builtin_ia32_andps256(__builtin_ia32_andps256(v1, v2), v3);
    }

    v8sf Civ_init;
    v8sf Crv;
    v8sf Zrv = zero;
    v8sf Ziv = zero;
    v8sf Trv = zero;
    v8sf Tiv = zero;
    v8sf rv = { 0. };

    for (int i = 0; i < 8; ++i) {
        Crv[i] = (x + i) * inverse_w - 1.5;
        Civ_init[i] = y * inverse_h - 1.0;
    }

    v8sf Civ = Civ_init;

    for (v8sf i = zero; i[0] < limit && isNotDone(rv); i += one) {
        Ziv = (Zrv*Ziv) + (Zrv*Ziv) + Civ;
        Zrv = Trv - Tiv + Crv;
        Trv = Zrv * Zrv;
        Tiv = Ziv * Ziv;

        v8sf escaped = __builtin_ia32_cmpps256(four, Trv + Tiv, 1);
        v8sf isRvZero = __builtin_ia32_cmpps256(rv, zero, 0);
        rv = __builtin_ia32_orps256(rv, and(isRvZero, escaped, i));
    }

    for (int i = 0; i < 8; ++i) {
        r[i] = rv[i] ? rv[i] + 1 : 0;
    }
}
