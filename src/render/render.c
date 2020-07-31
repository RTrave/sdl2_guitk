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


#include <SDL2/SDL_syswm.h>
#include "render.h"
#include "../debug.h"
#include "../context_prot.h"

//static SDLGuiTK_Render * main_render=NULL;


void Render_clean()
{
    //SDLGuiTK_threads_enter();
//    if(!main_render) return;
    glMatrixMode( GL_MODELVIEW );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    //SDLGuiTK_threads_leave();
}


void Render_swapbuffers(SDLGuiTK_Render *render)
{
    //SDLGuiTK_threads_enter();
//    if(!main_render) return;
    SDL_GL_SwapWindow(render->window);
    //SDLGuiTK_threads_leave();
}

static int ask_fullscreen = 0;
static int ask_width = 800;
static int ask_height = 800;
static int render_id=0;


static SDLGuiTK_Render * Render_new()
{
    SDLGuiTK_Render * new_render;
    new_render = malloc( sizeof( struct SDLGuiTK_Render ) );

    new_render->width = ask_width;
    new_render->height = ask_height;
    new_render->fullscreen = ask_fullscreen;
    new_render->bpp = 24;
    new_render->context = NULL;
    new_render->renderer = NULL;
    new_render->window = NULL;
    new_render->id = render_id++;
    return new_render;
}

void Render_ModeFullScreen( SDL_bool state )
{
    if( state==SDL_TRUE ) {
        ask_fullscreen = 1;
    }
    else ask_fullscreen = 0;
}

void Render_ModeSetWidth( int width )
{
    ask_width = width;
}

void Render_ModeSetHeight( int height )
{
    ask_height = height;
}

SDLGuiTK_Render * Render_set( SDL_Window * window, SDL_Renderer * renderer )
{
    SDLGuiTK_Render * render = Render_new ();
    render->window = window;
    render->renderer = renderer;
    return render;
}

static int context_exists=0;

SDLGuiTK_Render * Render_create()
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

    SDLGuiTK_Render * main_render = Render_new ();
    main_render->flags = SDL_WINDOW_OPENGL;
    if( main_render->fullscreen==1 ) {
        main_render->flags |= SDL_WINDOW_FULLSCREEN;
        main_render->width = mode.w;
        main_render->height = mode.h;
    }
    //if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE)
    //    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, context_exists);
    context_exists=1;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    //SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
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
    //SDL_GLContext * glcontext = SDL_GL_CreateContext(main_render->window);
    SDL_GL_MakeCurrent (main_render->window, main_render->context);
    //SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, main_render->id);
    //SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    //SDL_GL_SetSwapInterval(0);

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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //printf( "CREATE0a\n" );
        glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        //SDL_GL_SwapWindow(main_render->window);
        //glClearColor( (GLclampf)0.0, (GLclampf)0.0, (GLclampf)0.3, (GLclampf)1.0 );

#ifdef WIN32
    // initialise GLEW
    glewInit();
#endif

    SDL_Delay(500);
    return main_render;
}


void Render_destroy(SDLGuiTK_Render * render)
{
    //TODO
    /*   PROT__context_quit(); */
    SDL_DestroyRenderer(render->renderer);
    SDL_DestroyWindow(render->window);
    free(render);
}


SDL_Window * Render_GetVideoWindow(SDLGuiTK_Render * render)
{
    if(render)
        return render->window;
    return NULL;
}

SDL_Surface * Render_GetVideoSurface(SDLGuiTK_Render * render)
{
    if(render)
        return SDL_GetWindowSurface( render->window );
    return NULL;
}


