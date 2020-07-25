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

#ifdef WIN32
#include <GL/glew.h>

#else
//#define GL3_PROTOTYPES 1
//#include <GL3/gl3.h>

#endif
#include "GL/gl.h"

#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "mysdl.h"
#include "object_prot.h"
#include "render/surface2d.h"
#include "wmwidget.h"
#include "context_prot.h"



static int srfid=0;
#if DEBUG_LEVEL >= 3
static int surfaces_counter = 0;
static SDLGuiTK_List   * surfaces=NULL;
#endif


void MySDL_Surface_init()
{
#if DEBUG_LEVEL >= 3
    surfaces = SDLGuiTK_list_new();
#endif
}

void MySDL_Surface_uninit()
{
#if DEBUG_LEVEL >= 3
    SDLGuiTK_list_destroy( surfaces );
#endif
}

MySDL_Surface *MySDL_Surface_new(char *name )
{
    MySDL_Surface *new_surface;
    new_surface = malloc( sizeof( struct MySDL_Surface ) );
    new_surface->object = NULL;
    new_surface->id = srfid++;
    new_surface->srf = NULL;
    strcpy(new_surface->name,name);
#if DEBUG_LEVEL >= 3
    printf("MySDL_Surface_new():%s %d\n", new_surface->name, new_surface->id);
    SDLGuiTK_list_append( surfaces, (SDLGuiTK_Object *) new_surface );
#endif
    return new_surface;
}

void           MySDL_Surface_free( MySDL_Surface *surface )
{
#if DEBUG_LEVEL >= 3
    printf("MySDL_Surface_free():%s %d\n", surface->name, surface->id);
#endif
    if( surface->srf!=NULL ) {
        SDLGUITK_ERROR( "MySDL_Surface_free(): surface!=NULL .. deleting\n" );
        MySDL_FreeSurface( surface );
    }
#if DEBUG_LEVEL >= 3
    SDLGuiTK_list_remove( surfaces, (SDLGuiTK_Object *)surface );
#endif
    free(surface);
}

void MySDL_CreateRGBSurface( MySDL_Surface *surface,
                             int w, int h )
{
    if( surface->srf!=NULL ) {
        if(surface->srf->w==w && surface->srf->h==h)
            return;
        MySDL_FreeSurface( surface );
    }
    surface->srf = SDL_CreateRGBSurface( 0, w, h, 32,
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
#if DEBUG_LEVEL >= 3
    surfaces_counter++;
    printf("<surfaces_counter:%d> MySDL_CreateRGBSurface()\n",surfaces_counter);
#endif
}

void MySDL_CreateRGBSurface_WithColor( MySDL_Surface *surface, \
                                       int w, int h, \
                                       SDL_Color color )
{
    Uint32 bgcolor;

    MySDL_CreateRGBSurface( surface, w, h );
    bgcolor = SDL_MapRGBA( surface->srf->format, \
                           color.r, \
                           color.g, \
                           color.b, \
                           255 );
    SDL_FillRect( surface->srf, NULL, bgcolor );
}

void MySDL_FreeSurface( MySDL_Surface * surface )
{
    if( surface->srf==NULL ) {
        SDLGUITK_ERROR( "MySDL_FreeSurface(): surface==NULL .. return\n" );
        return;
    }
    SDL_FreeSurface( surface->srf );
    surface->srf = NULL;
#if DEBUG_LEVEL >= 3
    surfaces_counter--;
    printf("<surfaces_counter:%d> MySDL_FreeSurface()\n",surfaces_counter);
#endif
}


int MySDL_BlitSurface(MySDL_Surface*    src,
                      const SDL_Rect* srcrect,
                      MySDL_Surface*    dst,
                      SDL_Rect*       dstrect)
{
    return SDL_BlitSurface(src->srf, srcrect, dst->srf, dstrect);
}

int MySDL_FillRect(MySDL_Surface*  dst,
                   const SDL_Rect* rect,
                   Uint32          color)
{
    return SDL_FillRect(dst->srf, rect, color);
}

MySDL_Surface * MySDL_CopySurface( MySDL_Surface * dst, \
                                   MySDL_Surface * src )
{
    if( dst->srf!=NULL ) {
        SDL_FreeSurface( dst->srf );
        dst->srf = NULL;
    }
    dst->srf = src->srf;
    return dst;
}


/* static MySDL_Surface * MySDL_CopySurface_with_alpha( MySDL_Surface * dst, \ */
/*         MySDL_Surface * src, \ */
/*         Uint8 alpha ) */
/* { */
/*     int i=0, j=0; */
/*     Uint32 *bufp_s,*bufp_d; */
/*     Uint8 r, g, b, a; */

/*     if( dst->srf!=NULL ) { */
/*         SDL_FreeSurface( dst->srf ); */
/*         dst->srf = NULL; */
/*     } */

/*     MySDL_CreateRGBSurface( dst, src->srf->w, src->srf->h ); */
/*     SDL_LockSurface( src->srf ); */
/*     SDL_LockSurface( dst->srf ); */

/*     for( j=0; j<src->srf->h; j++ ) { */
/*         for( i=0; i<src->srf->w; i++ ) { */
/*             bufp_s = (Uint32 *)src->srf->pixels + j*src->srf->pitch/4 + i; */
/*             bufp_d = (Uint32 *)dst->srf->pixels + j*dst->srf->pitch/4 + i; */
/*             SDL_GetRGBA( *bufp_s, src->srf->format, &r, &g, &b, &a ); */
/*             if( alpha>a ) { */
/*                 *bufp_d = SDL_MapRGBA( dst->srf->format, r, g, b, a ); */
/*             } else { */
/*                 *bufp_d = SDL_MapRGBA( dst->srf->format, r, g, b, alpha ); */
/*             } */
/*         } */
/*     } */

/*     SDL_UnlockSurface( dst->srf ); */
/*     SDL_UnlockSurface( src->srf ); */
/*     return dst; */
/* } */


