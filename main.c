#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "pgm.h"

static uint8_t *bitmap;
static int vectorSize = 2;

static void write(int N, int x, int y, int two_pixels) {
    uint8_t *row_bitmap = bitmap + (N * y);
    row_bitmap[x] = two_pixels;
}

static void calc_row(int N, int y, double inverse_w, double inverse_h) {
    for (int x=0; x<N; x+=2)
    {
        int r[2] = { 0 };

        if (vectorSize == 2) {
            calc_2(x, y, 255, inverse_w, inverse_h, r);
        } else {
            r[0] = calc_1(x, y, 255, inverse_w, inverse_h);
            r[1] = calc_1(x + 1, y, 255, inverse_w, inverse_h);
        }

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

int main(int argc, char **argv)
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
