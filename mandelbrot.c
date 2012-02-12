#include "mandelbrot.h"

static float nextReal(float re, float im, float x) {
    return re * re - im * im + x;
}

static float nextImag(float re, float im, float y) {
    return 2.0 * re * im + y;
}

static int hasEscaped(float re, float im) {
    return re * re + im * im > 4.0;
}

int getIterations(float x, float y, int limit) {
    float real = 0, imag = 0;
    int count;

    for (count = 1; count < limit && !hasEscaped(real, imag); ++count) {
        float r1 = nextReal(real, imag, x);
        float i1 = nextImag(real, imag, y);
        real = r1;
        imag = i1;
    }

    return count < limit ? count - 1 : 0;
}

void iterations(float* xs, float y, int limit, int* result) {
    for (int i = 0; i < 8; ++i) {
        result[i] = getIterations(xs[i], y, limit);
    }
}
