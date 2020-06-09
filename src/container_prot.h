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



/* SDLGuiTK_Container structure definition */
struct SDLGuiTK_Container {
  SDLGuiTK_Object * object;       /* referent object */

  SDLGuiTK_Widget * widget;       /* herits from */

  SDLGuiTK_Bin  * bin;            /* herited */
  SDLGuiTK_Box  * box;            /* herited */
  SDLGuiTK_Menu * menu;           /* herited */

  int border_width;               /* "public" data */

  /* "private" data */
  SDL_Rect      children_area;    /* w,h set in herited DrawUpdate (child suggestion)
                                   * x,y set in self DrawUpdate */
  SDL_Rect      bg_area;          /* "private" data */
};


/* Create and destroy structure */
extern
SDLGuiTK_Container *PROT__container_new();
extern
void PROT__container_destroy( SDLGuiTK_Container *container );

/* Draw the Container on his top surface */
extern
void PROT__container_DrawUpdate( SDLGuiTK_Container *container );
extern
void PROT__container_DrawBlit( SDLGuiTK_Container *container );

