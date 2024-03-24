#define DEBUG

#include "xdriver.h"

static Display *        display;
static Window           root;
static Window           window;
static int              screen;
static int              depth;
static int              black;
static int              white;
static Font             font;
static int              ascent, descent;
static GC		copygc;
static GC		cleargc;
static int		width;
static int		height;
static Pixmap		map;

#define MAX_CHARS	256

static int		key_buf[MAX_CHARS];
static int		key_buf_in;
static int		key_buf_out;

#ifdef DEBUG
#include <stdio.h>
#endif

#define DEFAULT_TITLE      "VGA Driver"

XColor x_green, x_blue, x_red, x_white, x_black, x_darkgreen, x_orange;
XColor x_tone_red[MAX_TONE_RED], x_tone_blue[MAX_TONE_BLUE];
XColor x_tone_black[256];
Colormap colormap;

int init_driver(char *title,int win_height, int win_width) {

  int i;
  char buf[MAX_CHARS];
  static ScreenSpecs scr, *screen_specs=&scr;

  
   XEvent		event;
   int			dummy;
   XCharStruct		chars;
   unsigned long	event_mask;
   XGCValues            gcv;
   unsigned long        mask;

   screen_specs->title=title;

   

   key_buf_in = key_buf_out = 0;
   //   printf ("OLA\n");

   /* open the display */
   display 	= XOpenDisplay(NULL);
   screen 	= DefaultScreen(display);
   root		= DefaultRootWindow(display);
   depth 	= DefaultDepth(display, screen);
   black 	= BlackPixel(display, screen);
   white 	= WhitePixel(display, screen);

   screen_specs->planes = depth;

   /* get a font */
   font = XLoadFont(display, "fixed");
   XQueryTextExtents(display, font, "000000", 6, &dummy,
      &ascent, &descent, &chars);

   /* create a GC */

   mask = GCForeground | GCBackground | GCFunction |
          GCFont | GCGraphicsExposures;
   gcv.foreground         = black;
   gcv.background         = white;
   gcv.function           = GXcopy;
   gcv.font               = font;
   gcv.graphics_exposures = False;
   copygc                 = XCreateGC(display, root, mask, &gcv);


   /* context for clearing */
   gcv.foreground         = black;
   gcv.background         = white;
   gcv.function           = GXcopy;
   gcv.font               = font;
   gcv.graphics_exposures = False;
   cleargc                = XCreateGC(display, root, mask, &gcv);



   /* create window */
   {
     XSizeHints   hints;
     static char *argv[2];

     if(!screen_specs->coluns)
       screen_specs->coluns = win_width;
     if(!screen_specs->rows)
       screen_specs->rows = win_height;
     if(!screen_specs->title)
       screen_specs->title = DEFAULT_TITLE;

     argv[0] = screen_specs->title;
     argv[1] = 0;

     width =  screen_specs->coluns;
     height = screen_specs->rows;

     window = XCreateSimpleWindow(display, root, 0, 0, width, height,
				  1, black, black);


     hints.flags = PSize | PMinSize | PMaxSize;
     hints.width = hints.min_width = hints.max_width = width;
     hints.height = hints.min_height = hints.max_height = height;

     XSetStandardProperties(display, window, screen_specs->title,
			    screen_specs->title, 0, argv, 1, &hints);

     map  = XCreatePixmap(display, root, width, height, depth);
     XFillRectangle(display, map, cleargc, 0, 0,
		 width, height);
   }

   /* select the event mask for the window */
   event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;;
   XSelectInput(display, window, event_mask);
   

   /* wait for window to appear */
   XMapWindow(display, window);

   while (True) {
      XNextEvent(display, &event);
      if (event.xany.window != window) continue;
      if (event.type == Expose) break;
   }



   /* Init Colors */

   colormap = DefaultColormap(display, 0);

   XParseColor(display, colormap, "#00FF00", &x_green);
   XAllocColor(display, colormap, &x_green);

   XParseColor(display, colormap, "#FF0000", &x_red);
   XAllocColor(display, colormap, &x_red);

   XParseColor(display, colormap, "#0000FF", &x_blue);
   XAllocColor(display, colormap, &x_blue);

   XParseColor(display, colormap, "#000000", &x_black);
   XAllocColor(display, colormap, &x_black);

   XParseColor(display, colormap, "#FFFFFF", &x_white);
   XAllocColor(display, colormap, &x_white);

   XParseColor(display, colormap, "#339900", &x_darkgreen);
   XAllocColor(display, colormap, &x_darkgreen);

   XParseColor(display, colormap, "#ffcc00", &x_orange);
   XAllocColor(display, colormap, &x_orange);
   
   for (i=0;i<256;i++) {
     sprintf(buf,"#%02X0000",i); 
     /* sprintf(buf,"#%02X%02X%02X",i,i,i); -> tons de cinzento */
     XParseColor(display, colormap, buf, &x_tone_red[i]);
     XAllocColor(display, colormap, &x_tone_red[i]);
   }


   for (i=0;i<256;i++) {
     sprintf(buf,"#%02X%02X%02X",i,i,i);
     XParseColor(display, colormap, buf, &x_tone_black[i]);
     XAllocColor(display, colormap, &x_tone_black[i]);
   }

   for (i=256;i<512;i++) {
     sprintf(buf,"#FF%02X00",i-256);
     XParseColor(display, colormap, buf, &x_tone_red[i]);
     XAllocColor(display, colormap, &x_tone_red[i]);
   }
   for (i=512;i<768;i++) {
     sprintf(buf,"#FFFF%02X",i-512);
     XParseColor(display, colormap, buf, &x_tone_red[i]);
     XAllocColor(display, colormap, &x_tone_red[i]);
   }

   for (i=0;i<256;i++) {
     sprintf(buf,"#0000%02X",i);
     XParseColor(display, colormap, buf, &x_tone_blue[i]);
     XAllocColor(display, colormap, &x_tone_blue[i]);
   }

   return 1;
}


