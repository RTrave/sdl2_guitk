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

#pragma once

#include <SDL2/SDL_guitk.h>
#include "gridchild.h"


/* SDLGuiTK_Grid structure definition */
struct SDLGuiTK_Grid {
  SDLGuiTK_Object    * object;      /* referent object */

  SDLGuiTK_Container * container;   /* herits from */

  /* "public" data */
  SDL_bool        column_homogeneous;
  int             column_spacing;
  SDL_bool        row_homogeneous;
  int             row_spacing;
  SDLGuiTK_List * children;

  /* "private" data */
  int columns;
  int column_homogeneous_size;
  int rows;
  int row_homogeneous_size;
  GridChild *child;
  int *gx, *gy;
  int current_x, current_y;
  int base_width;
  int base_height;
  int expanded_size;
  int shown_nb;
};

void PROT__grid_remove( SDLGuiTK_Grid *this_grid, SDLGuiTK_Widget *widget );
void PROT__grid_set_top( SDLGuiTK_Grid *grid, SDLGuiTK_Widget *top);

