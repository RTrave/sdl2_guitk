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


/* SDLGuiTK_Label structure definition */
struct SDLGuiTK_Label {
  SDLGuiTK_Object * object;      /* referent object */

  SDLGuiTK_Misc * misc;          /* herits from */

  char * text;                   /* "public" data */

  int           text_flag;       /* "private" data */
  //SDL_Surface * text_srf;        /* "private" data */
  SDL_Rect      text_area;       /* "private" data */

  SDL_Surface * srf;             /* "private" data */
};

