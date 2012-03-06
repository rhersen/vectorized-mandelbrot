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

static double getElapsedSecondsSince(struct timeval* start) {
    struct timeval now;
    gettimeofday(&now, 0);
    int usec = now.tv_usec - start->tv_usec;
    int sec = now.tv_sec - start->tv_sec;

    return usec * 1e-6 + sec;
}

int main(void) {
    struct timeval start;

    double iw = 2 / 2e2;
    double ih = iw;

    void assertCalc(int x, int y, int limit, int* e) {
        for (int i = 0; i < 4; ++i) {
            assertEquals(calc_1(x + i, y, limit, iw, ih), e[i]);
        }

        int r[2] = { 0 };

        for (int i = 0; i < 4; i += 2) {
            calc_2(x + i, y, limit, iw, ih, r);

            for (int j = 0; j < 2; ++j) {
                assertEquals(r[j], e[i + j]);
            }
        }
    }

    gettimeofday(&start, 0);

    assertCalc(0, 1, 16, (int[]) {3, 3, 3, 3});
    assertCalc(65, 64, 255, (int[]) {7, 8, 8, 8});
    assertCalc(72, 64, 255, (int[]) {8, 8, 9, 9});
    assertCalc(72, 78, 255, (int[]) {13, 13, 14, 15});
    assertCalc(78, 78, 255, (int[]) {85, 0, 0, 0});
    assertCalc(77, 77, 255, (int[]) {22, 143, 0, 0});
    assertCalc(100, 100, 255, (int[]) {0, 0, 0, 0});

    assertEquals(calc_1(77, 78, 31, iw, ih), 0);
    assertEquals(calc_1(77, 78, 33, iw, ih), 32);

    assertCalc(74, 78, 0x10000000, (int[]) {14, 15, 17, 32});
    assertTrue(getElapsedSecondsSince(&start) < 0.25);

    printf("%lf seconds\n", getElapsedSecondsSince(&start));

    return 0;
}
