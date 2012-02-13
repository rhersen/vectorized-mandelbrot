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
            result = count;
        }
    }

    return result;
}

void iterations(float* xs, float y, float limit, float* result) {
    __m256 real = {0};
    __m256 imag = {0};
    __m256 product1;
    __m256 product2;
    __m256 diff;
    __m256 r1;
    __m256 i1;
    __m256 sum;
    __m256 xsv;
    __m256 zero;
    __m256 two;
    __m256 four;
    __m256 yv;
    __m256 isEscaped;
    __m256 notDone;
    __m256 counts;
    __m256 results;
    __m256 tmp;

    for (int i = 0; i < 8; ++i) {
        result[i] = 0;
        xsv[i] = xs[i];
        zero[i] = 0;
        two[i] = 2;
        four[i] = 4;
        yv[i] = y;
        notDone[i] = 1;
        results[i] = 0;
    }

    for (float count = 1; count < limit; ++count) {
        product1 = _mm256_mul_ps(real, real);
        product2 = _mm256_mul_ps(imag, imag);
        diff = _mm256_sub_ps(product1, product2);
        r1 = _mm256_add_ps(diff, xsv);
        product1 = _mm256_mul_ps(two, real);
        product2 = _mm256_mul_ps(product1, imag);
        i1 = _mm256_add_ps(product2, yv);
        product1 = _mm256_mul_ps(r1, r1);
        product2 = _mm256_mul_ps(i1, i1);
        sum = _mm256_add_ps(product1, product2);
        isEscaped = _mm256_cmp_ps(four, sum, 1);
        counts = _mm256_broadcast_ss(&count);
        tmp = _mm256_cmp_ps(results, zero, 0);
        tmp = _mm256_and_ps(tmp, isEscaped);
        tmp = _mm256_and_ps(tmp, counts);
        results = _mm256_add_ps(results, tmp);
        real = r1;
        imag = i1;
    }

    for (int i = 0; i < 8; ++i) {
        result[i] = results[i];
    }
}
