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

#include <stdio.h>
#include <sys/types.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef STDC_HEADERS
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "../debug.h"
#include "../mysdl.h"
#include "../object_prot.h"
#include "surface2d.h"



void MySDL_GL_Enter2DMode()
{
    int w,h;

    SDL_GetWindowSize(SDL_GL_GetCurrentWindow(),&w,&h);
    /* Note, there may be other things you need to change,
       depending on how you have your OpenGL state set up.
    */
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    /* 	glShadeModel( GL_SMOOTH ); */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)w, (GLdouble)h, 0.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glViewport(0, 0, (GLdouble)w, (GLdouble)h);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void MySDL_GL_Leave2DMode()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();

    glPopAttrib();
}

/* Quick utility function for texture creation */
static int power_of_two(int input)
{
    int value = 1;

    while ( value < input ) {
        value <<= 1;
    }
    return value;
}

GLuint MySDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord, GLuint *tex )
{
    GLuint texture;
    int w, h;
    SDL_Surface *image;
    SDL_Rect area;

    /* Use the surface width and height expanded to powers of 2 */
    w = power_of_two(surface->w);
    h = power_of_two(surface->h);
    texcoord[0] = 0.0f;                     /* Min X */
    texcoord[1] = 0.0f;                     /* Min Y */
    texcoord[2] = (GLfloat)surface->w / w;  /* Max X */
    texcoord[3] = (GLfloat)surface->h / h;  /* Max Y */

    image = SDL_CreateRGBSurface(
                SDL_SWSURFACE,
                w, h,
                32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
                0x000000FF,
                0x0000FF00,
                0x00FF0000,
                0xFF000000
#else
                0xFF000000,
                0x00FF0000,
                0x0000FF00,
                0x000000FF
#endif
            );
    if ( image == NULL ) {
        return 0;
    }

    /* Copy the surface into the GL texture image */
    area.x = 0;
    area.y = 0;
    area.w = surface->w;
    area.h = surface->h;
    SDL_BlitSurface(surface, &area, image, &area);

    /* Create an OpenGL texture for the image */
    if( ! *tex ) {
        glGenTextures(1, &texture);
    } else {
        texture = *tex;
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 w, h,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image->pixels);
    SDL_FreeSurface(image);
    return texture;
}



static SDLGuiTK_List * surf2D_cache=NULL;


SDLGuiTK_Surface2D * Surface2D_new()
{
    SDLGuiTK_Surface2D * new_surface2D;

    if( surf2D_cache==NULL ) {
        surf2D_cache = SDLGuiTK_list_new();
    }

    SDLGuiTK_list_lock( surf2D_cache );
    new_surface2D = (SDLGuiTK_Surface2D *)SDLGuiTK_list_pop_head( surf2D_cache );
    SDLGuiTK_list_unlock( surf2D_cache );
    if( new_surface2D==NULL ) {
        new_surface2D = malloc( sizeof( struct SDLGuiTK_Surface2D ) );
    }
    new_surface2D->alpha = 1;
    new_surface2D->texMinX = 0;
    new_surface2D->texMinY = 0;
    new_surface2D->texMaxX = 0;
    new_surface2D->texMaxY = 0;
    new_surface2D->texture[0] = 0;
    //new_surface2D->mtexture = 0;
    new_surface2D->w = 0;
    new_surface2D->h = 0;
    new_surface2D->srf = NULL;

    return new_surface2D;
}

void Surface2D_destroy( SDLGuiTK_Surface2D * surface2D )
{
    const GLuint *texture=&surface2D->texture[0];
    surface2D->srf = NULL;
    glFinish();
    if( surface2D->texture[0] ) {
        glDeleteTextures( 1, texture );
    }
    if( glIsTexture(surface2D->texture[0]) ) {
        SDLGUITK_ERROR( "MySDL_surface2D_destroy(): texture seems not free\n" );
    } else {
        surface2D->texture[0] = 0;
    }
    SDLGuiTK_list_lock( surf2D_cache );
    SDLGuiTK_list_append( surf2D_cache, (SDLGuiTK_Object *) surface2D );
    /*   free( surface2D ); */
    SDLGuiTK_list_unlock( surf2D_cache );
}


void Surface2D_update( SDLGuiTK_Surface2D * surface2D, \
                       SDL_Surface *srf )
{
    GLfloat texcoord[4];

    if ( srf == NULL ) {
        return;
    }
    surface2D->srf = srf;

    surface2D->w = srf->w;
    surface2D->h = srf->h;

    /* Convert the image into an OpenGL texture */
    surface2D->texture[0] = MySDL_GL_LoadTexture( srf,
                            texcoord,
                            &surface2D->texture[0] );

    surface2D->texMinX = texcoord[0];
    surface2D->texMinY = texcoord[1];
    surface2D->texMaxX = texcoord[2];
    surface2D->texMaxY = texcoord[3];

    if ( ! surface2D->texture[0] ) {
        return;
    }
}


void Surface2D_blitsurface( SDLGuiTK_Surface2D * surface2D, \
                            int x, int y )
{
    glEnable( GL_BLEND );
    glColor4f( 1.0f, 1.0f, 1.0f, surface2D->alpha);
    glBindTexture( GL_TEXTURE_2D, surface2D->texture[0] );

    glBegin( GL_TRIANGLE_STRIP );
    glTexCoord2f( surface2D->texMinX, surface2D->texMinY );
    glVertex2i( x, y );
    glTexCoord2f( surface2D->texMaxX, surface2D->texMinY );
    glVertex2i( x+surface2D->w, y );
    glTexCoord2f( surface2D->texMinX, surface2D->texMaxY );
    glVertex2i( x, y+surface2D->h );
    glTexCoord2f( surface2D->texMaxX, surface2D->texMaxY );
    glVertex2i( x+surface2D->w, y+surface2D->h );
    glEnd();

    glDisable( GL_BLEND );
    /*
          SDL_Renderer* Main_Renderer=SDL_GetRenderer (SDL_GL_GetCurrentWindow());
        SDL_RenderCopy(Main_Renderer, surface2D->mtexture, NULL, );
    */
    /* MySDL_GL_Leave2DMode(); */


    //Apply the image stretched
    /* SDL_Rect stretchRect; */
    /* stretchRect.x = x; */
    /* stretchRect.y = y; */
    /* stretchRect.w = surface2D->w; */
    /* stretchRect.h = surface2D->h; */

    /* SDL_Texture* newTexture = NULL; */
    /* newTexture = SDL_CreateTextureFromSurface( main_video->renderer, surface2D->srf ); */
    /* SDL_RenderCopy( main_video->renderer, newTexture, NULL, &stretchRect ); */
}


