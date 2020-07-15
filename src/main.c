/* 
   sdl_guitk - GUI toolkit designed for SDL environnements (GTK-style).

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


#include <SDL2/SDL_guitk.h>

#include "getopt.h"
/* #ifndef WIN32 */
/* #include "../system.h" */
/* #endif */

#ifdef WIN32
#define VERSION "0.9"
#endif

#include "debug.h"
#include "mysdl.h"
#include "myttf.h"
#include "mywm.h"
#include "mycursor.h"
#include "theme_prot.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "signal.h"
#include "context_prot.h"

static int           main_loop=0;
static SDL_mutex   * main_loop_mutex=NULL;

#define EXIT_FAILURE 1
#ifndef WIN32
char *xmalloc ();
char *xstrdup ();
#endif
static void usage (int status);

/* The name the program was run with, stripped of any leading path. */
char *program_name;

/* getopt_long return codes */
enum {DUMMY_CODE=129,
      WIDTH_CODE,
      HEIGHT_CODE
};

/* Option flags and variables */

static char* width;
static char* height;

static struct option const long_options[] =
{
  {"help", no_argument, 0, 'h'},
  {"version", no_argument, 0, 'V'},
  {"fullscreen", no_argument, 0, 'f'},
#ifndef WIN32
  {"width", required_argument, 0, WIDTH_CODE},
  {"height", required_argument, 0, HEIGHT_CODE},
#else
  {"width", required_argument, 0, 'W'},
  {"height", required_argument, 0, 'H'},
#endif
  {NULL, 0, NULL, 0}
};



static void Init_internals()
{
  main_loop_mutex = SDL_CreateMutex();

    MySDL_Surface_init();
  PROT__theme_init();
  PROT__widget_init();
  PROT__object_init();
  PROT__signal_init();
}

static void Quit_internals()
{
  PROT__context_quit();

  PROT__signal_uninit();
  PROT__object_uninit();
  PROT__widget_uninit();
  PROT__theme_uninit();

  PROT__context_uninit();
    MySDL_Surface_uninit();

  SDL_DestroyMutex( main_loop_mutex );
}



static void Main_loop()
{
  SDL_Event event;

  SDL_mutexP( main_loop_mutex );
  main_loop = 1;
  while( main_loop==1 ) {
    SDL_mutexV( main_loop_mutex );

    SDLGuiTK_context_check();
    MySDL_MainSurface_clean();
    SDLGuiTK_context_blitsurfaces();
    MySDL_SwapBuffers();

    while( ( SDL_PollEvent(&event))==1 ) {
      SDLGuiTK_context_pushevent( &event );
    }

    SDL_mutexP( main_loop_mutex );

    SDL_Delay( 10 );
  }
  SDL_mutexV( main_loop_mutex );
}


static int decode_switches (int argc, char **argv);

void SDLGuiTK_init( int argc, char **argv )
{
  int i;
  SDLGUITK_Video * video;

  program_name = argv[0];

  MySDL_Init();
  MyTTF_Init();
  MyWM_Init();

  Init_internals();
  i = decode_switches (argc, argv);
  video = MySDL_MainSurface_create();
  MyCursor_Init();
  PROT__context_new( SDL_GetWindowSurface(video->window), SDLGUITK_CONTEXT_MODE_SELF );
  MySDL_MainSurface_clean();
  MySDL_SwapBuffers();
  
}

void SDLGuiTK_main()
{
  Main_loop();

  MyCursor_Uninit();
/*   MySDL_MainSurface_destroy(); */
  Quit_internals();

  MyWM_Uninit();
  MyTTF_Uninit();
  MySDL_Uninit();
}

void SDLGuiTK_init_with_window( SDL_Window * window, SDL_Renderer * renderer )
{
  SDL_mutexP( main_loop_mutex );
  main_loop = 2;
  SDL_mutexV( main_loop_mutex );

  PROT__debug_init();

  MySDL_MainSurface_set( window, renderer );

  MyTTF_Init();
  MyWM_Init();

  Init_internals();
  PROT__context_new( SDL_GetWindowSurface( window ), SDLGUITK_CONTEXT_MODE_SLAVE );
  
  MyCursor_Init();
}

void SDLGuiTK_main_quit()
{
  SDLGUITK_LOG( " SDLGuiTK_main_quit()\n" );

  SDL_mutexP( main_loop_mutex );
  if( main_loop==1 ) {
    main_loop = 3;
  }
  if( main_loop==2 ) {
    SDL_mutexV( main_loop_mutex );

    if( current_context->type==SDLGUITK_CONTEXT_MODE_SLAVE ) {
      MyCursor_Uninit();
      MyWM_Uninit();
      MyTTF_Uninit();
      Quit_internals();
      PROT__debug_stop();
    } else {
      SDL_mutexP( current_context->mutex );
      SDL_mutexV( current_context->mutex );
    }
    return;
  }
  main_loop = 0;
  SDL_mutexV( main_loop_mutex );
}


static int
decode_switches (int argc, char **argv)
{
  int c;

#ifndef WIN32
  while ((c = getopt_long (argc, argv, 
			   "h"	/* help */
			   "V"	/* version */
			   "f"	/* version */
			   "w"	/* version */,
			   long_options, (int *) 0)) != EOF)
#else
  while ((c = getopt (argc, argv, 
			   "h"	/* help */
			   "V"	/* version */
			   "f"	/* version */
			   "W"	/* version */
			   "H"	/* version */,
		      )) != EOF)
#endif
    {
      switch (c)
	{
	case 'V':
	  //printf ("libsdl_guitk %s\n", VERSION);
	  exit (0);

	case 'h':
	  usage (0);

	case 'f':
	  MySDL_ModeFullScreen( SDL_TRUE );
	  break;
#ifndef WIN32
	case WIDTH_CODE:
	  width = xstrdup(optarg);
	  MySDL_ModeSetWidth( atoi(width) );
	  break;
	case HEIGHT_CODE:
	  height = xstrdup(optarg);
	  MySDL_ModeSetHeight( atoi(height) );
	  break;
/* #else */
/* 	case 'W': */
/* 	  width = xstrdup(optarg); */
/* 	  MySDL_ModeSetWidth( atoi(width) ); */
/* 	  break; */
/* 	case 'H': */
/* 	  height = xstrdup(optarg); */
/* 	  MySDL_ModeSetHeight( atoi(height) ); */
/* 	  break; */
#endif
	default:
	  usage (EXIT_FAILURE);
	}
    }

  return optind;
}


static void
usage (int status)
{
  printf ("%s build on sdl_guitk\n\
SDL2_GuiTK - GUI toolkit designed for SDL environnements.\n", program_name);
  printf ("Usage: %s [OPTION]...\n", program_name);
  printf ("\
Options:\n\
  -f, --fullscreen           fullscreen output\n\
  --width=WIDTH              set screen width\n\
  --height=HEIGHT            set screen height\n\
\n\
  -h, --help                 display this help and exit\n\
  -V, --version              output version information and exit\n\
");
  exit (status);
}
