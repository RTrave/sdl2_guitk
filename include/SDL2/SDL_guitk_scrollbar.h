/*
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

   Copyright (C) 2020 Trave Roman

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

#ifndef _SDLguitk_scrollbar_h
#define _SDLguitk_scrollbar_h

#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

  /* SDLGuiTK_Scrollbar structure definition */
  typedef struct SDLGuiTK_Scrollbar SDLGuiTK_Scrollbar;

  extern DECLSPEC
  SDLGuiTK_Scrollbar *SDLGuiTK_SCROLLBAR( SDLGuiTK_Widget *widget );


  /* Orientation of widget */
#define SDLGUITK_ORIENTATION_HORIZONTAL      ((int) 1)
#define SDLGUITK_ORIENTATION_VERTICAL        ((int) 2)


  /* ops. on SDLGuiTK_Scrollbar objects */
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_scrollbar_new(int orientation,
                                          SDLGuiTK_Adjustment *adjustment);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_scrollbar_h */


