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

#ifdef HAVE_GL_GL_H
#ifdef WIN32
#include <windows.h>
#endif
//2#include <SDL/SDL_opengl.h>
#endif


#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "mysdl.h"
#include "myttf.h"
/* #include "context_prot.h" */
#include "theme_prot.h"



static SDL_mutex      * theme_mutex;
static SDLGuiTK_Theme * default_theme=NULL;


static SDLGuiTK_Theme * Theme_create()
{
  SDLGuiTK_Theme * new_theme;

  new_theme = malloc( sizeof( struct SDLGuiTK_Theme ) );
  new_theme->bgcolor.r=0x00;
  new_theme->bgcolor.g=0x00;
  new_theme->bgcolor.b=0x00;
  new_theme->bgcolor.a=0xff;

  new_theme->bdcolor.r=0xc0;
  new_theme->bdcolor.g=0xc0;
  new_theme->bdcolor.b=0xc0;
  new_theme->bdcolor.a=0xff;

  new_theme->ftcolor.r=0xff;
  new_theme->ftcolor.g=0xff;
  new_theme->ftcolor.b=0xff;
  new_theme->ftcolor.a=0xff;

  new_theme->wgcolor.r=0x20;
  new_theme->wgcolor.g=0x20;
  new_theme->wgcolor.b=0x20;
  new_theme->wgcolor.a=0xff;

/*   new_theme->ref_srf=MySDL_CreateRGBSurface( NULL, 1, 1 ); */

#ifdef WIN32
  strcpy( new_theme->font_file, "font.ttf" );
#else
  strcpy( new_theme->font_file, "/home/font.ttf" );
#endif
  new_theme->font_size = 14;

  if( MyTTF_Load(new_theme->font_file)!=0 ) {

    SDLGUITK_ERROR( "Theme_create(): TODO\n" );

#ifdef WIN32
    strcpy( new_theme->font_file, "C:\\windows\\fonts\\DejaVuSans.ttf" );
#else
    strcpy( new_theme->font_file, "/usr/share/fonts/truetype/freefont/FreeSans.ttf" );
#endif
    new_theme->font_size = 14;

    if( MyTTF_Load(new_theme->font_file)!=0 ) {
      exit(1);
    }
  }

  return new_theme;
}

static void Theme_default()
{
/*   FILE *filet; */
  SDL_mutexP( theme_mutex );
  default_theme = Theme_create();
/*   filet = fopen( default_theme->font_file, "r" ); */
/*   if( filet==NULL ) { */
/*     SDLGUITK_ERROR( default_theme->font_file ); */
/*     SDLGUITK_ERROR( "Theme_default(): font file not found!" ); */
/*     exit(0); */
/*   } */
  SDL_mutexV( theme_mutex );
}


void PROT__theme_init()
{
  theme_mutex = SDL_CreateMutex();
  Theme_default();
}

void PROT__theme_uninit()
{
  SDL_DestroyMutex( theme_mutex );
}


SDLGuiTK_Theme * PROT__theme_get_and_lock()
{
  SDL_mutexP( theme_mutex );
  return default_theme;
}

void PROT__theme_unlock( SDLGuiTK_Theme * theme )
{
  SDL_mutexV( theme_mutex );
}


