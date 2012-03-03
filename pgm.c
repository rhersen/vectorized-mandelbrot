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
const v2df four = { 4.0, 4.0 };

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

/*
 * Mandelbrot bitmap
 */
static uint8_t *bitmap;

static void write(int x, int y, int two_pixels) {
    uint8_t *row_bitmap = bitmap + (N * y);
    row_bitmap[x] = two_pixels;
}

void calc_2(int x, int y, int limit, int* r) {
    const v2df Civ_init = { y*inverse_h-1.0, y*inverse_h-1.0 };
    v2df Crv = Crvs[x >> 1];
    v2df Civ = Civ_init;
    v2df Zrv = zero;
    v2df Ziv = zero;
    v2df Trv = zero;
    v2df Tiv = zero;
    int two_pixels = 1;
    v2df is_still_bounded;

    for (int i = 0; i < limit; ++i) {
        Ziv = (Zrv*Ziv) + (Zrv*Ziv) + Civ;
        Zrv = Trv - Tiv + Crv;
        Trv = Zrv * Zrv;
        Tiv = Ziv * Ziv;
        is_still_bounded = __builtin_ia32_cmplepd(Trv + Tiv, four);

        if (!is_still_bounded[0] && !r[0]) {
            r[0] = i;
        }

        if (!is_still_bounded[1] && !r[1]) {
            r[1] = i;
        }
    }
}

static void calc_row(int y) {
    for (int x=0; x<N; x+=2)
    {
        int r[2] = { 0 };

        calc_2(x, y, 255, r);

        write(x + 1, y, r[0]);
        write(x, y, r[1]);
    }
}

static void printPgm2() {
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

static void printPbm4() {
    printf("P4\n%d %d\n", N, N);
    fwrite(bitmap, bytes_per_row, N, stdout);
}

int main (int argc, char **argv)
{
    int i;

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
        calc_row(i);

    printPgm2();
    free(bitmap);
    free(Crvs);

    return EXIT_SUCCESS;
}
