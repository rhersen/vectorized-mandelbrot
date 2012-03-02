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


typedef double v2df __attribute__ ((vector_size(16))); /* vector of two doubles */
typedef int v4si __attribute__ ((vector_size(16))); /* vector of four ints */


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
uint8_t *bitmap;

static void write(int x, int y, int two_pixels) {
    /*
     * Add the two pixels to the bitmap, all bits are
     * initially zero since the area was allocated with
     * calloc()
     */
    uint8_t *row_bitmap = bitmap + (bytes_per_row * y);
    row_bitmap[x >> 3] |= (uint8_t) (two_pixels >> (x & 7));
}

static void calc_row(int y) {
    const v2df Civ_init = { y*inverse_h-1.0, y*inverse_h-1.0 };

    for (int x=0; x<N; x+=2)
    {
        v2df Crv = Crvs[x >> 1];
        v2df Civ = Civ_init;
        v2df Zrv = zero;
        v2df Ziv = zero;
        v2df Trv = zero;
        v2df Tiv = zero;
        int two_pixels = 1;
        v2df is_still_bounded;

        for (int i = 0; i < 50 && two_pixels; ++i) {
            Ziv = (Zrv*Ziv) + (Zrv*Ziv) + Civ;
            Zrv = Trv - Tiv + Crv;
            Trv = Zrv * Zrv;
            Tiv = Ziv * Ziv;
            is_still_bounded = __builtin_ia32_cmplepd(Trv + Tiv, four);
            two_pixels = __builtin_ia32_movmskpd(is_still_bounded);
        }

        write(x, y, two_pixels << 6);
    }
}

static void printPbm1() {
    int column = 0;
    printf("P1\n%d %d\n", N, N);

    for (int i = 0; i < bytes_per_row * N; ++i) {
        for (int j = 7; j >= 0; --j) {
            printf("%d", (bitmap[i] >> j) & 1);

            if (++column >= 70) {
                printf("\n");
                column = 0;
            }
        }
    }
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

    bitmap = calloc(bytes_per_row, N);
    if (bitmap == NULL)
        return EXIT_FAILURE;

#pragma omp parallel for schedule(static,1)
    for (i = 0; i < N; i++)
        calc_row(i);

    printPbm4();
    free(bitmap);
    free(Crvs);

    return EXIT_SUCCESS;
}
