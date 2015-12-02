/* 
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

   Copyright (C) 2005 Trave Roman

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

#ifndef _SDLguitk_editable_h
#define _SDLguitk_editable_h

#include <SDL2/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_Editable structure definition */
  typedef struct SDLGuiTK_Editable SDLGuiTK_Editable;

  extern DECLSPEC 
  SDLGuiTK_Editable *SDLGuiTK_EDITABLE( SDLGuiTK_Widget *widget );

  extern DECLSPEC
  void SDLGuiTK_editable_insert_text( SDLGuiTK_Editable *editable, \
				      const char *new_text, \
				      int new_text_length, int *position );

  extern DECLSPEC
  void SDLGuiTK_editable_delete_text( SDLGuiTK_Editable *editable, \
				      int start_pos, int end_pos );

  extern DECLSPEC
  char * SDLGuiTK_editable_get_chars( SDLGuiTK_Editable *editable, \
				      int start_pos, int end_pos );

  

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_editable_h */


