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

#ifndef _SDLguitk_menu_h
#define _SDLguitk_menu_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_Menu structure definition */
  /* Widget  definition */
  typedef struct SDLGuiTK_Menu SDLGuiTK_Menu;
  typedef struct SDLGuiTK_MenuShell SDLGuiTK_MenuShell;
  typedef struct SDLGuiTK_MenuItem SDLGuiTK_MenuItem;

  extern DECLSPEC
  SDL_bool SDLGuiTK_IS_MENU( SDLGuiTK_Widget * widget );
  extern DECLSPEC
  SDLGuiTK_Menu *SDLGuiTK_MENU( SDLGuiTK_Widget *widget );
  extern DECLSPEC
  SDL_bool SDLGuiTK_IS_MENUITEM( SDLGuiTK_Widget * widget );
  extern DECLSPEC
  SDLGuiTK_MenuItem *SDLGuiTK_MENUITEM( SDLGuiTK_Widget *widget );



  /* MENU functions */
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_menu_new();

  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_menu_get_active( SDLGuiTK_Menu *menu );

  /* MENUSHELL functions */
  extern DECLSPEC
  void SDLGuiTK_menu_shell_append( SDLGuiTK_Menu *menu, \
				   SDLGuiTK_Widget *child );


  /* MENUITEM functions */
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_menuitem_new();
  extern DECLSPEC
  SDLGuiTK_Widget *SDLGuiTK_menuitem_new_with_label( char *label );
  extern DECLSPEC
  void SDLGuiTK_menuitem_select( SDLGuiTK_MenuItem *menu_item );


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_menu_h */


