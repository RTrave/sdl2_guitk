/* 
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

   Copyright (C) 2007 Trave Roman

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

#ifndef _SDLguitk_panel_h
#define _SDLguitk_panel_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

  /* SDLGuiTK_Panel structure definition */
  typedef struct SDLGuiTK_Panel SDLGuiTK_Panel;

  extern DECLSPEC
  SDL_bool SDLGuiTK_IS_PANEL( SDLGuiTK_Widget * widget );
  extern DECLSPEC
  SDLGuiTK_Panel *SDLGuiTK_PANEL( SDLGuiTK_Widget *widget );



  /* ops. on SDLGuiTK_Panel objects */
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_panel_new();


  /* Position of panel on main surface */
#define SDLGUITK_PANEL_TOP          ((int) 1)
#define SDLGUITK_PANEL_BOTTOM       ((int) 2)
#define SDLGUITK_PANEL_LEFT         ((int) 3)
#define SDLGUITK_PANEL_RIGHT        ((int) 4)

  extern DECLSPEC
  void SDLGuiTK_panel_set_position( SDLGuiTK_Widget * panel,\
				    int position,	     \
				    void *data );

  extern DECLSPEC
  void SDLGuiTK_panel_add( SDLGuiTK_Widget * panel, \
			   SDLGuiTK_Widget * widget);



/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_panel_h */


