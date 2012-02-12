#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mandelbrot.h"

#define RESN 512

static Display *display;
static Window win;
static GC gc;

static void createWindow() {
    unsigned int width, height, x, y, border_width;
    unsigned screen;
    char *window_name = "Mandelbrot Set", *display_name = NULL;
    unsigned
        long valuemask = 0;
    XGCValues values;
    XSizeHints size_hints;
    XSetWindowAttributes attr[1];

    if ( (display = XOpenDisplay (display_name)) == NULL ) {
        fprintf (stderr, "drawon: cannot connect to X server %s\n",
                 XDisplayName (display_name) );
        exit (-1);
    }
 
    screen = DefaultScreen (display);

    /* set window size */

    width = RESN;
    height = RESN;

    /* set window position */

    x = 0;
    y = 0;

    /* create opaque window */

    border_width = 4;
    win = XCreateSimpleWindow (display, RootWindow (display, screen),
                               x, y, width, height, border_width, 
                               BlackPixel (display, screen),
                               WhitePixel (display, screen));

    size_hints.flags = USPosition|USSize;
    size_hints.x = x;
    size_hints.y = y;
    size_hints.width = width;
    size_hints.height = height;
    size_hints.min_width = 300;
    size_hints.min_height = 300;
 
    XSetNormalHints (display, win, &size_hints);
    XStoreName(display, win, window_name);

    /* create graphics context */

    gc = XCreateGC (display, win, valuemask, &values);

    XSetBackground (display, gc, WhitePixel (display, screen));
    XSetForeground (display, gc, BlackPixel (display, screen));
    XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);

    attr[0].backing_store = Always;
    attr[0].backing_planes = 1;
    attr[0].backing_pixel = BlackPixel(display, screen);

    XChangeWindowAttributes(display, win,
                            CWBackingStore | CWBackingPlanes | CWBackingPixel,
                            attr);

    XMapWindow (display, win);
    XSync(display, 0);
}

int main ()
{
    createWindow();

    struct timeval start;
    struct timeval now;
    gettimeofday(&start, 0);

    float scale = RESN / 4.0;
 
    for(int limit = 1; limit <= 256; ++limit) {
        for(int i=0; i < RESN; i++) {
            float cImag = (i - RESN / 2.2)/scale;
            for(int j=0; j < RESN; j += 8) {
                float a[8];
                int result[8];
                for (int k = 0; k < 8; ++k) {
                    float cReal = (j + k - RESN / 2.0)/scale;
                    a[k] = cReal;
                }

                iterations(a, cImag, limit, result);

                for (int k = 0; k < 8; ++k) {
                    /* if (getIterations(a[k], cImag, limit)) { */
                    if (result[k]) {
                        XDrawPoint (display, win, gc, j + k, i);
                    }
                }
            }
        }
    }

    gettimeofday(&now, 0);
    printf("\n\n%f\n", now.tv_usec * 1e-6 - start.tv_usec * 1e-6 +
           (now.tv_sec - start.tv_sec));
 
    XFlush (display);
}
