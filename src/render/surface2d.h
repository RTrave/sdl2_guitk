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


/* SDLGuiTK_2DWidget structure definition */
struct SDLGuiTK_Surface2D {
    SDLGuiTK_Object    * object;      /* referent widget */

    SDL_Surface * srf;                /* "private" data */
    int           w, h;               /* "private" data */
    //SDL_Texture * mtexture;
    GLfloat   alpha;                  /* "private" data */
    GLuint    texture[1];             /* "private" data */
    GLfloat   texMinX, texMinY;       /* "private" data */
    GLfloat   texMaxX, texMaxY;       /* "private" data */
};

extern
SDLGuiTK_Surface2D * Surface2D_new();
extern
void Surface2D_destroy(     SDLGuiTK_Surface2D * surface2D );

// Update the surface
extern
void Surface2D_update(      SDLGuiTK_Surface2D * surface2D, \
                            SDL_Surface *srf );
// Low level blit surface (openGL)
extern
void Surface2D_blitsurface( SDLGuiTK_Surface2D * surface2D, \
                            int x, int y );

// Low level functions (now internall code)
extern
void MySDL_GL_Enter2DMode();
extern
void MySDL_GL_Leave2DMode();
extern
GLuint MySDL_GL_LoadTexture(SDL_Surface *surface,
                            GLfloat *texcoord,
                            GLuint *tex);

