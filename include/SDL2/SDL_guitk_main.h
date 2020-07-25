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

#ifndef _SDLguitk_main_h
#define _SDLguitk_main_h

/* #include <SDL/SDL.h> */
/* #include "SDL_guitk.h" */

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif



  /* Init the context with MODE_SELF flag: */
  /* - initialize SDL, launch a internal thread and create a window */
  /* - internal thhread deals with SDL drawing fct and SDL_Events */
  extern DECLSPEC
  void SDLGuiTK_init( int argc, char **argv );

  /* This function have a effect only with MODE_SELF context !! */
  /* Start main loop. */
  extern DECLSPEC
  void SDLGuiTK_main();



  /* Init the context with MODE_SLAVE: */
  /* - uses gived surface to initialize himself */
  extern DECLSPEC
  void SDLGuiTK_init_with_window( SDL_Window * window, SDL_Renderer * renderer );
  /* - update function is SDLGuiTK_update() */
  /* - draw function is SDLGuiTK_blitsurfaces() */
  /* - event function is SDLGuiTK_pushevent() */

  /* This function have a effect only with MODE_SLAVE context !! */
  /* Check the textures states (Gen and Delete) */
  extern DECLSPEC
  void SDLGuiTK_update();

  /* This function have a effect only with MODE_SLAVE context !! */
  /* Draw active objects on main surface */
  extern DECLSPEC
  void SDLGuiTK_blitsurfaces();

  /* Push an SDL_Event in current context */
  extern DECLSPEC
  int  SDLGuiTK_pushevent( SDL_Event *event );



  /* Quit engine in both mode, MODE_SELF and MODE_SLAVE. */
  extern DECLSPEC
  void SDLGuiTK_main_quit();


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

#endif /* _SDLguitk_main_h */


