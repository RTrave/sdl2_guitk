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

#include "GL/gl.h"
#ifdef WIN32
#include <GL/glew.h>

#else
//#define GL3_PROTOTYPES 1
//#include <GL3/gl3.h>

#endif

#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "mysdl.h"
#include "object_prot.h"
#include "mywm.h"
#include "context_prot.h"


static int glflag=0;

static SDLGUITK_Video * main_video=NULL;

#if DEBUG_LEVEL >= 3
static int surfaces_counter = 0;
static SDLGuiTK_List   * surfaces=NULL;
#endif


void * MySDL_MainSurface_clean()
{
    SDLGuiTK_threads_enter();
    glMatrixMode( GL_MODELVIEW );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    SDLGuiTK_threads_leave();
    return (void *)NULL;
}


void * MySDL_SwapBuffers()
{
    SDLGuiTK_threads_enter();
    SDL_GL_SwapWindow(main_video->window);
    SDLGuiTK_threads_leave();
    return (void *)NULL;
}


static void Init_SDL()
{
    SDL_Init( SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS );
}

static void Quit_SDL()
{
    SDL_Quit();
}

static void MainVideo_init()
{
    main_video = malloc( sizeof( struct SDLGUITK_Video ) );
    main_video->width = 800;
    main_video->height = 600;
    main_video->fullscreen = 0;
    //main_video->opengl_ask = 1;
    main_video->bpp = 24;
    //glflag = 1;
}

/*
void MySDL_ModeOpenGL( SDL_bool state )
{
    if( state==SDL_TRUE ) {
        main_video->opengl_ask = 1;
    }
    else main_video->opengl_ask = 0;
}
*/
void MySDL_ModeFullScreen( SDL_bool state )
{
    if( state==SDL_TRUE ) {
        main_video->fullscreen = 1;
    }
    else main_video->fullscreen = 0;
}

void MySDL_ModeSetWidth( int width )
{
    main_video->width = width;
}

void MySDL_ModeSetHeight( int height )
{
    main_video->height = height;
}

/*
#ifdef HAVE_GL_GL_H
void MySDL_OpenGLEnable()
{
    MySDL_MainSurface_clean = MySDL_MainSurface_clean_GL;
    MySDL_SwapBuffers = MySDL_SwapBuffers_GL;
    MyWM_blitsurface_setopengl( 1 );
    SDLGUITK_LOG( "OpenGL ENABLED! \n" );
}
#endif
static void MySDL_OpenGLDisable()
{
    MySDL_MainSurface_clean = MySDL_MainSurface_clean_noGL;
    MySDL_SwapBuffers = MySDL_SwapBuffers_noGL;
    MyWM_blitsurface_setopengl( 0 );
    SDLGUITK_LOG( "OpenGL DISABLED! \n" );
}
*/

void MySDL_Init()
{
    Init_SDL();
    PROT__debug_init();
    MainVideo_init();
}

void MySDL_Uninit()
{
    /*   PROT__context_quit(); */
    PROT__debug_stop();
    Quit_SDL();
}

void MySDL_MainSurface_set( SDL_Window * window, SDL_Renderer * renderer )
{
    main_video = malloc( sizeof( struct SDLGUITK_Video ) );
    main_video->width = 800;
    main_video->height = 600;
    main_video->fullscreen = 0;
    //main_video->opengl_ask = 1;
    main_video->window = window;
    main_video->renderer = renderer;
    //main_video->context = context;
}

SDLGUITK_Video * MySDL_MainSurface_create()
{
    int i;
#if DEBUG_LEVEL >= 1
    char tmpstr[512];
#endif
    i=0;

    if( main_video->opengl_ask==0 ) {
        glflag=0;
    };
    glflag=1;

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
/*
    if( glflag==1 ) {
#ifdef HAVE_GL_GL_H
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        MySDL_OpenGLEnable();
#endif
        main_video->flags = SDL_WINDOW_OPENGL;
    } else {
        main_video->flags = 0;
        MySDL_OpenGLDisable();
    }
*/
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    main_video->flags = SDL_WINDOW_OPENGL;
    if( main_video->fullscreen==1 ) {
        main_video->flags |= SDL_WINDOW_FULLSCREEN;
        main_video->width = mode.w;
        main_video->height = mode.h;
    }

    main_video->window = SDL_CreateWindow( "SDL_GuiTK main display (MainRenderWindow)",
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           main_video->width, main_video->height,
                                           main_video->flags);
    SDL_SysWMinfo sysInfo;
    SDL_VERSION( &sysInfo.version );

    if (SDL_GetWindowWMInfo(main_video->window, &sysInfo) <= 0)
    {
#if DEBUG_LEVEL >= 1
        sprintf( tmpstr, "MainVideo_init(): modeset failed: %s\n", \
                 SDL_GetError( ) );
        SDLGUITK_ERROR( tmpstr );
#endif
        exit(0);
    }
    main_video->renderer = SDL_CreateRenderer(main_video->window, -1, SDL_RENDERER_ACCELERATED);
    main_video->context = SDL_GL_CreateContext(main_video->window);

    if( glflag==1 ) {

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
        SDL_GL_SwapWindow(main_video->window);
        glClearColor( (GLclampf)0.0, (GLclampf)0.0, (GLclampf)0.3, (GLclampf)1.0 );
    }

#ifdef WIN32
    // initialise GLEW
    glewInit();
#endif

    SDL_Delay(500);
    return main_video;
}


void MySDL_MainSurface_destroy()
{
    /*   PROT__context_quit(); */
}


SDL_Window * MySDL_GetVideoWindow()
{
    return main_video->window;
}

SDL_Surface * MySDL_GetVideoSurface()
{
    return SDL_GetWindowSurface( main_video->window );
}



static int srfid=0;

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

static MySDL_Surface * MySDL_CopySurface_with_alpha( MySDL_Surface * dst, \
        MySDL_Surface * src, \
        Uint8 alpha )
{
    int i=0, j=0;
    Uint32 *bufp_s,*bufp_d;
    Uint8 r, g, b, a;

    if( dst->srf!=NULL ) {
        SDL_FreeSurface( dst->srf );
        dst->srf = NULL;
    }

    MySDL_CreateRGBSurface( dst, src->srf->w, src->srf->h );
    SDL_LockSurface( src->srf );
    SDL_LockSurface( dst->srf );

    for( j=0; j<src->srf->h; j++ ) {
        for( i=0; i<src->srf->w; i++ ) {
            bufp_s = (Uint32 *)src->srf->pixels + j*src->srf->pitch/4 + i;
            bufp_d = (Uint32 *)dst->srf->pixels + j*dst->srf->pitch/4 + i;
            SDL_GetRGBA( *bufp_s, src->srf->format, &r, &g, &b, &a );
            if( alpha>a ) {
                *bufp_d = SDL_MapRGBA( dst->srf->format, r, g, b, a );
            } else {
                *bufp_d = SDL_MapRGBA( dst->srf->format, r, g, b, alpha );
            }
        }
    }

    SDL_UnlockSurface( dst->srf );
    SDL_UnlockSurface( src->srf );
    return dst;
}


void MySDL_GL_Enter2DMode()
{
    int w,h;

    SDL_GetWindowSize(main_video->window,&w,&h);
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


SDLGuiTK_Surface2D * MySDL_surface2D_new()
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

void MySDL_surface2D_destroy( SDLGuiTK_Surface2D * surface2D )
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


void MySDL_surface2D_update( SDLGuiTK_Surface2D * surface2D, \
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


void MySDL_surface2D_blitsurface( SDLGuiTK_Surface2D * surface2D, \
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


