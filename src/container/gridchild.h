/*
   SDL_guitk - GUI toolkit designed for SDL environnements.

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


struct GridChild {
  SDLGuiTK_Object * object;

  SDLGuiTK_Widget * child;
  int               left;
  int               top;
  int               width;
  int               height;

  SDL_Rect          area;
};

typedef struct GridChild GridChild;


/* Create and destroy structure */
extern
GridChild *GridChild_create( SDLGuiTK_Widget *widget );
extern
void      GridChild_destroy( GridChild *child );

/* Find or remove a GridChild's widget of his grid */
extern
GridChild *GridChild_find( SDLGuiTK_Grid *grid, SDLGuiTK_Widget *widget );
extern
GridChild *GridChild_remove( SDLGuiTK_Grid *grid, SDLGuiTK_Widget *widget );

/* Draw the GridChild on his top surface */
extern
void      GridChild_DrawUpdate( GridChild *child );
extern
void      GridChild_DrawBlit( GridChild *child );


