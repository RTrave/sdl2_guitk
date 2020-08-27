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

#ifndef _SDLguitk_progressbar_h
#define _SDLguitk_progressbar_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_ProgressBar structure definition */
  typedef struct SDLGuiTK_ProgressBar SDLGuiTK_ProgressBar;

  extern DECLSPEC
  SDLGuiTK_ProgressBar *SDLGuiTK_PROGRESSBAR( SDLGuiTK_Widget *widget );


  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_progress_bar_new();

  extern DECLSPEC
  void SDLGuiTK_progress_bar_set_orientation (SDLGuiTK_ProgressBar *pbar,
                                              int orientation);

  extern DECLSPEC
  void SDLGuiTK_progress_bar_set_fraction (SDLGuiTK_ProgressBar *pbar,
                                           double fraction);

  extern DECLSPEC
  double SDLGuiTK_progress_bar_get_fraction (SDLGuiTK_ProgressBar *pbar);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_progressbar_h */


