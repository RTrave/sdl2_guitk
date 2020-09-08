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

#ifndef _SDLguitk_misc_h
#define _SDLguitk_misc_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_Misc structure definition */
  /*  */
  typedef struct SDLGuiTK_Misc SDLGuiTK_Misc;

  extern DECLSPEC
  SDL_bool SDLGuiTK_IS_MISC( SDLGuiTK_Widget * widget );
  extern DECLSPEC
  SDLGuiTK_Misc *SDLGuiTK_MISC( SDLGuiTK_Widget *widget );



  /*  */
  extern DECLSPEC
  void SDLGuiTK_misc_set_alignment( SDLGuiTK_Misc * misc, \
				    float xalign, \
				    float yalign );
  
  extern DECLSPEC
  void SDLGuiTK_misc_set_padding(   SDLGuiTK_Misc * misc, \
				    int xpad, \
				    int ypad );
  
  extern DECLSPEC
  void SDLGuiTK_misc_get_alignment( SDLGuiTK_Misc * misc, \
				    float * xalign, \
				    float * yalign );
  
  extern DECLSPEC
  void SDLGuiTK_misc_get_padding(   SDLGuiTK_Misc * misc, \
				    int * xpad, \
				    int * ypad );



/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_misc_h */


