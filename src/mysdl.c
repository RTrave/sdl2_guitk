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

#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "mysdl.h"
#include "object_prot.h"
#include "mywm.h"
#include "context_prot.h"


//static Uint32 bgcolor;
static int glflag=0;

static SDLGUITK_Video * main_video=NULL;

#if DEBUG_LEVEL >= 3
static int surfaces_counter = 0;
static SDLGuiTK_List   * surfaces=NULL;
#endif

#ifdef HAVE_GL_GL_H
void * MySDL_MainSurface_clean_GL()
{
    SDLGuiTK_threads_enter();
    glMatrixMode( GL_MODELVIEW );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    /* GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT */
    SDLGuiTK_threads_leave();
    return (void *)NULL;
}
#endif

void * MySDL_MainSurface_clean_noGL()
{
    SDLGuiTK_threads_enter();
    SDLGuiTK_threads_leave();
    return (void *)NULL;
}

#ifdef HAVE_GL_GL_H
static void * MySDL_SwapBuffers_GL()
{
    SDLGuiTK_threads_enter();
    //SDL_GL_SwapBuffers();
    SDL_GL_SwapWindow(main_video->window);
    SDLGuiTK_threads_leave();
    return (void *)NULL;
}
#endif

static void * MySDL_SwapBuffers_noGL()
{
    SDLGuiTK_threads_enter();
    //SDL_Flip( current_context->surface );
    SDL_RenderPresent(main_video->renderer);
    SDLGuiTK_threads_leave();
    return (void *)NULL;
}

void * (*MySDL_MainSurface_clean)();
void * (*MySDL_SwapBuffers)();


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
    /* const SDL_VideoInfo* info=NULL; */
    //Uint32 bpp;
#if DEBUG_LEVEL >= 1
    //char tmpstr[512];
#endif

    main_video = malloc( sizeof( struct SDLGUITK_Video ) );
    main_video->width = 1200;
    main_video->height = 800;
    main_video->fullscreen = 0;
    main_video->opengl_ask = 1;
    /*
    info = SDL_GetVideoInfo();
    if( !info ) {
    #if DEBUG_LEVEL >= 1
      sprintf( tmpstr, "MainVideo_init(): GetVideoInfo failed: %s\n", SDL_GetError() );
      SDLGUITK_ERROR( tmpstr );
    #endif
      exit(0);
    }

    main_video->bpp = info->vfmt->BitsPerPixel;
    switch( main_video->bpp ) {
    case 16:
    //    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    //    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
    //    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    //    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 1 );
    //    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 16 );
      break;
    case 32:
    //    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    //    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    //    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    //    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    //    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
      break;
    default:
      break;
    }
    */

    /* #ifdef HAVE_GL_GL_H */
    /*   bpp=SDL_VideoModeOK(640, 480, 32, SDL_HWSURFACE|SDL_OPENGL); */
    /*   if( !bpp ) { */
    /*     glflag = 0; */
    /*     SDLGUITK_ERROR("OpeGL Mode not available.\n"); */
    /* #endif */
    /*     bpp=SDL_VideoModeOK(640, 480, 32, SDL_HWSURFACE); */
    /*     if( !bpp ) { */
    /*       SDLGUITK_ERROR("Mode not available.\n"); */
    /*       exit(-1); */
    /*     } */
    /* #ifdef HAVE_GL_GL_H */
    /*   } else { */
    /*     glflag = 1; */
    /*   } */
    /* #endif */
    glflag = 1;
    /*   if( ((int)main_video->bpp)!=((int)bpp) ) { */
    /* #if DEBUG_LEVEL >= 1 */
    /*     sprintf( tmpstr, "MainVideo_init(): founded %dbpp, system says: %dbpp\n", \ */
    /* 	     main_video->bpp, \ */
    /* 	     bpp ); */
    /*     SDLGUITK_ERROR( tmpstr ); */
    /* #endif */
    /* /\*     main_video->bpp = 24; *\/ */
    /*   } else { */
    /* #if DEBUG_LEVEL >= 1 */
    /*     sprintf( tmpstr, "MainVideo_init(): founded and system says: %dbpp\n", \ */
    /* 	     bpp ); */
    /*     SDLGUITK_ERROR( tmpstr ); */
    /* #endif */
    /*   } */

    if( ((int)main_video->bpp)!=32 ) {
        SDLGUITK_ERROR( "Try to emulate with a 32bpp forced surface!\n" );
        main_video->bpp = 32;
    }

    /*   video_flags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_OPENGLBLIT|SDL_SRCALPHA|SDL_HWACCEL; */
    /*   video_flags |= SDL_RESIZABLE|SDL_SRCALPHA|SDL_HWACCEL; */
}