static void
check_events()
{
  XEvent event;
  char c_buf;
  KeySym last_key;
  XComposeStatus  status;
  
  XSync(display, False);
  
  while (QLength(display) > 0) {
    XNextEvent(display, &event); 
    if (event.xany.window != window) continue;
    switch (event.type) {
    case KeyPress:
      XLookupString((XKeyEvent *)&event, &c_buf, 1, &last_key, &status);
      key_buf[key_buf_in++] = c_buf;
      if(key_buf_in>=MAX_CHARS) key_buf_in = 0;
      if(key_buf_out==key_buf_in) 
	if(++key_buf_out>MAX_CHARS) key_buf_out = 0;
      break;
    case UnmapNotify:
      while (True) {
	XNextEvent(display, &event);
	if (event.xany.window != window) continue;
	if (event.type == MapNotify) break;
      }
      XCopyArea(display, map, window, copygc, 0, 0,
		width, height, 0, 0);
      break;
    case Expose:
      XCopyArea(display, map, window, copygc, 0, 0,
		width, height, 0, 0);
      break;
    default: break;
    }
  }
}


int
get_key(int *key, int *mods)
{
  check_events();
  if(key_buf_in!=key_buf_out) {
    *key = key_buf[key_buf_out++];
    if(key_buf_out>MAX_CHARS) key_buf_out=0;
    *mods = 0;
    return 1;
  }
  return 0;
}

void flush_keys() {
  key_buf_in = key_buf_out = 0;
}

int refresh_screen() {
  XCopyArea(display, map, window, copygc, 0, 0,
	    width, height, 0, 0);
  check_events();
  return 1;
}

int clear_screen() {
  // XSetForeground(display, copygc, x_tone_black[255].pixel);
  //XFillRectangle(display, map, cleargc, 0, 0, width, height);
draw_rectangle( x_tone_black[255],x_tone_black[255] , 0, 0, width,height);
  return 1;
} 



int draw_line(XColor c, int x0, int y0, int x1, int y1) {
  XSetForeground(display, copygc, c.pixel);

  XDrawLine(display, map, copygc, x0, y0,
	    x1, y1);
  return 1;
}

int draw_point(XColor c, int x0, int y0) {
  int i, j,  lim=2;

  XSetForeground(display, copygc, c.pixel);
  
  for (i=0; i<lim; i++)
    for (j=0; j<lim; j++)
      XDrawPoint(display, map, copygc, x0+i,   y0+j);

  return 1;
}

