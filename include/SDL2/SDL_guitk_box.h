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

#ifndef _SDLguitk_box_h
#define _SDLguitk_box_h

#include <SDL2/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_Box structure definition */
  /* Widget grouping boxes definition */
  typedef struct SDLGuiTK_Box SDLGuiTK_Box;

  extern DECLSPEC
  SDLGuiTK_Box *SDLGuiTK_BOX( SDLGuiTK_Widget *widget );


  /* Define the type of boxes */
#define SDLGUITK_BOX_H ((int) 0)   /* horizontal */
#define SDLGUITK_BOX_V ((int) 1)   /* vertical */



  /*  */
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_hbox_new();
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_vbox_new();

  extern DECLSPEC
  void SDLGuiTK_box_pack_start( SDLGuiTK_Box *box, \
				SDLGuiTK_Widget *widget, \
				SDL_bool expand,
				SDL_bool fill,
				int padding );


  extern DECLSPEC
  void SDLGuiTK_box_set_spacing( SDLGuiTK_Box *box, int spacing );
  extern DECLSPEC
  int  SDLGuiTK_box_get_spacing( SDLGuiTK_Box *box );

  extern DECLSPEC
  void SDLGuiTK_box_set_homogeneous( SDLGuiTK_Box *box, SDL_bool homogeneous);

  extern DECLSPEC
  SDL_bool SDLGuiTK_box_get_homogeneous( SDLGuiTK_Box *box );

  extern DECLSPEC
  void SDLGuiTK_box_set_orientation( SDLGuiTK_Box *box, int orientation);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_box_h */


