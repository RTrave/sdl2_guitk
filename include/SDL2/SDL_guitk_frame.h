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

#ifndef _SDLguitk_frame_h
#define _SDLguitk_frame_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

  /* SDLGuiTK_Frame structure definition */
  typedef struct SDLGuiTK_Frame SDLGuiTK_Frame;

  extern DECLSPEC
  SDL_bool SDLGuiTK_IS_FRAME( SDLGuiTK_Widget * widget );
  extern DECLSPEC
  SDLGuiTK_Frame *SDLGuiTK_FRAME( SDLGuiTK_Widget *widget );


  /* ops. on SDLGuiTK_Frame objects */
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_frame_new();

  extern DECLSPEC
  void 	SDLGuiTK_frame_set_label (SDLGuiTK_Frame * frame,
                                  const char * label);

  extern DECLSPEC
  void 	SDLGuiTK_frame_set_label_widget (SDLGuiTK_Frame * frame,
                                         SDLGuiTK_Widget * label_widget);

  extern DECLSPEC
  const char * 	SDLGuiTK_frame_get_label (SDLGuiTK_Frame * frame );

  extern DECLSPEC
  SDLGuiTK_Widget * SDLGuiTK_frame_get_label_widget (SDLGuiTK_Frame * frame );


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_frame_h */


