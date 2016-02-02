#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef WIN32
	#include "windows.h"
#else
	#ifndef X11
	#define X11
	#endif
#endif

#ifdef X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif

/* Graphics.h
* Originally written by Vaughn Betz (vaughn@eecg.utoronto.ca)
* Win32 port by Paul Leventis (leventi@eecg.utoronto.ca)
* Enhanced version by William Chow (chow@eecg.utoronto.ca)
* Minor updates by Guy Lemieux (lemieux@ece.ubc.ca)
*/

#define SCREEN 0 
#define POSTSCRIPT 1 

#define NUM_COLOR 11

enum color_types {WHITE, BLACK, DARKGREY, LIGHTGREY, BLUE, GREEN, YELLOW,
CYAN, RED, DARKGREEN, MAGENTA};

enum line_types {SOLID, DASHED};

#define MAXPTS 100    /* Maximum number of points drawable by fillpoly */

typedef struct {
	float x; 
	float y;
} t_point; /* Used in calls to fillpoly */

typedef struct {
#ifdef X11
	Window mainwnd; 
#else
	HWND mainwnd;
#endif
	float xmult, ymult;
	float ps_xmult, ps_ymult;
	float xleft, xright, ytop, ybot;
	int top_width, top_height;
} t_report;

/************** ESSENTIAL FUNCTIONS ******************/

/* This is the main routine for the graphics.  When event_loop is
* called, it will continue executing until the Proceed button is
* pressed.  Meanwhile, any buttons pressed will invoke their
* event handlers, and any left-mouse button click on the graphics
* area will be passed to the act_on_button routine. When, GetMouse(1) is called,
* and the mouse moves, act_on_mousemove routine will be called. To disable,
* call GetMouse(0). Whenever the graphics needs to be redrawn, drawscreen will be called. */
void event_loop (void (*act_on_button) (float x, float y, int flags),
			void (*act_on_mousemove) (float x, float y),
			void (*act_on_keypress) (int i),
			void (*drawscreen) (void));  

				 /* Opens up the graphics; the window will have window_name in its
* title bar. */

/* cindex defines the background color, cleanup is the function to call when shutting down */
void init_graphics (char *window_name, int cindex, void (*cleanup)(void));

/* Sets world coordinates of the graphics window so that the 
* upper-left corner has virtual coordinate (xl, yt) and the
* bottom-right corner has virtual coordinate (xr, yb). */
void init_world (float xl, float yt, float xr, float yb); 

/* Closes the graphics */
void close_graphics (void);

/* Changes the status message to msg. */
void update_message (char *msg);

/* Creates a button on the menu bar after the button with text
* prev_button_text.  The button will have text button_text,
* and when clicked will call function button_func.  
* button_func is a function that accepts a void function as
* an argument; this argument is set to the drawscreen routine
* as passed into the event loop. */
void create_button (char *prev_button_text , char *button_text, 
					void (*button_func) (void (*drawscreen) (void))); 

/* Destroys the button with the given text */
void destroy_button (char *button_text);

/*************** PostScript Routines *****************/

/* Opens file for postscript commands and initializes it.  All subsequent  *
* drawing commands go to this file until close_postscript is called.      */
int init_postscript (char *fname);   /* Returns 1 if successful */

/* Closes file and directs output to screen again.       */

void close_postscript (void);      


/*************** DRAWING ROUTINES ******************/

/* The following routines draw to SCREEN if disp_type = SCREEN *
* and to a PostScript file if disp_type = POSTSCRIPT          */

/* Set the current draw colour to the supplied colour index from color_types */
void setcolor (int cindex);

/* Sets the line style to the specified line_style */
void setlinestyle (int linestyle); /* dotted style only works if linewidth = 1, else the line will be drawn
								      using solid style */

/* Sets the line width in pixels */
void setlinewidth (int linewidth);

/* Sets the font size */
void setfontsize (int pointsize);

/* Draws a line from (x1, y1) to (x2, y2) in world coordinates */
void drawline (float x1, float y1, float x2, float y2);

/* Draws a rectangle from (x1, y1) to (x2, y2) in world coordinates, using
* the current line style, colour and width. */
void drawrect (float x1, float y1, float x2, float y2);

/* Draws a filled rectangle */
void fillrect (float x1, float y1, float x2, float y2);

/* Draws a filled polygon (may not work under Win32) */
void fillpoly (t_point *points, int npoints); 

/* Draw or fill a circular arc, respectively.  Angles in degrees.  startang  *
* measured from positive x-axis of Window.  Positive angextent means        *
* counterclockwise arc.                                                     */
void drawarc (float xcen, float ycen, float rad, float startang,
			  float angextent); 
void fillarc (float xcen, float ycen, float rad, float startang,
			  float angextent);
void drawellipticarc (float xc, float yc, float radx, float rady, float startang, float angextent);
void fillellipticarc (float xc, float yc, float radx, float rady, float startang, float angextent);

			  /* boundx specifies horizontal bounding box.  If text won't fit in    *
* the space specified by boundx (world coordinates) text isn't drawn */
void drawtext (float xc, float yc, char *text, float boundx);

/* Clears the screen */
void clearscreen (void);

/* redraw the screen */
void invalidate_screen(void);

/* Ameer Abdelhadi: moved from graphics.c	*/
void postscript (void (*drawscreen) (void));

/*************** ADVANCED FUNCTIONS *****************/

/* Normal users shouldn't have to use draw_message.  Should only be *
* useful if using non-interactive graphics and you want to redraw  *
* yourself because of an expose.                                   */
void draw_message (void);     

/* Empties event queue */
void flushinput (void);

/* added by William to provide double buffering in Windows */
void report_structure(t_report*);
void get_mouse(int);
void get_key(int);
void set_buttonmask(int);
void draw_xor();
void draw_normal();
void update_window (float x1, float y1, float x2, float y2, void (*drawscreen)(void));
#ifdef WIN32
void drawtobuffer(void);
void drawtoscreen(void);
void displaybuffer(void);
void setcolor_by_colorref (COLORREF);
void settooltiptext(char *text);
void setuptooltips(int waittime, int lasttime);
void enabletooltips();
void disabletooltips();
void change_button_text(char *button_text, char *new_button_text);
void drawcurve(t_point *points, int npoints);
void fillcurve(t_point *points, int npoints);
void object_start(int all);
void object_end();
int pt_on_object(int all, float x, float y);
int findfontsize(float ymax);

// for detecting control/shift buttons while pressing mouse button
#define MOUSECLK_CONTROL MK_CONTROL
#define MOUSECLK_SHIFT MK_SHIFT

#endif

#endif