void MySDL_ModeOpenGL( SDL_bool state )
{
    /*   printf("TEST OK\n"); */
    if( state==SDL_TRUE ) {
        main_video->opengl_ask = 1;
    }
    else main_video->opengl_ask = 0;
}

void MySDL_ModeFullScreen( SDL_bool state )
{
    if( state==SDL_TRUE ) {
        main_video->fullscreen = 1;
        /*     main_video->flags |= SDL_FULLSCREEN; */
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
    main_video->width = 2200;
    main_video->height = 800;
    main_video->fullscreen = 0;
    main_video->opengl_ask = 1;
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

    if( glflag==1 ) {
#ifdef HAVE_GL_GL_H
        /*   SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 ); */
        /*   SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 ); */
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        MySDL_OpenGLEnable();
#endif
        /*   main_video->flags = SDL_OPENGLBLIT; |SDL_SRCALPHA|SDL_HWACCEL*/
        main_video->flags = SDL_WINDOW_OPENGL;
    } else {
        main_video->flags = 0;  /* |SDL_SRCALPHA */
        MySDL_OpenGLDisable();
    }

    /*   main_video->flags = SDL_OPENGLBLIT; |SDL_SRCALPHA|SDL_HWACCEL*/

    if( main_video->fullscreen==1 ) main_video->flags |= SDL_WINDOW_FULLSCREEN;

    /* main_video->window = SDL_CreateWindow( "SDL_GuiTK main display (MainRenderWindow)", */
    /*                                        SDL_WINDOWPOS_UNDEFINED, */
    /*                                        SDL_WINDOWPOS_UNDEFINED, */
    /*                                        800,600, */
    /*                                        SDL_WINDOW_OPENGL); */
    main_video->window = SDL_CreateWindow( "MainVideo",
                					       SDL_WINDOWPOS_UNDEFINED,
                					       SDL_WINDOWPOS_UNDEFINED,
                					       main_video->width,
                					       main_video->height,
                                           0);
    SDL_SysWMinfo sysInfo;
    SDL_VERSION( &sysInfo.version );

    if (SDL_GetWindowWMInfo(main_video->window, &sysInfo) <= 0)
        //2if( main_video->window == 0 );
    {
#if DEBUG_LEVEL >= 1
        sprintf( tmpstr, "MainVideo_init(): modeset failed: %s\n", \
                 SDL_GetError( ) );
        SDLGUITK_ERROR( tmpstr );
#endif
        exit(0);
    }
    /* main_video->renderer = SDL_CreateRenderer(main_video->window, -1, 0); */
    main_video->renderer = SDL_CreateRenderer(main_video->window, -1, SDL_RENDERER_ACCELERATED);

    main_video->context = SDL_GL_CreateContext(main_video->window);

#ifdef HAVE_GL_GL_H

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
#else
    SDLGUITK_ERROR( "HAVE_GL_GL_H not defined at build time !!!\n" );
#endif

#ifdef WIN32
    // initialise GLEW
    glewInit();
#endif

    //SDL_WM_SetCaption( "SDL Gui ToolKit", "SDL_GuiTK main display" );

    /*   SDL_SetAlpha( main_video->surface, SDL_SRCALPHA|SDL_RLEACCEL, 255 ); /\*  *\/ */

    /*   MySDL_MainSurface_clean(); */
    /*   MySDL_SwapBuffers(); */
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
    printf("TESTEST\n");
    surfaces = SDLGuiTK_list_new();
#endif
}

void MySDL_Surface_uninit()
{
#if DEBUG_LEVEL >= 3
    printf("TESTEST\n");
    SDLGuiTK_list_destroy( surfaces );
#endif
}

MySDL_Surface *MySDL_Surface_new(char *name )
{
    MySDL_Surface *new_surface;
    new_surface = malloc( sizeof( struct MySDL_Surface ) );
    new_surface->object = NULL;
    //new_surface->object = PROT__object_new();
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
    /* if(w==0) w=1; */
    /* if(h==0) h=1; */
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

/*
 SDL_Surface * MySDL_CreateRGBASurface( SDL_Surface * dst,		\
				       int w, int h )
{
  SDL_Surface * tmp_srf;

  if( dst!=NULL ) {
    MySDL_FreeSurface( dst );
  }

  tmp_srf = SDL_CreateRGBSurface( 0,
				  w, h,
				  32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
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

  //dst = SDL_DisplayFormatAlpha( tmp_srf );
  *dst = *tmp_srf;
  MySDL_FreeSurface( tmp_srf );
  //SDL_SetAlpha( dst, SDL_RLEACCEL, 255 );

#if DEBUG_LEVEL >= 3
  surfaces_counter++;
    printf("<surfaces_counter:%d> MySDL_CreateRGBASurface()\n",surfaces_counter);
#endif
  return dst;
}
*/

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
    /*   SDL_Surface * dst; */

    if( dst->srf!=NULL ) {
        SDL_FreeSurface( dst->srf );
        dst->srf = NULL;
    }

    /*   dst = SDL_DisplayFormat( src ); */
    //dst = SDL_DisplayFormatAlpha( src );
    dst->srf = src->srf;
    //SDL_SetAlpha( dst, SDL_RLEACCEL, 255 ); /* SDL_SRCALPHA| */

    return dst;
}

static MySDL_Surface * MySDL_CopySurface_with_alpha( MySDL_Surface * dst, \
        MySDL_Surface * src, \
        Uint8 alpha )
{
    int i=0, j=0;
    Uint32 *bufp_s,*bufp_d;
    Uint8 r, g, b, a;
    /*   Uint32 color; */
    /*   SDL_Surface * tmp; */

    if( dst->srf!=NULL ) {
        SDL_FreeSurface( dst->srf );
        dst->srf = NULL;
    }

    /*   tmp = MySDL_CreateRGBSurface( NULL, src->w, src->h ); */
    /*   SDL_SetAlpha( tmp, SDL_RLEACCEL, 128 ); /\* SDL_SRCALPHA| *\/ */
    /*   color = SDL_MapRGBA( tmp->format, 0x00, 0x00, 0x00, 0x80 ); */
    /*   SDL_FillRect( tmp, NULL, color ); */
    /*   SDL_UpdateRect( tmp, 0, 0, tmp->w, tmp->h ); */

    /*   dst = SDL_DisplayFormatAlpha( src ); */
    /*   SDL_SetAlpha( dst, SDL_RLEACCEL, 128 ); /\* SDL_SRCALPHA| *\/ */
    /*   SDL_BlitSurface( tmp, NULL, dst, NULL ); */
    /*   color = SDL_MapRGBA( dst->format, 0x00, 0x00, 0x00, 0x80 ); */
    /*   SDL_FillRect( dst, NULL, color ); */
    /*   SDL_UpdateRect( dst, 0, 0, dst->w, dst->h ); */

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

    //SDL_UpdateRect( dst, 0, 0, 0, 0 );

    return dst;
}

/* SDL_Surface * MySDL_CopySurface_with_alpha1( SDL_Surface * dst, \ */
/* 					    SDL_Surface * src, \ */
/* 					    Uint8 alpha ) */
/* { */
/*   int i=0, j=0; */
/*   int offset; */
/*   Uint8 *s_point, *d_point; */

/*   if( dst!=NULL ) { */
/*     MySDL_FreeSurface( dst ); */
/*     dst = NULL; */
/*   } */

/*   dst = MySDL_CreateRGBSurface( dst, src->w, src->h ); */

/*   SDL_LockSurface( src ); */
/*   SDL_LockSurface( dst ); */

/*   s_point = (Uint8 *) src->pixels; */
/*   d_point = (Uint8 *) dst->pixels; */
/* /\*   s_point32 = (Uint16 *) src->pixels; *\/ */
/* /\*   d_point32 = (Uint16 *) dst->pixels; *\/ */
/* /\*   printf("SIZEOF Uint8: %d\nBytesPerPixel: %d\n", sizeof(Uint8), src->format->BytesPerPixel); *\/ */


/* /\*   if( src->format->BytesPerPixel==2 ) { *\/ */
/* /\* /\\*     SDL_SetAlpha( dst, SDL_RLEACCEL, 255 ); /\\\* SDL_SRCALPHA| *\\\/ *\\/ *\/ */
/* /\*     d_skip = dst->pitch-(dst->w*dst->format->BytesPerPixel); *\/ */
/* /\*     s_skip = src->pitch-(src->w*src->format->BytesPerPixel); *\/ */
/* /\*     for( j=0; j<src->h; j++ ) { *\/ */
/* /\*       for( i=0; i<src->w; i++ ) { *\/ */
/* /\* /\\* 	offset = (j*dst->pitch) + ((i)*src->format->BytesPerPixel); *\\/ *\/ */
/* /\* 	s_pixel = *s_point32++; *\/ */
/* /\* 	SDL_GetRGBA( s_pixel, src->format, &r, &g, &b, &a ); *\/ */
/* /\* /\\* 	printf("SRF: r:%d g:%d b:%d\n", r, g, b ); *\\/ *\/ */
/* /\* 	d_pixel = SDL_MapRGBA( dst->format, r, g, b, 128 ); *\/ */
/* /\* /\\* 	pixel = SDL_MapRGBA( dst->format, 0xc0, 0xc0, 0xc0, 128 ); *\\/ *\/ */
/* /\* 	*d_point32++ = d_pixel; *\/ */
/* /\*       } *\/ */
/* /\*       d_point32 += d_skip; *\/ */
/* /\*       s_point32 += s_skip; *\/ */
/* /\*     } *\/ */
/* /\*     SDL_SetAlpha( dst, SDL_SRCALPHA|SDL_RLEACCEL, 128 ); /\\* SDL_SRCALPHA| *\\/ *\/ */
/* /\*   } else { *\/ */
/*     for( j=0; j<src->h; j++ ) { */
/*       for( i=0; i<src->w; i++ ) { */
/* 	offset = (j*dst->pitch) + ((i)*src->format->BytesPerPixel); */
/* /\* 	memmove ( &d_point[offset], &alpha, sizeof(Uint8) ); *\/ */
/* 	memmove ( &d_point[offset], &s_point[offset], sizeof(Uint8) ); */
/* 	memmove ( &d_point[offset+1], &s_point[offset+1], sizeof(Uint8) ); */
/* 	memmove ( &d_point[offset+2], &s_point[offset+2], sizeof(Uint8) ); */
/* /\* 	memmove ( &d_point[offset+3], &s_point[offset+3], sizeof(Uint8) ); *\/ */
/* 	memmove ( &d_point[offset+3], &alpha, sizeof(Uint8) ); */
/*       } */
/*     } */
/*     SDL_SetAlpha( dst, SDL_RLEACCEL, 128 ); /\* SDL_SRCALPHA| *\/ */
/* /\*   } *\/ */

/*   SDL_UnlockSurface( dst ); */
/*   SDL_UnlockSurface( src ); */

/*   SDL_UpdateRect( dst, 0, 0, dst->w, dst->h ); */

/*   return dst; */
/* } */


/* static SDL_Surface * Window_Load_bgimage( SDLGuiTK_Window * window ) */
/* { */
/*   SDL_Surface *temp; */
/*   SDL_Surface *image; */

/*   temp = SDL_LoadBMP("icon.bmp"); */
/*   if ( temp == NULL ) { */
/*     printf( "Window_Load_bgimage(): not found\n" ); */
/*     return NULL; */
/*   } */
/*   image = MySDL_CreateRGBSurface( temp->w, temp->h ); */
/*   if ( image != NULL ) { */
/*     SDL_BlitSurface(temp, NULL, image, NULL); */
/*   } */
/*   SDL_FreeSurface(temp); */
/*   if ( image == NULL ) { */
/*     printf( "Window_Load_bgimage(): SDL_BlitSurface failed\n" ); */
/*     return NULL; */
/*   } */
/*   printf( "Window_Load_bgimage(): icon.bmp loaded\n" ); */
/*   return image; */
/* } */

#ifdef HAVE_GL_GL_H

void MySDL_GL_Enter2DMode()
{
    //SDL_Surface *screen = SDL_GetVideoSurface();
    //SDL_Surface *screen = SDL_GetWindowSurface(main_video->window);
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

    /* This allows alpha blending of 2D textures with the scene */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*         glViewport(0, 0, screen->w, screen->h); */

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    /* glOrtho(0.0, (GLdouble)screen->w, (GLdouble)screen->h, 0.0, 0.0, 1.0); */
    glOrtho(0.0, (GLdouble)w, (GLdouble)h, 0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void MySDL_GL_Leave2DMode()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    /*         glEnable(GL_DEPTH_TEST); */
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
    //Uint32 saved_flags;
    //Uint8  saved_alpha;

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

    /* Save the alpha blending attributes */
    //saved_flags = surface->flags;
    //saved_alpha = surface->format->alpha;
    //2if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
    //SDL_SetAlpha(surface, 0, 0);
    //2}

    /* Copy the surface into the GL texture image */
    area.x = 0;
    area.y = 0;
    area.w = surface->w;
    area.h = surface->h;
    SDL_BlitSurface(surface, &area, image, &area);

    /* Restore the alpha blending attributes */
    //2if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
    //SDL_SetAlpha(surface, saved_flags, saved_alpha);
    //2}

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
    SDL_FreeSurface(image); /* No longer needed */

    return texture;
}

#endif


/* CODE FROM mywm.c */

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
#ifdef HAVE_GL_GL_H
    new_surface2D->alpha = 1;
    /*   new_surface2D->alpha_flag=-0.01; */
    new_surface2D->texMinX = 0;
    new_surface2D->texMinY = 0;
    new_surface2D->texMaxX = 0;
    new_surface2D->texMaxY = 0;
    new_surface2D->texture[0] = 0;
#endif
    new_surface2D->w = 0;
    new_surface2D->h = 0;
    /*   new_surface2D->texture_flag = 0; */
    new_surface2D->srf = NULL;

    return new_surface2D;
}

void MySDL_surface2D_destroy( SDLGuiTK_Surface2D * surface2D )
{
#ifdef HAVE_GL_GL_H
    const GLuint *texture=&surface2D->texture[0];
#endif

    /*   MySDL_FreeSurface( surface2D->srf ); */
    surface2D->srf = NULL;
#ifdef HAVE_GL_GL_H
    glFinish();
    if( surface2D->texture[0] ) {
        glDeleteTextures( 1, texture );
    }
    if( glIsTexture(surface2D->texture[0]) ) {
        SDLGUITK_ERROR( "MySDL_surface2D_destroy(): texture seems not free\n" );
    } else {
        surface2D->texture[0] = 0;
    }
#endif
    SDLGuiTK_list_lock( surf2D_cache );
    SDLGuiTK_list_append( surf2D_cache, (SDLGuiTK_Object *) surface2D );
    /*   free( surface2D ); */
    SDLGuiTK_list_unlock( surf2D_cache );
}

#ifdef HAVE_GL_GL_H

void MySDL_surface2D_update( SDLGuiTK_Surface2D * surface2D, \
                             SDL_Surface *srf )
{
    GLfloat texcoord[4];
    /*     GLfloat active_flag=-0.01; */

    /* Load the image (could use SDL_image library here) */
    /* image = SDL_LoadBMP(LOGO_FILE); */
    if ( srf == NULL ) {
        return;
    }
    surface2D->srf = srf;

    /*     if( surface2D->alpha>=0.5 ) { surface2D->alpha_flag = -0.01; }; */
    /*     if( surface2D->alpha<=0.25 ) { surface2D->alpha_flag = 0.01; }; */
    /*     surface2D->alpha+=surface2D->alpha_flag; */

    surface2D->w = srf->w;
    surface2D->h = srf->h;

    /*     if( wmwidget_2D->texture ) { */
    /*       glDeleteTextures( 1, &wmwidget_2D->texture ); */
    /*     } */
    /* Convert the image into an OpenGL texture */
    surface2D->texture[0] = \
                            MySDL_GL_LoadTexture( srf, texcoord, &surface2D->texture[0] );

    /* Make texture coordinates easy to understand */
    surface2D->texMinX = texcoord[0];
    surface2D->texMinY = texcoord[1];
    surface2D->texMaxX = texcoord[2];
    surface2D->texMaxY = texcoord[3];

    /* We don't need the original image anymore */
    /* SDL_FreeSurface(image); */

    /* Make sure that the texture conversion is okay */
    if ( ! surface2D->texture[0] ) {
        return;
    }
}


void MySDL_surface2D_blitsurface( SDLGuiTK_Surface2D * surface2D, \
                                  int x, int y )
{
    /* Show the image on the screen */
    /*   MySDL_GL_Enter2DMode(); */


    glEnable( GL_BLEND );
    glColor4f( 1.0f, 1.0f, 1.0f, surface2D->alpha);

    glBindTexture( GL_TEXTURE_2D, surface2D->texture[0] );
    glBegin( GL_TRIANGLE_STRIP );
    glTexCoord2f( surface2D->texMinX, \
                  surface2D->texMinY );
    glVertex2i( x, y );
    glTexCoord2f( surface2D->texMaxX, surface2D->texMinY );
    glVertex2i( x+surface2D->w, y );
    glTexCoord2f( surface2D->texMinX, surface2D->texMaxY );
    glVertex2i( x, y+surface2D->h );
    glTexCoord2f( surface2D->texMaxX, surface2D->texMaxY );
    glVertex2i( x+surface2D->w, y+surface2D->h );
    glEnd();

    glDisable( GL_BLEND );


    /*   MySDL_GL_Leave2DMode(); */


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

#endif

void MySDL_UpdateWindowSurface()
{
    //SDL_UpdateWindowSurface( main_video->window );
}

