/*
 * color-drawing.c - demonstrate drawing of pixels, lines, arcs, etc, using
 *		      different foreground colors, in a window.
 */

#include <X11/Xlib.h>
//nadine wuz here
#include <stdio.h>
#include <stdlib.h>		/* getenv(), etc. */
#include <unistd.h>		/* sleep(), etc.  */

#define WIDTH 800
#define HEIGHT 600

Display* display;		/* pointer to X Display structure.           */
int screen_num;		/* number of screen to place the window on.  */
Window win;			/* pointer to the newly created window.      */
char *display_name;  /* address of the X display.      */
GC gc;			/* GC (graphics context) used for drawing    */
      /*  in our window.			     */
Colormap screen_colormap;     /* color map to use for allocating colors.   */
XColor red, brown, blue, yellow, green;
      /* used for allocation of the given color    */
      /* map entries.                              */
Status rc;			/* return status of various Xlib functions.  */

/*
 * function: create_simple_window. Creates a window with a white background
 *           in the given size.
 * input:    display, size of the window (in pixels), and location of the window
 *           (in pixels).
 * output:   the window's ID.
 * notes:    window is created with a black border, 2 pixels wide.
 *           the window is automatically mapped after its creation.
 */
Window
create_simple_window(Display* display, int width, int height, int x, int y)
{
  int screen_num = DefaultScreen(display);
  int win_border_width = 2;
  Window win;

  /* create a simple window, as a direct child of the screen's */
  /* root window. Use the screen's black and white colors as   */
  /* the foreground and background colors of the window,       */
  /* respectively. Place the new window's top-left corner at   */
  /* the given 'x,y' coordinates.                              */
  win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                            x, y, width, height, win_border_width,
                            BlackPixel(display, screen_num),
                            WhitePixel(display, screen_num));

  /* make the window actually appear on the screen. */
  XMapWindow(display, win);

  return win;
}

GC
create_gc(Display* display, Window win)
{
  GC gc;				/* handle of newly created GC.  */
  unsigned long valuemask = 0;		/* which values in 'values' to  */
					/* check when creating the GC.  */
  XGCValues values;			/* initial values for the GC.   */
  unsigned int line_width = 2;		/* line width for the GC.       */
  int line_style = LineSolid;		/* style for lines drawing and  */
  int cap_style = CapButt;		/* style of the line's edje and */
  int join_style = JoinBevel;		/*  joined lines.		*/
  int screen_num = DefaultScreen(display);

  gc = XCreateGC(display, win, valuemask, &values);
  if (gc < 0) {
	fprintf(stderr, "XCreateGC: \n");
  }

  XSetBackground(display, gc, WhitePixel(display, screen_num));

  /* define the style of lines that will be drawn using this GC. */
  XSetLineAttributes(display, gc,
                     line_width, line_style, cap_style, join_style);

  /* define the fill style for the GC. to be 'solid filling'. */
  XSetFillStyle(display, gc, FillSolid);

  return gc;
}

void mainLoop() {
  XEvent e;

  XSelectInput(display, win, ExposureMask | KeyPressMask);

  while (1) {
     XNextEvent(display, &e);
     if (e.type == Expose) {

       for(int i = 0; i < 100; i++) {
         for(int j = 0; j < 100; j++) {
           XSetForeground(display, gc, blue.pixel);
           if(j % 8 < 4) {
             XSetForeground(display, gc, red.pixel);
           }
           XDrawPoint(display, win ,gc, 100 + i, 100 + j);
         }
       }
     }
     if (e.type == KeyPress)
        break;
  }
}

void
main(int argc, char* argv[])
{
  display_name = getenv("DISPLAY");  /* address of the X display.      */

  display = XOpenDisplay(display_name);

  /* get the geometry of the default screen for our display. */
  screen_num = DefaultScreen(display);

  win = create_simple_window(display, WIDTH, HEIGHT, 0, 0);

  /* allocate a new GC (graphics context) for drawing in the window. */
  gc = create_gc(display, win);
  XSync(display, False);

  /* get access to the screen's color map. */
  screen_colormap = DefaultColormap(display, DefaultScreen(display));

  /* allocate the set of colors we will want to use for the drawing. */
  XAllocNamedColor(display, screen_colormap, "red", &red, &red);
  XAllocNamedColor(display, screen_colormap, "brown", &brown, &brown);
  XAllocNamedColor(display, screen_colormap, "blue", &blue, &blue);
  XAllocNamedColor(display, screen_colormap, "yellow", &yellow, &yellow);
  XAllocNamedColor(display, screen_colormap, "green", &green, &green);

  mainLoop();

  /* close the connection to the X server. */
  XCloseDisplay(display);
}
