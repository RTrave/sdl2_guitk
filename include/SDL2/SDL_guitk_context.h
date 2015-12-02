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

#ifndef _SDLguitk_context_h
#define _SDLguitk_context_h

/* #include <SDL/SDL.h> */
/* #include "SDL_guitk.h" */

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif



  /* SDLGuiTK_Context structure definition */
  /* Contains all informations and object references for current session */
  typedef struct SDLGuiTK_Context SDLGuiTK_Context;

  /* SDLGuiTK_WMWidget structure definition */
  typedef struct SDLGuiTK_WMWidget SDLGuiTK_WMWidget;
  typedef struct SDLGuiTK_Surface2D SDLGuiTK_Surface2D;

  /* This function have a effect only with MODE_SLAVE context !! */
  /* Check the textures states (Gen and Delete) */
  extern DECLSPEC
  void SDLGuiTK_context_check();

  /* This function have a effect only with MODE_SLAVE context !! */
  /* Draw active objects on main surface */
  extern DECLSPEC
  void SDLGuiTK_context_blitsurfaces();
  
  /* Push an SDL_Event in current context */
  extern DECLSPEC
  int  SDLGuiTK_context_pushevent( SDL_Event *event );


  /* Lock context to perform actions */
  extern DECLSPEC
  void  SDLGuiTK_threads_enter();
  /* Unlock context after actions */
  extern DECLSPEC
  void  SDLGuiTK_threads_leave();


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_context_h */


