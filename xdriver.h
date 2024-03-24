#ifndef _XDRIVER_
#define _XDRIVER_

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <math.h> //sqrt... Sline

typedef struct point
{
  int x,y;
}Point;


#define MAX_TONE_RED 768
#define MAX_TONE_BLUE 256

typedef struct {
  int coluns;
  int rows;
  int planes;
  char *title;
} ScreenSpecs;
/*------------Estas funcoes chamam as minhas --------------------*/
int drawLine(Point p0,Point p1);
int drawCircle(Point p0,int r);
int drawFilledPoligon (Point v[],int n);
int clipLine(Point p0,Point p1,Point q0,Point q1);
int drawSmoothLine(Point p0,Point p1);

int drawRectangle(Point p0,Point p1); // transparente ...


/*-------------Sistema  com cor-------------------------------------*/
int init_driver(char *, int, int);

void get_mouse(int *x, int *y, int *buttons);

int rgb_pixel(int R, int G, int B);

int refresh_screen();

int clear_screen();

int get_key(int *key, int *mods);

int draw_pixel(XColor c, int x, int y);

int draw_line(XColor c, int x0, int y0, int x1, int y1);

int draw_point(XColor c, int x0, int y0);

int draw_circle(XColor fg, XColor bg, int x0, int y0, int radius);

int draw_arc(XColor fg, int x0, int y0, int radius, int angle_start, int angle_end);

int draw_rectangle(XColor fg, XColor bg, int x1, int y1, int x2, int y2);

int draw_text(XColor fg, int x, int y, char *string);

/*-------------------sistema sem cor----------------------*/

void drawline(int x0, int y0, int x1, int y1);

void drawcircle(int x0, int y0, int radius);

void drawpoint(int x0, int y0);

/*--------------minhas sem cor------------------*/
void line (int xp,int yp,int xq,int yq);

void drawfilledpoligon (int *X,int *Y,int n);

void circle (int xc,int yc,int r);


#endif