int draw_circle(XColor fg, XColor bg, int x0, int y0, int radius) {  
  XSetForeground(display, copygc, bg.pixel);
  XFillArc(display, map, copygc,
	   x0-radius, y0-radius,
	   radius*2, radius*2, 0, 23040);
    
  XSetForeground(display, copygc, fg.pixel);

  XDrawArc(display, map, copygc,
	   x0-radius, y0-radius,
	   radius*2, radius*2, 0, 23040);
  return 1;
}


int draw_arc(XColor fg, int x0, int y0, int radius, int angle_start, int angle_end) {
  XSetForeground(display, copygc, fg.pixel);

  XDrawArc(display, map, copygc,
	   x0-radius, y0-radius,
	   radius*2, radius*2, angle_start*64, (angle_end-angle_start)*64);
  return 1;
}

int draw_rectangle(XColor fg, XColor bg, int x, int y, int width, int height) {

  XSetForeground(display, copygc, bg.pixel);
  XFillRectangle(display, map, copygc, x, y, width, height);

  XSetForeground(display, copygc, fg.pixel);
  XDrawRectangle(display, map, copygc, x, y, width, height);
  return 1;
}

int draw_text(XColor fg, int x, int y, char *string) {
  XSetForeground(display, copygc, fg.pixel);

  XDrawString(display, map, copygc, x, y, string, strlen(string));
  
  return 1;
}


int draw_pixel(XColor c, int x, int y) {

  XSetForeground(display, copygc, c.pixel);
  XDrawPoint(display, map, copygc, x, y);

  return 1;
}


typedef struct element
{
  int xp,yq,dx,dy,E;
  struct element *next;
}*eltpr,element;

void drawline(int x0, int y0, int x1, int y1)
{
  draw_line(x_tone_black[0],x0,y0,x1, y1);
}

void drawcircle(int x0, int y0, int radius)
{
  draw_circle(x_tone_black[0],x_tone_black[255],x0, y0,radius);
 
}

void drawpoint(int x0, int y0)
{
  draw_pixel(x_tone_black[0],x0,y0);
  
}

/*-------------------------linhas---------------------*/
int  drawLine(Point p0,Point p1)    
{
  line (p0.x,p0.y,p1.x,p1.y);
}


void line (int xp,int yp,int xq,int yq)
{
  
        int  x=xp, y=yp, d=0, dx=xq-xp,dy=yq-yp,c , m,
	xinc=1, yinc=1;
	if(dx<0){xinc=-1;dx=-dx;}
	if(dy<0){yinc=-1;dy=-dy;}
	if(dy<dx)
	{
		c=2*dx;m=2*dy;
		while (x!=xq)
		{
			drawpoint(x,y);
			x+=xinc;d+=m;
			if (d>dx){y+=yinc;d-=c;}
		}
	}
	else
	{
		c=2*dy;m=2*dx;
		while (y!=yq)
		{
			drawpoint(x,y);
			y+=yinc;d+=m;
			if (d>dy){x+=xinc;d-=c;}
		}
	}
}

/*-------------------------Poligno convexo-------------------------*/

int drawFilledPoligon (Point v[],int n)
{
  int *xpoly=(int*)calloc(n,sizeof(int));
  int *ypoly=(int*)calloc(n,sizeof(int));
  int i=0;
  
  for(i=0;i<n;i++)
    {
      xpoly[i]=v[i].x;
      ypoly[i]=v[i].y;
    }
  drawfilledpoligon (xpoly,ypoly, n);
  free(xpoly);
  free(ypoly);
}


static void checkmem(void *p)
{
	if (p==NULL) {printf ("n-bão há mais memória!!!");exit (1);}
}

void horlinemem (int x1,int x2,int y)
{
  draw_line (x_tone_black[0],x1,y,x2+1,y);  // chama função de sis
  /*	int i,aux1=x1,aux2=x2;
	int mx=ecra.maxx,my=ecra.maxy;

	if (aux1>aux2) {i=aux1; aux1=aux2;aux2=i;}
        if (aux2>mx) aux2=mx;
	if (aux1<0) aux1=0;
        if (aux1<=mx && y<=my && y>=0)
	setmem(ecra.mem[my-y]+aux1, aux2-aux1+2,valor);*/ //<<--fast
}



