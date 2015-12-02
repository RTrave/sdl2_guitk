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


typedef struct SDLGuiTK_Theme SDLGuiTK_Theme;

/* SDLGuiTK_Theme structure definition */
struct SDLGuiTK_Theme {
/*   SDLGuiTK_Widget *widget; */

  /* themeable values and associate mutex */
  SDL_Color bgcolor;        /* background color */
  SDL_Color bdcolor;        /* border color */
  SDL_Color ftcolor;        /* font color */
  SDL_Color wgcolor;        /* widgets background color */

  char font_file[256];      /* font file */
  int font_size;            /* font size */

/*   SDL_Surface * ref_srf;    /\* "private data" *\/ */
};

extern
void PROT__theme_init();
extern
void PROT__theme_uninit();

extern
SDLGuiTK_Theme *PROT__theme_get_and_lock();
extern
void            PROT__theme_unlock( SDLGuiTK_Theme *theme );

