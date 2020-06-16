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



/* SDLGuiTK_Bin structure definition */
struct SDLGuiTK_Bin {
  SDLGuiTK_Object         * object;         /* referent instance */

  SDLGuiTK_Container      * container;      /* instance from */

  SDLGuiTK_Window         * window;         /* herited */
  SDLGuiTK_Panel          * panel;          /* herited */
  SDLGuiTK_Alignment      * alignment;      /* herited */
  SDLGuiTK_Button         * button;         /* herited */
  SDLGuiTK_MenuItem       * menuitem;       /* herited */
  SDLGuiTK_ScrolledWindow * scrolledwindow; /* herited */

  /* "public" data */
  SDLGuiTK_Widget * child;

  /* "private" data */
  //MySDL_Surface * srf;
};

/* Create and destroy structure */
extern
SDLGuiTK_Bin *PROT__bin_new();
extern
void PROT__bin_destroy( SDLGuiTK_Bin *bin );

/* Add or remove a component to Bin */
extern
void PROT__bin_add( SDLGuiTK_Bin *bin, SDLGuiTK_Widget *widget );
extern
void PROT__bin_remove( SDLGuiTK_Bin *bin, SDLGuiTK_Widget *widget );

/* Draw the Bin on his top surface */
extern
void PROT__bin_DrawUpdate( SDLGuiTK_Bin *bin );
extern
void PROT__bin_DrawBlit( SDLGuiTK_Bin *bin );

