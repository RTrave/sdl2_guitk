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

#ifndef _SDLguitk_grid_h
#define _SDLguitk_grid_h

#include <SDL2/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_Grid structure definition */
  /* Widget grouping boxes definition */
  typedef struct SDLGuiTK_Grid SDLGuiTK_Grid;

  extern DECLSPEC
  SDL_bool SDLGuiTK_IS_GRID( SDLGuiTK_Widget * widget );
  extern DECLSPEC
  SDLGuiTK_Grid *SDLGuiTK_GRID( SDLGuiTK_Widget *widget );

  /*  */
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_grid_new();

  extern DECLSPEC
  void SDLGuiTK_grid_attach ( SDLGuiTK_Grid *grid,
                              SDLGuiTK_Widget *child,
                              int left,
                              int top,
                              int width,
                              int height );

  extern DECLSPEC
  void SDLGuiTK_grid_set_column_spacing (SDLGuiTK_Grid *grid,
                                         int column_spacing);

  extern DECLSPEC
  int SDLGuiTK_grid_get_column_spacing (SDLGuiTK_Grid *grid);

  extern DECLSPEC
  void SDLGuiTK_grid_set_row_spacing (SDLGuiTK_Grid *grid,
                                      int row_spacing);

  extern DECLSPEC
  int SDLGuiTK_grid_get_row_spacing (SDLGuiTK_Grid *grid);

  extern DECLSPEC
  void SDLGuiTK_grid_set_column_homogeneous (SDLGuiTK_Grid *grid,
                                             SDL_bool column_homogeneous);

  extern DECLSPEC
  SDL_bool SDLGuiTK_grid_get_column_homogeneous (SDLGuiTK_Grid *grid);

  extern DECLSPEC
  void SDLGuiTK_grid_set_row_homogeneous (SDLGuiTK_Grid *grid,
                                          SDL_bool row_homogeneous);

  extern DECLSPEC
  SDL_bool SDLGuiTK_grid_get_row_homogeneous (SDLGuiTK_Grid *grid);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_grid_h */


