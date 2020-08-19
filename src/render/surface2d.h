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

#pragma once

#include <GL/gl.h>
#include <SDL2/SDL_guitk.h>
//#include "../mysdl.h"

typedef struct SDLGuiTK_Surface2D SDLGuiTK_Surface2D;

/* SDLGuiTK_2DWidget structure definition */
struct SDLGuiTK_Surface2D {
    SDLGuiTK_Object    * object;      /* referent widget */

    /* "private" data */
    //SDL_Surface * srf;
    int           w, h;
    //SDL_Texture * mtexture;
    GLfloat   alpha;
    GLuint    texture[1];
    GLfloat   texMinX, texMinY;
    GLfloat   texMaxX, texMaxY;
};


SDLGuiTK_Surface2D * Surface2D_new();

void Surface2D_destroy(     SDLGuiTK_Surface2D * surface2D );

// Update the surface
void Surface2D_update(      SDLGuiTK_Surface2D * surface2D, \
                            SDL_Surface *srf );
// Low level blit surface (openGL)
void Surface2D_blitsurface( SDLGuiTK_Surface2D * surface2D, \
                            int x, int y );

// Low level functions (now internall code)
void MySDL_GL_Enter2DMode();
void MySDL_GL_Leave2DMode();
GLuint MySDL_GL_LoadTexture(SDL_Surface *surface,
                            GLfloat *texcoord,
                            GLuint *tex);

