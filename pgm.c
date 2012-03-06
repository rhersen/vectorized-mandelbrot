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
#include <stdlib.h>


typedef double v2df __attribute__ ((vector_size(16)));

const v2df zero = { 0.0, 0.0 };
const v2df one = { 1.0, 1.0 };
const v2df four = { 4.0, 4.0 };

/*
 * Mandelbrot bitmap
 */
static uint8_t *bitmap;

static void write(int N, int x, int y, int two_pixels) {
    uint8_t *row_bitmap = bitmap + (N * y);
    row_bitmap[x] = two_pixels;
}

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

void calc_2(int x, int y, int limit, double inverse_w, double inverse_h, int* r) {
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

static void calc_row(int N, int y, double inverse_w, double inverse_h) {
    for (int x=0; x<N; x+=2)
    {
        int r[2] = { 0 };

        /* calc_2(x, y, 50, inverse_w, inverse_h, r); */
        r[0] = calc_1(x, y, 50, inverse_w, inverse_h);
        r[1] = calc_1(x + 1, y, 50, inverse_w, inverse_h);

        write(N, x, y, r[0]);
        write(N, x + 1, y, r[1]);
    }
}

static void printPgm2(int N) {
    int column = 0;
    printf("P2\n%d %d %d\n", N, N, 255);

    for (int i = 0; i < N * N; ++i) {
        printf(" %d", bitmap[i]);

        if (++column >= 70) {
            printf("\n");
            column = 0;
        }
    }

    printf("\n");
}

static void printPbm4(int N, int bytes_per_row) {
    printf("P4\n%d %d\n", N, N);
    fwrite(bitmap, bytes_per_row, N, stdout);
}

int maine(int argc, char **argv)
{
    int i;

/*
 * Constant throughout the program, value depends on N
 */
int bytes_per_row;
double inverse_w;
double inverse_h;

/*
 * Program argument: height and width of the image
 */
int N;

/*
 * Lookup table for initial real-axis value
 */
v2df *Crvs;

    N = atoi(argv[1]);
    bytes_per_row = (N + 7) >> 3;

    inverse_w = 2.0 / (bytes_per_row << 3);
    inverse_h = 2.0 / N;

    /*
     * Crvs must be 16-bytes aligned on some CPU:s.
     */
    if (posix_memalign((void**)&Crvs, sizeof(v2df), sizeof(v2df) * N / 2))
        return EXIT_FAILURE;

#pragma omp parallel for
    for (i = 0; i < N; i+=2) {
        v2df Crv = { (i+1.0)*inverse_w-1.5, (i)*inverse_w-1.5 };
        Crvs[i >> 1] = Crv;
    }

    bitmap = calloc(N, N);
    if (bitmap == NULL)
        return EXIT_FAILURE;

#pragma omp parallel for schedule(static,1)
    for (i = 0; i < N; i++)
        calc_row(N, i, inverse_w, inverse_h);

    printPgm2(N);
    free(bitmap);
    free(Crvs);

    return EXIT_SUCCESS;
}
