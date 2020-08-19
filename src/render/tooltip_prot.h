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
#include "surface2d.h"
#include "../mysdl.h"

/* SDLGuiTK_Tooltip structure definition */
struct SDLGuiTK_Tooltip {
    SDLGuiTK_Object * object;     /* referent object */

    SDLGuiTK_Bin * bin;            /* herits from */

  /* "private" data */
    MySDL_Surface       * srf;
    SDL_Rect              area;
    SDLGuiTK_Widget     * label;
    SDLGuiTK_Surface2D * surface2D;
};

SDLGuiTK_Tooltip * PROT__Tooltip_create();
void PROT__Tooltip_destroy(SDLGuiTK_Tooltip * tooltip);

void PROT__Tooltip_update( SDLGuiTK_Tooltip * tooltip, char *text );

void PROT__Tooltip_blit( SDLGuiTK_Tooltip * tooltip );


