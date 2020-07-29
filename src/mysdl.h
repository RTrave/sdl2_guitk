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


typedef struct MySDL_Surface MySDL_Surface;

struct MySDL_Surface {
    SDLGuiTK_Object    * object;      /* referent widget */

    int id;
    char name[64];
    SDL_Surface *srf;
    Uint32 updated;
};

void MySDL_Surface_init();

void MySDL_Surface_uninit();

MySDL_Surface *MySDL_Surface_new(char *name);
void           MySDL_Surface_free(MySDL_Surface *surface);

void MySDL_CreateRGBSurface(MySDL_Surface *dst,
                            int w, int h);
void MySDL_CreateRGBSurface_WithColor(MySDL_Surface *dst, \
                                      int w, int h, \
                                      SDL_Color color);

void         MySDL_FreeSurface(MySDL_Surface *surface);

int MySDL_BlitSurface(MySDL_Surface*  src,
                      const SDL_Rect* srcrect,
                      MySDL_Surface*  dst,
                      SDL_Rect*       dstrect);

int MySDL_FillRect(MySDL_Surface*  dst,
                   const SDL_Rect* rect,
                   Uint32          color);

MySDL_Surface *MySDL_CopySurface( MySDL_Surface *dst, MySDL_Surface *src );

