/* 
   SDL_guitk - GUI toolkit designed for SDL environnements.

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

#ifndef _SDLguitk_button_h
#define _SDLguitk_button_h

//#include <SDL2/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

  /* SDLGuiTK_Button structure definition */
  typedef struct SDLGuiTK_Button SDLGuiTK_Button;

  extern DECLSPEC 
  SDLGuiTK_Button *SDLGuiTK_BUTTON( SDLGuiTK_Widget *widget );



  /*  */
  extern DECLSPEC
  SDLGuiTK_Widget * SDLGuiTK_button_new();
  extern DECLSPEC
  SDLGuiTK_Widget * SDLGuiTK_button_new_with_label( char * text );


  extern DECLSPEC
  char * gtk_button_get_label( char * text );
  extern DECLSPEC
  void * gtk_button_set_label( SDLGuiTK_Widget * button, char * text );


  /* SDLGuiTK_ToggleButton structure definition */
  typedef struct SDLGuiTK_ToggleButton SDLGuiTK_ToggleButton;

  extern DECLSPEC
  SDLGuiTK_ToggleButton *SDLGuiTK_TOGGLE_BUTTON( SDLGuiTK_Widget *widget );

  /*  */
  extern DECLSPEC
  SDLGuiTK_Widget * SDLGuiTK_toggle_button_new();
  extern DECLSPEC
  SDLGuiTK_Widget * SDLGuiTK_toggle_button_new_with_label( char * text );

  extern DECLSPEC
  SDL_bool SDLGuiTK_toggle_button_get_active( SDLGuiTK_ToggleButton * togglebutton );
  extern DECLSPEC
  void SDLGuiTK_toggle_button_set_active( SDLGuiTK_ToggleButton * togglebutton,
                                          SDL_bool is_active );


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_button_h */


