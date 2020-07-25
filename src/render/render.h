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

#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_guitk.h>

typedef struct SDLGUITK_Render SDLGUITK_Render;

struct SDLGUITK_Render {
    int width, height;          /* public data */
    int bpp;                    /* public data */
    int fullscreen;             /* public data */
    int opengl_ask;             /* public data */

    Uint32 flags;               /* private data */
    SDL_Window    * window;     /* private data */
    SDL_Renderer  *renderer;    /* private data */
    SDL_GLContext *context;     /* Our opengl context handle */
};



SDLGUITK_Render * Render_set( SDL_Window * window, SDL_Renderer * renderer );

SDLGUITK_Render * Render_create();

void              Render_destroy(SDLGUITK_Render * render);

void              Render_clean();

void              Render_swapbuffers();


SDL_Window  * Render_GetVideoWindow();

SDL_Surface * Render_GetVideoSurface();


void Render_ModeFullScreen( SDL_bool state );

void Render_ModeSetWidth( int width );

void Render_ModeSetHeight( int height );