void drawfilledpoligon (int *X,int *Y,int n)
{
  int x,y,i,ymin=1000,ymax=0, j, ny, i1,xp,yp,xq,yq,temp, dx,dy,m,dyq,E,xleft,xright;
  eltpr *table, p, start, end, p0,q;
  x=X[n-1];y=Y[n-1];
  for(i=0;i<n;i++)
	{
	  //draw_line (x_tone_black[0],x,y,X[i],Y[i]);
	  //  line(x,y,X[i],Y[i]);
		x=X[i];y=Y[i];
		if (y<ymin) ymin=y;
		if (y>ymax) ymax=y;
	}
	ny=ymax-ymin+1;
	table=(eltpr*)calloc(ny,sizeof(eltpr));
	checkmem(table);
	for(j=0;j<ny;j++) table[j]=NULL;
	for (i=0; i<n ;i++)
	{
		i1=i+1;
		if (i1==n)i1=0;
		xp=X[i];yp=Y[i];
		xq=X[i1];yq=Y[i1];
		if (yp==yq) 
			continue;
		if (yq<yp)
		{
			temp=xp;xp=xq;xq=temp;
			temp=yp;yp=yq;yq=temp;
		}
	
		p=(element*)malloc(sizeof(element));
		checkmem(p);
		p->xp=xp;p->dx=xq-xp;
		p->yq=yq;p->dy=yq-yp;//VER SE ESTA CORRECTO yq
		j=yp-ymin;
		p->next = table[j];table[j]=p;
	}
	
	
	start=end=(element*)malloc(sizeof(element));
	checkmem (start);
	for (j=0;j<ny;j++)
	{
		y=ymin+j;
		p=start;
		while (p!=end)
		{
			if(p->yq==y)
			{
				if ((q=p->next)==end) end=p;else *p =*q;
	
				free(q);
			}
			else
			{
				if ((dx=p->dx)!=0)
				{
					x=p->xp;
					dy=p->dy;
					E=p->E;
					m=dx/dy;
					dyq=2*dy;
					x+=m;E+=2*dx-m*dyq;
					if (E>dy || E< -dy)
					{
						if (dx>0)
						{
							x++;E-=dyq;
						}
						else
						{
							x--;E+=dyq;
						}
					}
					p->xp=x;
					p->E=E;
				}
				p=p->next;
			}
		}
		p=table[j];
		while (p!=NULL)
		{
			x=end->xp=p->xp;yq=p->yq;
			dx=p->dx;dy=p->dy;q=start;
			while (q->xp<x || q->xp==x && 
				q!=end && (long)q->dx*dy<(long)dx*q->dy)
				q=q->next;
			p0=p;p=p->next;
			if (q==end)end=p0;else *p0=*q;
			q->xp=x;q->yq=yq;
			q->dx=dx;q->dy=dy;
			q->E=0;q->next=p0;
		}
		for (p=start;p!=end;p=p->next)
		{
			xleft=p->xp+1;p=p->next;
			xright=p->xp-1;
			if (xleft<=xright)
			{
				horlinemem(xleft,xright,y);
			}
		}
	}
	p=start;
	while (p!=end)
	{
		p0=p;p=p->next;
	
		free(p0);
	}
	
	free(start);
	//delete[ny] table;
	
}

/*------------------------Circulos-------------------*/
drawCircle(Point p0,int r)
{
  circle(p0.x,p0.y,r);
  return 0;
}

void circle (int xc,int yc,int r)
{
	int x=0,y=r, u=1, v=2*r-1, E=0;
	drawpoint(xc, yc + r);
	drawpoint(xc, yc - r);
	drawpoint(xc + r, yc);
	drawpoint(xc - r, yc);
	while (x<y)
	{
		x++; E+=u; u+=2;
		if (v<2*E) {y--;E-=v;v-=2;}
		if (x<=y)
		{
			drawpoint(xc+x, yc +y);
			drawpoint(xc-x, yc +y);
			drawpoint(xc+x, yc -y);
			drawpoint(xc-x, yc -y);
			if (x<y)
			{
				drawpoint(xc+y, yc + x);
				drawpoint(xc-y, yc + x);
				drawpoint(xc+y, yc - x);
				drawpoint(xc-y, yc - x);
			}
		}
	}
}

