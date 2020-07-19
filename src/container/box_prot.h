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



/* SDLGuiTK_Box structure definition */
struct SDLGuiTK_Box {
  SDLGuiTK_Object    * object;      /* referent object */

  SDLGuiTK_Container * container;   /* herits from */

  int             type;             /* "public" data */
  int             spacing;          /* "public" data */
  SDL_bool        homogeneous;      /* "public" data */
  SDLGuiTK_List * children;         /* "public" data */

  SDL_Surface * srf;                /* "private" data */
  int homogeneous_size;             /* "private" data */
  int current_x, current_y;         /* "private" data */
  int base_size;
  int expanded_nb;                  /* "private" data */
  int expanded_size;                /* "private" data */
  int shown_nb;                     /* "private" data */
};


extern
void PROT__box_remove( SDLGuiTK_Box *this_box, SDLGuiTK_Widget *widget );


