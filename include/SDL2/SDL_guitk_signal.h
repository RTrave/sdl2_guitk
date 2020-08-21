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

#ifndef _SDLguitk_signal_h
#define _SDLguitk_signal_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_Signal definition */
  typedef struct SDLGuiTK_Signal SDLGuiTK_Signal;


  extern DECLSPEC
  void SDLGuiTK_signal_connect( SDLGuiTK_Object *object, \
				char *type, \
				void *(*fct)( SDLGuiTK_Signal *, void * ) , \
				void *data );

  /* Retrieve SDL_KeyboardEvent values*/
  extern DECLSPEC
  SDLGuiTK_Object * SDLGuiTK_signal_get_object( SDLGuiTK_Signal *signal);

  extern DECLSPEC
  Uint8 SDLGuiTK_signal_get_state( SDLGuiTK_Signal *signal);

  extern DECLSPEC
  Uint8 SDLGuiTK_signal_get_repeat( SDLGuiTK_Signal *signal);

  extern DECLSPEC
  SDL_Keysym * SDLGuiTK_signal_get_keysym( SDLGuiTK_Signal *signal);

  /* Retrieve SDL_TextInputEvent values*/
  extern DECLSPEC
  char * SDLGuiTK_signal_get_text( SDLGuiTK_Signal *signal);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_signal_h */

