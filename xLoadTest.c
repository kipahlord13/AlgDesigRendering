#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <sys/param.h>
#include <sys/time.h>

Display *d;
Window w;
XEvent e;
int s;
GC gc;

XImage* image;

long pixels[600][600];

int setupX11() {
  d = XOpenDisplay(NULL);
  if (d == NULL) {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }

  s = DefaultScreen(d);
  w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 600, 600, 40,
  BlackPixel(d, s), WhitePixel(d, s));
  XSelectInput(d, w, ExposureMask | KeyPressMask);
  XMapWindow(d, w);
  gc = DefaultGC(d, s);
  return 0;
}

int buildColor(double red, double green, double blue) {
    return(
        (((int)(red*255)%256)<<16)+
        (((int)(green*255)%256)<<8)+
        (((int)(blue*255)%256)));
}

void fillPixels() {
  for(int i = 0; i < 600; i++) {
    for(int j = 0; j < 600; j++) {
      float grad = (i + j) / 1200.0;
      pixels[i][j] = buildColor(grad, grad, grad);
    }
  }
}

void main() {
  setupX11();
  fillPixels();
  for(int i = 0; i < 600; i++) {
    for(int j = 0; j < 600; j++) {
      int r = XSetForeground(d, DefaultGC(d, s), pixels[i][j]);
      if(r != 1) printf("Bad set foreground %d\n", r);
      r = XDrawPoint(d, w, DefaultGC(d, s), i, j);
      if(r != 1) printf("Bad draw point %d\n", r);
    }
  }
  for(int i = 0; i < 600; i++) {
    for(int j = 0; j < 600; j++) {
      int r = XSetForeground(d, DefaultGC(d, s), pixels[i][j]);
      if(r != 1) printf("Bad set foreground %d\n", r);
      r = XDrawPoint(d, w, DefaultGC(d, s), i, j);
      if(r != 1) printf("Bad draw point %d\n", r);
    }
  }
  XFlush(d);
  image = XGetImage(d, w, 0, 0, 600, 600, AllPlanes, ZPixmap);
  printf("%ld\n", XGetPixel(image, 40, 40));
  int color = 0;
  while(1) {
    // if(XCheckMaskEvent(d, ExposureMask | KeyPressMask, &e)) {
    //   if (e.type == KeyPress) {
    //     break;
    //   }
    // }
    // for(int i = 0; i < 600; i++) {
    //   for(int j = 0; j < 600; j++) {
    //     int r = XSetForeground(d, DefaultGC(d, s), pixels[i][j]);
    //     if(r != 1) printf("Bad set foreground %d\n", r);
    //     r = XDrawPoint(d, w, DefaultGC(d, s), 599 - i, 599 - j);
    //     if(r != 1) printf("Bad draw point %d\n", r);
    //   }
    // }
    // XFlush(d);
    for(int i = 0; i < 600; i++) {
      for(int j = 0; j < 600; j++) {
        XPutPixel(image, i, j, 0x00FFFF);
      }
    }
    if(color % 500 < 250) {
      XPutImage(d, w, gc, image, 0, 0, 0, 0, 600, 600);
    } else {
      XPutImage(d, w, gc, image, 100, 0, 0, 0, 500, 600);
    }
    if(++color % 10 == 0) printf("frame no: %d\n", color);
    //printf("draw time: %lld\n", timeInMicroseconds() - drawStart);
  }
}
