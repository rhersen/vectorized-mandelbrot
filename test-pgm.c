#include <stdio.h>
#include <sys/time.h>
#include "pgm.h"

#define assertEquals(a,e) _assertEquals(a,e,__FILE__,__LINE__)

static void _assertEquals(int actual, int expected, char *file, int line) {
    if (actual == expected) {
        printf("%s:%d:\033[0;32m %d OK\033[0m\n", file, line, actual);
    } else {
        printf("%s:%d:\033[0;31m expected %d but got %d\033[0m\n",
               file, line, expected, actual);
    }
}

#define assertFalse(a) _assertTrue(!(a),__FILE__,__LINE__)
#define assertTrue(a) _assertTrue((a),__FILE__,__LINE__)

static void _assertTrue(int actual, char *file, int line) {
    if (actual) {
        printf("%s:%d:\033[0;32m %d OK\033[0m\n", file, line, actual);
    } else {
        printf("%s:%d:\033[0;31m got %d\033[0m\n",
               file, line, actual);
    }
}

int main(void) {
    struct timeval start;
    struct timeval now;
    gettimeofday(&start, 0);

    double iw = 2 / 2e2;
    double ih = iw;

    assertEquals(calc_1(0, 1, 16, iw, ih), 3);
    assertEquals(calc_1(7, 1, 16, iw, ih), 3);

    assertEquals(calc_1(65, 64, 255, iw, ih), 7);
    assertEquals(calc_1(66, 64, 255, iw, ih), 8);
    assertEquals(calc_1(72, 64, 255, iw, ih), 8);

    assertEquals(calc_1(77, 78, 31, iw, ih), 0);
    assertEquals(calc_1(77, 78, 33, iw, ih), 32);

    assertEquals(calc_1(72, 78, 255, iw, ih), 13);
    assertEquals(calc_1(73, 78, 255, iw, ih), 13);
    assertEquals(calc_1(74, 78, 255, iw, ih), 14);
    assertEquals(calc_1(75, 78, 255, iw, ih), 15);
    assertEquals(calc_1(76, 78, 255, iw, ih), 17);
    assertEquals(calc_1(77, 78, 255, iw, ih), 32);
    assertEquals(calc_1(78, 78, 255, iw, ih), 85);
    assertEquals(calc_1(79, 78, 255, iw, ih), 0);

    assertEquals(calc_1(78, 77, 255, iw, ih), 143);
    assertEquals(calc_1(78, 78, 255, iw, ih), 85);
    assertEquals(calc_1(78, 79, 255, iw, ih), 0);
    assertEquals(calc_1(79, 78, 255, iw, ih), 0);
    assertEquals(calc_1(79, 79, 255, iw, ih), 0);

    assertEquals(calc_1(100, 100, 16, iw, ih), 0);

    int r[2] = { 0 };

    calc_2(0, 1, 16, iw, ih, r);
    assertEquals(r[0], 3);
    assertEquals(r[1], 3);

    calc_2(77, 78, 0x10000000, iw, ih, r);
    assertEquals(r[0], 32);
    assertEquals(r[1], 85);
    gettimeofday(&now, 0);
    float elapsed = now.tv_usec * 1e-6 - start.tv_usec * 1e-6 + (now.tv_sec - start.tv_sec);
    assertTrue(elapsed < 0.25);

    calc_2(78, 77, 255, iw, ih, r);
    assertEquals(r[0], 143);
    assertEquals(r[1], 0);

    calc_2(100, 100, 255, iw, ih, r);
    assertEquals(r[0], 0);
    assertEquals(r[1], 0);

    gettimeofday(&now, 0);
    printf("%f\n", now.tv_usec * 1e-6 - start.tv_usec * 1e-6 +
           (now.tv_sec - start.tv_sec));

    return 0;
}
