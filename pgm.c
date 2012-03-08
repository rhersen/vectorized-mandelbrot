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

void calc_2
#include "mandelbrot-function-body.h"

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

void calc_4
#include "mandelbrot-function-body.h"

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

void calc_8
#include "mandelbrot-function-body.h"
