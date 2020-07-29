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
#include "render/surface2d.h"

/* SDLGuiTK_Tooltips structure definition */
struct SDLGuiTK_Tooltips {
  SDLGuiTK_Object * object;     /* referent object */

/*   SDLGuiTK_WMWidget * wm_widget;   /\* WM widget *\/ */
  SDLGuiTK_Surface2D *surface2D;

  int  enabled;

  SDL_Surface * srf;            /* "private" data */
  SDL_Rect      area;           /* "private" data */
  SDL_Surface * tip_srf;        /* "private" data */
  SDL_Rect      tip_area;       /* "private" data */
};

/* SDLGuiTK_Tooltips structure definition */
struct SDLGuiTK_TooltipsData {
  SDLGuiTK_Object * object;     /* referent object */

  SDLGuiTK_Tooltips * tooltips; /*  */
  SDLGuiTK_Widget * widget;   /* linked widget */
  const char *tip_text;               /* "public" data */
  int updated;
};

int
PROT__TooltipsData_update( SDLGuiTK_TooltipsData *tooltipsdata );

void 
PROT__Tooltips_DestroyAll();

void 
PROT__TooltipsData_Destroy( SDLGuiTK_TooltipsData *tooltipsdata );