int drawRectangle(Point p0,Point p1)
{
  drawline(p0.x,p0.y,p1.x,p0.y);
  drawline(p0.x,p0.y,p0.x,p1.y);
  drawline(p1.x,p1.y,p1.x,p0.y);
  drawline(p1.x,p1.y,p0.x,p1.y);

  return 0;
}

/*-----------------------------clipline-----------------------*/


static float xmin,xmax,ymin,ymax;

void setclipboundaries(float x1,float x2,float y1,float y2)
{
  float aux;
  xmin=x1;xmax=x2;
  ymin=y1;ymax=y2;
  if (xmin>xmax) {aux=xmin;xmin=xmax;aux=xmax;}
  if (ymin>ymax) {aux=ymin;ymin=ymax;ymax=aux;}
}

static int code(float x, float y)
{
  return ((x<xmin)<<3) | ((x>xmax)<<2) | ((y<ymin)<<1) | (y>ymax);
}

void clipdraw(float xp,float yp, float xq,float yq)
{
  int cp=code(xp,yp),cq=code(xq,yq),i=0;
  float dx,dy;
  //  if ( (xp>xmax)&&
  while (cp | cq)
    {
      //  printf("estou aqui\n");
        if (cp&cq) return;//ver isto
	dx=xq-xp;
      dy=yq-yp;
      if(cp)
	{
	  if(cp&8) {yp+=(xmin-xp)*dy/dx;xp=xmin;}else 
	  if(cp&4) {yp+=(xmax-xp)*dy/dx;xp=xmax;}else 
	  if(cp&2) {xp+=(ymin-yp)*dx/dy;yp=ymin;}else 
	  if(cp&1) {xp+=(ymax-yp)*dx/dy;yp=ymax;}
	  cp=code(xp,yp);
	}
      else
	{
	  if(cq&8) {yq+=(xmin-xq)*dy/dx;xq=xmin;}else 
	  if(cq&4) {yq+=(xmax-xq)*dy/dx;xq=xmax;}else 
	  if(cq&2) {xq+=(ymin-yq)*dx/dy;yq=ymin;}else 
	  if(cq&1) {xq+=(ymax-yq)*dx/dy;yq=ymax;}
	  cq=code(xq,yq);
	}
      //  if (++i>9) break;
    }
    drawline((int)xp,(int)yp,(int)xq,(int)yq);
} 

int clipLine(Point p0,Point p1,Point q0,Point q1)
{
  setclipboundaries(p0.x,p1.x,p0.y,p1.y);
  clipdraw(q0.x,q0.y,q1.x,q1.y);
  return 0; 
}


/*--------------------- smoothline---------------------------*/


void intensifyPixel(int x,int y,float d)
{
  int color=(int) (fabs(d)*255.);
  if (color>255) color=255;
  draw_pixel(x_tone_black[color],x,y);  
}


void sline(int x1,int y1,int x2,int y2)
{
  int dx,dy,incrE,incrNE,d,x,y,two_v_dx;
  float invDenom,two_dx_invDenom;
  
  dx=x2-x1;
  dy=y2-y1;
  d=2*(dy-dx);   //ver isto
  incrE=2+dy;
  incrNE=2*(dy-dx);
  two_v_dx=0;
  invDenom=1/(3*sqrt((dx*dx)+(dy*dy)));
  two_dx_invDenom=2*dx*invDenom;
  x=x1;
  y=y1;
  intensifyPixel(x,y,0.);
  intensifyPixel(x2,y2,0.);
  intensifyPixel(x,y+1,two_dx_invDenom);
  intensifyPixel(x,y-1,two_dx_invDenom);
  while(x<x2)
    {
      if(d<0)
	{
	  two_v_dx=d+dx;
	  d=d+incrE;
	  x++;
	}
      else
	{
	  two_v_dx=d-dx;
	  d=d+incrNE;
	  x++;
	  y++;
	}
      

      intensifyPixel(x,y+1,two_dx_invDenom-(two_v_dx*invDenom));
      intensifyPixel(x,y-1,two_dx_invDenom+(two_v_dx*invDenom) );
      intensifyPixel(x,y,two_v_dx*invDenom);      
    }
  
}


int drawSmoothLine(Point p0,Point p1)
{
  sline(p0.x,p0.y,p1.x,p1.y);
  return 0;
}
