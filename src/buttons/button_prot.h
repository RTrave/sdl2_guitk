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


/* SDLGuiTK_Button structure definition */
struct SDLGuiTK_Button {
  SDLGuiTK_Object * object;      /* referent object */

  SDLGuiTK_Bin * bin;            /* herits from */

  char * text;                   /* "public" data */

  SDL_Surface * srf;     /* "private" data */

  int           text_flag;       /* "private" data */
  int           pressed_flag;    /* "private" data */
  SDL_Surface * text_srf;        /* "private" data */
  SDL_Rect      text_area;       /* "private" data */
/*   SDL_Surface * inactive_srf;    /\* "private" data *\/ */
  SDL_Surface * active_srf;      /* "private" data */
  int           active_cflag;     /* "private" data */
  float         active_alpha_mod; /* "private" data */
/*   SDL_Surface * clicked_srf;     /\* "private" data *\/ */
};

