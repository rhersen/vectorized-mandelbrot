#include "mandelbrot.h"
#include "immintrin.h"

static float nextReal(float re, float im, float x) {
    float product1 = re * re;
    float product2 = im * im;
    float diff = product1 - product2;
    float sum = diff + x;
    return sum;
}

static float nextImag(float re, float im, float y) {
    float product1 = 2.0 * re;
    float product2 = product1 * im;
    float sum = product2 + y;
    return sum;
}

static int hasEscaped(float re, float im) {
    float product1 = re * re;
    float product2 = im * im;
    float sum = product1 + product2;
    return sum > 4.0;
}

int getIterations(float x, float y, int limit) {
    float real = 0, imag = 0;
    int result = 0;
    int count;

    for (count = 1; count < limit; ++count) {
        float r1 = nextReal(real, imag, x);
        float i1 = nextImag(real, imag, y);
        real = r1;
        imag = i1;

        if (!result && hasEscaped(real, imag)) {
            return count;
        }
    }

    return result;
}

#define plus _mm256_add_ps
#define minus _mm256_sub_ps
#define times _mm256_mul_ps
#define and _mm256_and_ps

typedef __m256 v;

void iterations(float* xs, float yf, float limit, float* result) {
    v inline square(v x) {
        return times(x, x);
    }

    v inline lessThan(v x, v y) {
        return _mm256_cmp_ps(x, y, 1);
    }

    v inline equal(v x, v y) {
        return _mm256_cmp_ps(x, y, 0);
    }

    v real = {0}, imag = {0}, x, c0, c1, c4, y, limits, results;

    for (int i = 0; i < 8; ++i) {
        x[i] = xs[i];
        y[i] = yf;
        c0[i] = 0;
        c1[i] = 1;
        c4[i] = 4;
        limits[i] = limit;
        results[i] = 0;
    }

    v notDone = equal(results, c0);

    for (v i = plus(c0, c1);
         !_mm256_testz_ps(notDone, lessThan(i, limits));
         i = plus(i, c1)) {
        v r1 = plus(minus(square(real), square(imag)), x);
        v i1 = plus(times(plus(real, real), imag), y);
        v isEscaped = lessThan(c4, plus(square(r1), square(i1)));
        notDone = equal(results, c0);
        v isEscapedNow = and(notDone, isEscaped);
        results = plus(results, and(isEscapedNow, i));
        real = r1;
        imag = i1;
    }

    for (int i = 0; i < 8; ++i) {
        result[i] = results[i];
    }
}
