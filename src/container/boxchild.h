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

#pragma once

#include <SDL2/SDL_guitk.h>


struct BoxChild {
  SDLGuiTK_Object * object;

  SDLGuiTK_Widget * child;
  SDL_bool          expand;
  SDL_bool          fill;
  int               padding;

  SDL_Rect          area;
};

typedef struct BoxChild BoxChild;


/* Create and destroy structure */
extern
BoxChild *BoxChild_create( SDLGuiTK_Widget *widget );
extern
void      BoxChild_destroy( BoxChild *child );

/* Find or remove a BoxChild's widget of his box */
extern
BoxChild *BoxChild_find( SDLGuiTK_Box *box, SDLGuiTK_Widget *widget );
extern
BoxChild *BoxChild_remove( SDLGuiTK_Box *box, SDLGuiTK_Widget *widget );

/* Draw the BoxChild on his top surface */
extern
void      BoxChild_DrawUpdate( BoxChild *child );
extern
void      BoxChild_DrawBlit( BoxChild *child );


