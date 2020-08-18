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

#pragma once

#include <SDL2/SDL_guitk.h>


/* SDLGuiTK_Menu structure definition */
struct SDLGuiTK_Menu {
    SDLGuiTK_Object * object;       /* referent object */

    SDLGuiTK_Container * container; /* herits from */

    char                title[64];    /* "public" data */
    SDLGuiTK_MenuItem * selected;     /* "public" data */
    int  position;                    /* "public" data */

    SDL_Surface * srf;            /* "private" data */
    SDL_Rect      area;           /* "private" data */
    SDL_Rect      child_area;     /* "private" data */

    struct SDLGuiTK_MenuShell * menushell;    /* "private" data */
};

