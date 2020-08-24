/* ////////////////////////////////////////////////////////////////////////////
//                                                                          //
// XPMBIFF			                                           //
//                                                                        //
// xpmbiff is a VERY simple mail notification program. It can display    //
// a colored and shaped window which shows the status of the spoolfile. ///////
// It shows an empty icon if it is empty, otherwise a full icon.            // 
// When you click on the icon it will start a program of your choice,      //
// probably a mail reader. When the spoolfile increase in size it will    //
// execute another command of your choice, for example:                  // 
// cat mail.au > /dev/audio                                             ///////
//                                                                          // 
// This is the first version, try it and use it if you like. Mail me any   //
// comments and suggestions. It is made for being swallowed by GoodStuff  //
// that is why it sets its position to 2000,2000. The pixmaps have to be //
// the same size.                                                       //
//                                                                     //
// Fredrik Öhrström d92-foh@nada.kth.se                               //
//                                                                   //
// FILE: xpmbiff.c                                                  //
// DATE: 9 December 1995                                           //
//  VER: 1.0                                                      //
//                                                               //
//////////////////////////////////////////////////////////////////

/* 
 * Copyright (C) 1995 Fredrik Öhrström (d92-foh@nada.kth.se)
 *
 * xpmbiff.c version 1.0 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* BUGS that I know of:

   Only for GoodStuff for the moment. Very easy to fix, just add geometry.
   It will not announce new mails with moremail_file during the first check
   after a delete of mails. Tell me how to solve this without looking into
   the file please! :)
   You have to set the volume by some other program.

*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/extensions/shape.h>
#include <X11/xpm.h>

#include <stdio.h>
#include <stdlib.h>

/* GRAPHIC VARIABLES ////////////////////////////////////////////////////////*/

  Display *display;
  Window window;
  int screen;
  XEvent event;
  Pixmap full_pixmap,empty_pixmap;
  Pixmap full_mask,empty_mask;
  XpmAttributes full_attributes,empty_attributes;
  XSizeHints size_hints;

/* OTHER VARIABLES //////////////////////////////////////////////////////////*/

  int error, spool_size, old_spool_size, counter, timer, pressed, wait;
  FILE *spool;
  char *empty_file, *full_file, spool_file[256], 
       *click_file, *moremail_file;
       
/* MAIN LOOP ////////////////////////////////////////////////////////////////*/

int main (int argc, char **argv)
{
  if (argc<3)
  {
    puts ("xpmbiff version 1.0 by Fredrik Ohrstrom 1995");
    puts ("usage:");
    puts ("xpmbiff empty full [time [click [moremail]]]");
    puts ("");
    puts ("empty is the filename to the empty pixmap.");
    puts ("full         - '' -          full  pixmap.");
    puts ("time is in seconds, how often to check for new mail.");
    puts ("click is the commandline to execute on a click.");
    puts ("moremail is the commandline to execute when new mail arrive.");
    puts ("");
    puts ("Example:");
    puts ("xpmbiff /gfx/full.xpm /gfx/empty.xpm 5 'xterm -e pine &' "
	  "'cat /home/snd/newmail.au > /dev/audio &'");
    exit (0);
  }

  empty_file = argv[1];
  full_file = argv[2];

  timer = 60;			  /* Standard check once every minute. */
  if (argc>=4)
  {
    timer = atoi(argv[3]);
    if (timer==0) timer = 60;	  /* Probably erroneous input. */
    if (timer<5)  timer = 5;	  /* Not less than 2 seconds.  */
  }

  click_file = moremail_file = 0; /* Standard nothing happens. */
  if (argc>=5) click_file = argv[4]; 
  if (argc>=6) moremail_file = argv[5]; 
    
  display = XOpenDisplay (getenv("DISPLAY"));
  if (!display)
  {
    puts ("Xpmbiff cannot connect to X server.");
    exit (0);
  }

  screen = DefaultScreen (display);

  window = XCreateSimpleWindow (display,
				RootWindow(display, screen),
				2000, 2000, 
				100, /* Pick any size.         */
				100, /* Will be resized later. */
				0,
				BlackPixel(display, screen),
				WhitePixel(display, screen));

  XSelectInput (display, window, ButtonPressMask);

  error = XpmReadFileToPixmap (display, window,
		 empty_file,
		 &empty_pixmap, &empty_mask,
		 &empty_attributes);

  error = XpmReadFileToPixmap (display, window,
		 full_file,
                 &full_pixmap, &full_mask,
		 &full_attributes);

  size_hints.flags      = PMinSize | PMaxSize | PPosition;
  size_hints.x          = 2000;
  size_hints.y          = 2000;
  size_hints.min_height = empty_attributes.height;
  size_hints.min_width  = empty_attributes.width;
  size_hints.max_height = empty_attributes.height;
  size_hints.max_width  = empty_attributes.width;

  XSetWMNormalHints (display, window, &size_hints);

  XStoreName (display, window, "xpmbiff");
  XSetIconName(display, window, "xpmbiff");

  XSetWindowBackgroundPixmap (display, window, empty_pixmap);
  XShapeCombineMask (display,window,ShapeBounding, 0, 0, empty_mask, ShapeSet);
  XMapWindow (display, window);
  XClearWindow (display, window);
  
  XFlush (display);

  old_spool_size = 0;

  strcpy (spool_file, "/etc/spool/mail/");
  strcat (spool_file, getenv("USER"));

  counter = timer;
  wait = 0;

  for (;;)
  {
    sleep (1);
    if (++counter >= timer) 
    {
      counter = 0;
      spool = fopen (spool_file,"r");
      if (spool)
      {
	fseek (spool, 0, 2);	     /* Go to the end. */
	spool_size = ftell (spool);
	fclose (spool);
      }

      if (spool_size > 0)
      {
	XSetWindowBackgroundPixmap (display, window, full_pixmap);
	XShapeCombineMask(display,window,ShapeBounding,0,0,
			  full_mask,ShapeSet);
	XClearWindow (display, window);
	XFlush (display);
	if (spool_size > old_spool_size && !wait && moremail_file) 
	{
	  system (moremail_file);
	  wait = 2;
	}
      }
      if (spool_size == 0)
      {
	XSetWindowBackgroundPixmap (display, window, empty_pixmap);
	XShapeCombineMask(display,window,ShapeBounding,0,0,
			  empty_mask,ShapeSet);
	XClearWindow (display, window);
	XFlush (display);
      }
      old_spool_size = spool_size;
      if (wait) wait--;
    }
    pressed = 0;
    while (XPending(display))
    {
      XNextEvent (display, &event);
      if (event.type == ButtonPress && !pressed) 
      {
	system (click_file);
	pressed = 1;
      }
    }    
  }

  /* Actually it never gets here. */

  XFreePixmap (display,empty_pixmap);
  XFreePixmap (display,full_pixmap);
  XFreePixmap (display,empty_mask);
  XFreePixmap (display,full_mask);
  XCloseDisplay (display);
}












