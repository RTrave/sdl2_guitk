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

#ifdef WIN32
#include <GL/glew.h>

#else
//#define GL3_PROTOTYPES 1
//#include <GL3/gl3.h>

#endif
#include "GL/gl.h"

#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_guitk.h>

#include "../debug.h"
#include "../mysdl.h"
#include "../object_prot.h"
#include "surface2d.h"
#include "../wmwidget.h"
#include "../context_prot.h"

#include "render.h"


static SDLGUITK_Render * main_render=NULL;


void Render_clean()
{
    SDLGuiTK_threads_enter();
    glMatrixMode( GL_MODELVIEW );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    SDLGuiTK_threads_leave();
}


void Render_swapbuffers()
{
    SDLGuiTK_threads_enter();
    SDL_GL_SwapWindow(main_render->window);
    SDLGuiTK_threads_leave();
}


static SDLGUITK_Render * Render_new()
{
    SDLGUITK_Render * new_render;
    new_render = malloc( sizeof( struct SDLGUITK_Render ) );
    new_render->width = 800;
    new_render->height = 600;
    new_render->fullscreen = 0;
    new_render->bpp = 24;
    return new_render;
}

void Render_ModeFullScreen( SDL_bool state )
{
    if( state==SDL_TRUE ) {
        main_render->fullscreen = 1;
    }
    else main_render->fullscreen = 0;
}

void Render_ModeSetWidth( int width )
{
    main_render->width = width;
}

void Render_ModeSetHeight( int height )
{
    main_render->height = height;
}

SDLGUITK_Render * Render_set( SDL_Window * window, SDL_Renderer * renderer )
{
    main_render = Render_new ();
    main_render->window = window;
    main_render->renderer = renderer;
    return main_render;
}

SDLGUITK_Render * Render_create()
{
    int i;
#if DEBUG_LEVEL >= 1
    char tmpstr[512];
#endif
    i=0;

    int display_count = 0, display_index = 0, mode_index = 0;
    SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

    if ((display_count = SDL_GetNumVideoDisplays()) < 1) {
        SDL_Log("SDL_GetNumVideoDisplays returned: %i", display_count);
        return NULL;
    }

    if (SDL_GetDisplayMode(display_index, mode_index, &mode) != 0) {
        SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
        return NULL;
    }

    main_render = Render_new ();

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    main_render->flags = SDL_WINDOW_OPENGL;
    if( main_render->fullscreen==1 ) {
        main_render->flags |= SDL_WINDOW_FULLSCREEN;
        main_render->width = mode.w;
        main_render->height = mode.h;
    }

    main_render->window = SDL_CreateWindow( "SDL_GuiTK main display (MainRenderWindow)",
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           main_render->width, main_render->height,
                                           main_render->flags);
    SDL_SysWMinfo sysInfo;
    SDL_VERSION( &sysInfo.version );

    if (SDL_GetWindowWMInfo(main_render->window, &sysInfo) <= 0)
    {
#if DEBUG_LEVEL >= 1
        sprintf( tmpstr, "MainVideo_init(): modeset failed: %s\n", \
                 SDL_GetError( ) );
        SDLGUITK_ERROR( tmpstr );
#endif
        exit(0);
    }
    main_render->renderer = SDL_CreateRenderer(main_render->window, -1, SDL_RENDERER_ACCELERATED);
    main_render->context = SDL_GL_CreateContext(main_render->window);

#if DEBUG_LEVEL >= 1

        SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &i );
        sprintf( tmpstr, "SDL_GL_RED_SIZE: %d\n", i );
        SDLGUITK_OUTPUT( tmpstr );
        SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &i );
        sprintf( tmpstr, "SDL_GL_GREEN_SIZE: %d\n", i );
        SDLGUITK_OUTPUT( tmpstr );
        SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &i );
        sprintf( tmpstr, "SDL_GL_BLUE_SIZE: %d\n", i );
        SDLGUITK_OUTPUT( tmpstr );
        SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &i );
        sprintf( tmpstr, "SDL_GL_ALPHA_SIZE: %d\n", i );
        SDLGUITK_OUTPUT( tmpstr );
        SDL_GL_GetAttribute( SDL_GL_BUFFER_SIZE, &i );
        sprintf( tmpstr, "SDL_GL_BUFFER_SIZE: %d\n", i );
        SDLGUITK_OUTPUT( tmpstr );
        SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &i );
        sprintf( tmpstr, "SDL_GL_DEPTH_SIZE: %d\n", i );
        SDLGUITK_OUTPUT( tmpstr );
        SDL_GL_GetAttribute( SDL_GL_STENCIL_SIZE, &i );
        sprintf( tmpstr, "SDL_GL_STENCIL_SIZE: %d\n", i );
        SDLGUITK_OUTPUT( tmpstr );

        glGetIntegerv( GL_STENCIL_BITS, &i );
        sprintf( tmpstr, "GL_STENCIL_BITS: %d\n", i );
        SDLGUITK_OUTPUT( tmpstr );
        printf( "CREATE\n" );
        glGetIntegerv( GL_DEPTH_BITS, &i );
        sprintf( tmpstr, "GL_DEPTH_BITS: %d\n", i );
        printf( "CREATE\n" );
        SDLGUITK_OUTPUT( tmpstr );

#endif

        //printf( "CREATE0a\n" );
        glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(main_render->window);
        glClearColor( (GLclampf)0.0, (GLclampf)0.0, (GLclampf)0.3, (GLclampf)1.0 );

#ifdef WIN32
    // initialise GLEW
    glewInit();
#endif

    SDL_Delay(500);
    return main_render;
}


void Render_destroy(SDLGUITK_Render * render)
{
    //TODO
    /*   PROT__context_quit(); */
}


SDL_Window * Render_GetVideoWindow()
{
    return main_render->window;
}

SDL_Surface * Render_GetVideoSurface()
{
    return SDL_GetWindowSurface( main_render->window );
}


