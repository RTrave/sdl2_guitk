/* 
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

   Copyright (C) 2003 Trave Roman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/

#ifndef _SDLguitk_window_h
#define _SDLguitk_window_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

  /* SDLGuiTK_Window structure definition */
  typedef struct SDLGuiTK_Window SDLGuiTK_Window;

  extern DECLSPEC
  SDL_bool SDLGuiTK_IS_WINDOW( SDLGuiTK_Widget * widget );
  extern DECLSPEC
  SDLGuiTK_Window *SDLGuiTK_WINDOW( SDLGuiTK_Widget *widget );



  /* ops. on SDLGuiTK_Window objects */
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_window_new();

  /* Set the title on tile */
  extern DECLSPEC
  void SDLGuiTK_window_set_title( SDLGuiTK_Widget * window, \
				  const char *title );

  /* Position of window on main surface */
#define SDLGUITK_WINDOW_CENTER      ((int) 1)
#define SDLGUITK_WINDOW_MOUSE       ((int) 2)
#define SDLGUITK_WINDOW_MOUSEALWAYS ((int) 3)

  extern DECLSPEC
  void SDLGuiTK_window_set_position( SDLGuiTK_Widget * window,\
				     int position, \
				     void *data );

  extern DECLSPEC
  void SDLGuiTK_window_add( SDLGuiTK_Widget * window, \
			    SDLGuiTK_Widget * widget);



/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_window_h */


