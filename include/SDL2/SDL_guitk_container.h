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

#ifndef _SDLguitk_container_h
#define _SDLguitk_container_h

#include <SDL2/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_Alignment structure definition */
  /*  */
  typedef struct SDLGuiTK_Container SDLGuiTK_Container;

  extern DECLSPEC
  SDL_bool SDLGuiTK_IS_CONTAINER( SDLGuiTK_Widget * widget );
  extern DECLSPEC
  SDLGuiTK_Container *SDLGuiTK_CONTAINER( SDLGuiTK_Widget *widget );



  /*  */
  extern DECLSPEC
  void SDLGuiTK_container_add( SDLGuiTK_Container *container,\
			       SDLGuiTK_Widget *widget );

  extern DECLSPEC
  void SDLGuiTK_container_remove( SDLGuiTK_Container *container,\
				  SDLGuiTK_Widget *widget );

  extern DECLSPEC
  void SDLGuiTK_container_set_border_width( SDLGuiTK_Container *container,\
					    int border_width );

  extern DECLSPEC
  SDLGuiTK_List *SDLGuiTK_container_get_children( SDLGuiTK_Container *container );


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_container_h */


