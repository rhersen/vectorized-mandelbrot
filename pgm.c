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

#define V v2df
#define NV 2
#define IS __builtin_ia32_movmskpd
#define AND __builtin_ia32_andpd
#define OR __builtin_ia32_orpd
#define EQ(a,b) __builtin_ia32_cmpeqpd(a,b)
#define GT(a,b) __builtin_ia32_cmpltpd(b,a)

void calc_2(int x, int y,
            int limit,
            double inv_w, double inv_h,
            int* r) {
    V zero, one, four, Civ, Crv, rv = { 0.0 };

    void writeOutput() {
        for (int i = 0; i < NV; ++i) {
            r[i] = rv[i] ? rv[i] + 1 : 0;
        }
    }

    void initVectors(double civ) {
        for (int i = 0; i < NV; ++i) {
            Crv[i] = (x + i) * inv_w - 1.5;
            Civ[i] = civ;
            zero[i] = 0.0;
            one[i] = 1.0;
            four[i] = 4.0;
        }
    }

    initVectors(y * inv_h - 1.0);

    V Zrv = zero, Ziv = zero, Trv = zero, Tiv = zero;

    for (V i = zero; i[0] < limit && IS(EQ(rv, zero)); i += one) {
        Ziv = Zrv * Ziv + Zrv * Ziv + Civ;
        Zrv = Trv - Tiv + Crv;
        Trv = Zrv * Zrv;
        Tiv = Ziv * Ziv;

        V escaped = GT(Trv + Tiv, four);
        V shouldSet = AND(EQ(rv, zero), escaped);
        rv = OR(rv, AND(shouldSet, i));
    }

    writeOutput();
}

#undef V
#undef NV
#undef IS
#undef AND
#undef OR
#undef EQ
#undef GT

#define V v4df
#define NV 4
#define IS __builtin_ia32_movmskpd256
#define AND __builtin_ia32_andpd256
#define OR __builtin_ia32_orpd256
#define EQ(a,b) __builtin_ia32_cmppd256(a,b,0)
#define GT(a,b) __builtin_ia32_cmppd256(b,a,1)

void calc_4(int x, int y,
            int limit,
            double inv_w, double inv_h,
            int* r) {
    V zero, one, four, Civ, Crv, rv = { 0.0 };

    void writeOutput() {
        for (int i = 0; i < NV; ++i) {
            r[i] = rv[i] ? rv[i] + 1 : 0;
        }
    }

    void initVectors(double civ) {
        for (int i = 0; i < NV; ++i) {
            Crv[i] = (x + i) * inv_w - 1.5;
            Civ[i] = civ;
            zero[i] = 0.0;
            one[i] = 1.0;
            four[i] = 4.0;
        }
    }

    initVectors(y * inv_h - 1.0);

    V Zrv = zero, Ziv = zero, Trv = zero, Tiv = zero;

    for (V i = zero; i[0] < limit && IS(EQ(rv, zero)); i += one) {
        Ziv = Zrv * Ziv + Zrv * Ziv + Civ;
        Zrv = Trv - Tiv + Crv;
        Trv = Zrv * Zrv;
        Tiv = Ziv * Ziv;

        V escaped = GT(Trv + Tiv, four);
        V shouldSet = AND(EQ(rv, zero), escaped);
        rv = OR(rv, AND(shouldSet, i));
    }

    writeOutput();
}

#undef V
#undef NV
#undef IS
#undef AND
#undef OR
#undef EQ
#undef GT

#define V v8sf
#define NV 8
#define IS __builtin_ia32_movmskps256
#define AND __builtin_ia32_andps256
#define OR __builtin_ia32_orps256
#define EQ(a,b) __builtin_ia32_cmpps256(a,b,0)
#define GT(a,b) __builtin_ia32_cmpps256(b,a,1)

void calc_8(int x, int y,
            int limit,
            double inv_w, double inv_h,
            int* r) {
    V zero, one, four, Civ, Crv, rv = { 0.0 };

    void writeOutput() {
        for (int i = 0; i < NV; ++i) {
            r[i] = rv[i] ? rv[i] + 1 : 0;
        }
    }

    void initVectors(double civ) {
        for (int i = 0; i < NV; ++i) {
            Crv[i] = (x + i) * inv_w - 1.5;
            Civ[i] = civ;
            zero[i] = 0.0;
            one[i] = 1.0;
            four[i] = 4.0;
        }
    }

    initVectors(y * inv_h - 1.0);

    V Zrv = zero, Ziv = zero, Trv = zero, Tiv = zero;

    for (V i = zero; i[0] < limit && IS(EQ(rv, zero)); i += one) {
        Ziv = Zrv * Ziv + Zrv * Ziv + Civ;
        Zrv = Trv - Tiv + Crv;
        Trv = Zrv * Zrv;
        Tiv = Ziv * Ziv;

        V escaped = GT(Trv + Tiv, four);
        V shouldSet = AND(EQ(rv, zero), escaped);
        rv = OR(rv, AND(shouldSet, i));
    }

    writeOutput();
}
