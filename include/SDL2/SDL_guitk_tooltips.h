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

#ifndef _SDLguitk_tooltips_h
#define _SDLguitk_tooltips_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

  /* SDLGuiTK_Tooltips structure definition */
  typedef struct SDLGuiTK_Tooltips SDLGuiTK_Tooltips;
  typedef struct SDLGuiTK_TooltipsData SDLGuiTK_TooltipsData;

  extern DECLSPEC
  SDLGuiTK_Tooltips *SDLGuiTK_TOOLTIPS( SDLGuiTK_Tooltips *tooltips );



  /* ops. on SDLGuiTK_Tooltips objects */
  extern DECLSPEC
  SDLGuiTK_Tooltips *SDLGuiTK_tooltips_new();

  extern DECLSPEC
  void SDLGuiTK_tooltips_enable ( SDLGuiTK_Tooltips *tooltips );
  extern DECLSPEC
  void SDLGuiTK_tooltips_disable ( SDLGuiTK_Tooltips *tooltips );

  extern DECLSPEC
  void SDLGuiTK_tooltips_set_tip   (SDLGuiTK_Tooltips *tooltips,
				    SDLGuiTK_Widget *widget,
				    const char *tip_text,
				    const char *tip_private);



/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_tooltips_h */


