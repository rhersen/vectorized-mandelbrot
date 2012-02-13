#include <stdio.h>
#include <sys/time.h>
#include "mandelbrot.h"

#define assertEquals(a,e) _assertEquals(a,e,__FILE__,__LINE__)

static void _assertEquals(int actual, int expected, char *file, int line) {
    if (actual == expected) {
        printf("%s:%d:\033[0;32m %d OK\033[0m\n", file, line, actual);
    } else {
        printf("%s:%d:\033[0;31m expected %d but got %d\033[0m\n",
               file, line, expected, actual);
    }
}

int main(void) {
    struct timeval start;
    struct timeval now;
    gettimeofday(&start, 0);

    assertEquals(getIterations(-0.75, -0.5, 16), 6);
    assertEquals(getIterations(-0.5, -0.5, 16), 0);
    assertEquals(getIterations(-0.25, -0.5, 16), 0);
    assertEquals(getIterations(0.25, -0.25, 16), 0);
    assertEquals(getIterations(0.5, -0.25, 16), 5);
    assertEquals(getIterations(0.75, -0.25, 16), 3);
    assertEquals(getIterations(-0.75, 0.25, 16), 13);
    assertEquals(getIterations(-0.5, 0.25, 16), 0);
    assertEquals(getIterations(0.25, -0.5625, 4096), 1402);

    float a[8] = {-0.8, -0.5, -0.2, 0.2, 0.5, 0.7, -0.75, -0.4};
    float it[8];
    iterations(a, -0.25, 16, it);

    assertEquals(it[0], 11);
    assertEquals(it[1], 0);
    assertEquals(it[2], 0);
    assertEquals(it[3], 0);
    assertEquals(it[4], 5);
    assertEquals(it[5], 3);
    assertEquals(it[6], 13);
    assertEquals(it[7], 0);

    float a2[8] = {-2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5};
    iterations(a2, -0.318182, 32, it);

    assertEquals(it[0], 1);
    assertEquals(it[1], 5);
    assertEquals(it[2], 16);
    assertEquals(it[3], 0);
    assertEquals(it[4], 0);
    assertEquals(it[5], 6);
    assertEquals(it[6], 2);
    assertEquals(it[7], 2);

    gettimeofday(&now, 0);
    printf("%f\n", now.tv_usec * 1e-6 - start.tv_usec * 1e-6 +
           (now.tv_sec - start.tv_sec));

    return 0;
}
