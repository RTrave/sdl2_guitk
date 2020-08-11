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


/* IN THE FUTURE, CAN BE SWITCHED TO  A MyPango LAYER in mypango.c */
/* TO GIVE A SUPPORT TO "Markup Langage" */

#pragma once

#include <SDL2/SDL_guitk.h>

extern void MyTTF_Init();
extern void MyTTF_Uninit();


extern 
int MyTTF_Load( char *font_file );


#define MyTTF_STYLE_NORMAL        0x00
#define MyTTF_STYLE_BOLD          0x01
#define MyTTF_STYLE_ITALIC        0x02
#define MyTTF_STYLE_UNDERLINE     0x04


/* extern SDL_Surface * MyTTF_Render_Shaded( SDL_Surface * render_text, \ */
/* 					  char *text, int size, \ */
/* 					  Uint8 style ); */

/* extern SDL_Surface * MyTTF_Render_Solid(  SDL_Surface * render_text, \ */
/* 					  char *text, int size, \ */
/* 					  Uint8 style, \ */
/* 					  SDL_Color color ); */

extern 
SDL_Surface * MyTTF_Render_Blended( SDL_Surface * render_text, \
				    char *text, int size, \
				    Uint8 style, SDL_Color color );
extern
SDL_Surface * MyTTF_Render_Solid_Block( SDL_Surface * render_text, \
					char *text, int size, \
					Uint8 style, \
					SDL_Color color ,\
					int maxcharbyline );


#define MyTTF_MAX_CHARS ((int) 256)

#ifndef HAVE_UNICODE_H /* Useful functions from libunicode */

/* Skip over a utf8 character.
   Note that P cannot be const because then there is no useful return
   type for this function.  */
extern 
char * unicode_next_utf8 (const char *p);
/* Return length of a UTF8 string.  */
extern 
int    unicode_strlen (const char *p, int max);

#else

#include <unicode.h>
extern 
char * MyUnicode_U2L( const char * src );

#endif


extern 
char * MyUnicode_getUTF8( Uint16 * key_unicode );

