/*
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

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

#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_guitk.h>


/* SDLGuiTK_Builder structure definition */
struct SDLGuiTK_Builder {
  SDLGuiTK_Object * object;      /* referent object */

  SDLGuiTK_Widget * top_widget;
  SDLGuiTK_List   * widgets;
};

SDLGuiTK_Widget * Extract_object(xmlNode * node);

int isnode(xmlNode * node);
int namecmp(xmlNode * node, char * string);
char * nameget(xmlNode * node);
int contentcmp(xmlNode * node, char * string);
char * contentget(xmlNode * node);
int propcmp(xmlNode * node, char * name, const char * string);
char * propget(xmlNode * node, const char * name);
