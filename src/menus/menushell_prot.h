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


/* SDLGuiTK_MenuShell structure definition */
struct SDLGuiTK_MenuShell {
  SDLGuiTK_Object    * object;     /* referent object */

  SDLGuiTK_Widget    * widget;  /* herits from */

  SDLGuiTK_Menu      * menu;       /* referent menu */

  /* "public" data */
  SDLGuiTK_List      * children;   /* "public" data */

  /* "private" data */
  SDLGuiTK_WMWidget  * wm_widget;  /* herits from */
  int                  wm_flag;
  int                  title_flag;
  MySDL_Surface      * title_srf;
  SDL_Rect             title_area;
};

extern void
PROT__menushell_create( SDLGuiTK_Menu * menu );
extern void
PROT__menushell_destroy( SDLGuiTK_Menu * menu );

extern void
PROT__menushell_start( SDLGuiTK_Menu * menu );
