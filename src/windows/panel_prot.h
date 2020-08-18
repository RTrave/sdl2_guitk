/* 
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

   Copyright (C) 2007 Trave Roman

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
#include "../wmwidget.h"


/* SDLGuiTK_Panel structure definition */
struct SDLGuiTK_Panel {
  SDLGuiTK_Object * object;     /* referent object */

  SDLGuiTK_Bin      * bin;      /* herits from */
  SDLGuiTK_WMWidget * wm_widget;/* herits from */

  /* "public" data */
  int  position;

  /* "private" data */
  MySDL_Surface * srf;
  SDL_Rect        area;
};


