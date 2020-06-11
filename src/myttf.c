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

#include <SDL2/SDL_guitk.h>

#ifdef HAVE_SDL_SDL_TTF_H
#include <SDL2/SDL_ttf.h>
#endif
#include <SDL2/SDL_ttf.h>

#ifdef HAVE_UNICODE_H
#include <unicode.h>
#endif

#include "ctype.h"

#include "debug.h"
#include "mysdl.h"
#include "myttf.h"
#include "theme_prot.h"


static int font_mediumsize=12;
#ifdef HAVE_UNICODE_H
static unicode_iconv_t u2l, l2u;
#endif

void MyTTF_Init()
{
  //2SDL_EnableUNICODE(1);
  //2SDL_EnableKeyRepeat( 500, 50 );
  TTF_Init();
  font_mediumsize = 16;
#ifdef HAVE_UNICODE_H
  unicode_init ();
  if ((u2l = unicode_iconv_open("UTF-8", "UCS2")) == (unicode_iconv_t) -1) {
    SDLGUITK_ERROR( "Conversion from UNICODE to UTF-8 not available\n" );
  }
  if ((l2u = unicode_iconv_open("UCS2", "UTF-8")) == (unicode_iconv_t) -1) {
    SDLGUITK_ERROR( "Conversion from UTF-8 to UNICODE not available\n" );
  }
#endif
}

void MyTTF_Uninit()
{
#ifdef HAVE_UNICODE_H
  unicode_iconv_close (u2l);
  unicode_iconv_close (l2u);
#endif
}


int MyTTF_Load( char *font_file )
{
  TTF_Font *font;

  if( (font=TTF_OpenFont(font_file,10))==NULL ) {

    SDLGUITK_ERROR(font_file );
    SDLGUITK_ERROR( "   ... not found!\n" );
    return -1;
    
  }
  

  return 0;
}

/* SDL_Surface * MyTTF_Render_Shaded( SDL_Surface * render_text, \ */
/* 				   char *text, int size, Uint8 style ) */
/* { */
/*   char tmpstr[512]; */
/*   TTF_Font *font; */
/*   SDLGuiTK_Theme * theme; */

/*   if( render_text!=NULL ) { */
/*     SDL_FreeSurface( render_text ); */
/*     render_text = NULL; */
/*   } */

/*   theme = PROT__theme_get_and_lock(); */
/*   font = TTF_OpenFont( theme->font_file, size ); */

/*   if ( font==NULL ) { */
/*     sprintf( tmpstr, "Couldn't load %d pt font from %s: %s\n", */
/* 	     size, theme->font_file, SDL_GetError()); */
/*     SDLGUITK_ERROR( tmpstr ); */
/*     exit(2); */
/*   } */
/*   TTF_SetFontStyle( font, style ); */
/*   render_text = TTF_RenderText_Shaded( font, text, \ */
/* 				       theme->ftcolor, theme->bgcolor ); */

/*   PROT__theme_unlock( theme ); */
/*   TTF_CloseFont(font); */

/*   return render_text; */
/* } */

/* SDL_Surface * MyTTF_Render_Solid( SDL_Surface * render_text, \ */
/* 				  char *text, int size, \ */
/* 				  Uint8 style, SDL_Color color ) */
/* { */
/*   char tmpstr[512]; */
/*   TTF_Font *font; */
/*   SDLGuiTK_Theme * theme; */

/*   if( render_text!=NULL ) { */
/*     SDL_FreeSurface( render_text ); */
/*     render_text = NULL; */
/*   } */

/*   theme = PROT__theme_get_and_lock(); */
/*   font = TTF_OpenFont( theme->font_file, size ); */

/*   if ( font==NULL ) { */
/*     sprintf( tmpstr, "Couldn't load %d pt font from %s: %s\n", */
/* 	     size, theme->font_file, SDL_GetError()); */
/*     SDLGUITK_ERROR( tmpstr ); */
/*     exit(2); */
/*   } */
/*   TTF_SetFontStyle( font, style ); */
/*   render_text = TTF_RenderText_Solid( font, text, \ */
/* 				      color ); */

/*   PROT__theme_unlock( theme ); */
/*   TTF_CloseFont(font); */

/*   return render_text; */
/* } */

SDL_Surface * MyTTF_Render_Blended( SDL_Surface * render_text, \
				    char *text, int size, \
				    Uint8 style, SDL_Color color )
{
/*   SDL_Surface * render_tmp; */
  char tmpstr[512];
  TTF_Font *font;
  SDLGuiTK_Theme * theme;

  if( render_text!=NULL ) {
    SDL_FreeSurface( render_text );
    render_text = NULL;
  }

  theme = PROT__theme_get_and_lock();
  font = TTF_OpenFont( theme->font_file, size );

  if ( font==NULL ) {
    sprintf( tmpstr, "Couldn't load %d pt font from %s: %s\n",
	     size, theme->font_file, SDL_GetError());
    SDLGUITK_ERROR( tmpstr );
    exit(2);
  }
  TTF_SetFontStyle( font, style );
  render_text = TTF_RenderUTF8_Blended( font, text, \
				       color );
/*   render_text = SDL_DisplayFormatAlpha( render_tmp ); */
/*   MySDL_FreeSurface( render_tmp ); */
  PROT__theme_unlock( theme );
  TTF_CloseFont(font);

  return render_text;
}

SDL_Surface * MyTTF_Render_BlendedU( SDL_Surface * render_text, \
				     Uint16 *text, int size, \
				     Uint8 style, SDL_Color color )
{
  SDL_Surface * render_tmp;
  char tmpstr[512];
  TTF_Font *font;
  SDLGuiTK_Theme * theme;

  if( render_text!=NULL ) {
    SDL_FreeSurface( render_text );
    render_text = NULL;
  }

  theme = PROT__theme_get_and_lock();
  font = TTF_OpenFont( theme->font_file, size );

  if ( font==NULL ) {
    sprintf( tmpstr, "Couldn't load %d pt font from %s: %s\n",
	     size, theme->font_file, SDL_GetError());
    SDLGUITK_ERROR( tmpstr );
    exit(2);
  }
  TTF_SetFontStyle( font, style );
  render_tmp = TTF_RenderUNICODE_Blended( font, text, \
					  color );
  //render_text = SDL_DisplayFormatAlpha( render_tmp );
  render_text = render_tmp;
/*   render_text = render_tmp; */
  SDL_FreeSurface( render_tmp );
  PROT__theme_unlock( theme );
  TTF_CloseFont(font);

  return render_text;
}



SDL_Surface * MyTTF_Render_Solid_Block( SDL_Surface * render_text, \
					char *text, int size, \
					Uint8 style, \
					SDL_Color color , \
					int maxcharbyline )
{
#if DEBUG_LEVEL >= 2
  //char tmpstr[512];
#endif
  int finished=0;
  //int text_lenght;
  int index=0;
  int current_line=0, current_pos=0;
  int last_space_index=0, last_space_pos=0;
  SDL_Surface * line_srf[32];
  SDL_Rect      line_area[32];
  char        * line_txt;
  int max_w=0, current_y=0;
  int i;
  SDLGuiTK_Theme * theme;

  if( render_text!=NULL ) {
    SDL_FreeSurface( render_text );
    render_text = NULL;
  }

  //text_lenght = strlen( text );
  line_txt = calloc( maxcharbyline+2, sizeof( char ) );
  while( finished==0 ) {

    if( current_pos!=maxcharbyline ) {
      if( text[index]==' ' ) {
	last_space_index = index; last_space_pos = current_pos;
      }
      line_txt[current_pos] = text[index];
    } else {
      if( text[index]==' ' ) {
	last_space_index = index; last_space_pos = current_pos;
      } else {
	index = last_space_index;
      }
      current_pos = -1;
      line_txt[last_space_pos] = '\0';
      last_space_pos = 0;
/*       sprintf( tmpstr, "Render_Solid_Block: line%d %s\n", \ */
/* 	       current_line, line_txt ); */
/*       SDLGUITK_LOG( tmpstr ); */
      line_srf[current_line] = MyTTF_Render_Blended( NULL, \
						     line_txt, size, \
						     style, \
						     color );
      line_area[current_line].x = 0;
      line_area[current_line].y = current_y;
      line_area[current_line].w = line_srf[current_line]->w;
      line_area[current_line].h = line_srf[current_line]->h;
      if( max_w<=line_srf[current_line]->w ) {
	max_w = line_srf[current_line]->w + 2;
      }
      current_y += line_area[current_line].h;
      current_line++;
    }

    index++;
    current_pos++;
    if( text[index]=='\0' ) {
      finished = 1;
    }
  }

  line_txt[current_pos] = '\0';
/*   sprintf( tmpstr, "Render_Solid_Block: last%d %s\n", \ */
/* 	   current_line, line_txt ); */
/*   SDLGUITK_LOG( tmpstr ); */
  line_srf[current_line] = MyTTF_Render_Blended( NULL, \
						 line_txt, size, \
						 style, \
						 color );
  line_area[current_line].x = 0;
  line_area[current_line].y = current_y;
  line_area[current_line].w = line_srf[current_line]->w;
  line_area[current_line].h = line_srf[current_line]->h;
  if( max_w<=line_srf[current_line]->w ) {
    max_w = line_srf[current_line]->w + 2;
  }
  current_y += line_area[current_line].h;

  theme = PROT__theme_get_and_lock();
  /* render_text = MySDL_CreateRGBSurface_WithColor( render_text, \ */
		/* 				  max_w, current_y, \ */
		/* 				  theme->bgcolor ); */
    render_text = SDL_CreateRGBSurface( 0, max_w, current_y, 32,
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

    PROT__theme_unlock( theme );
  for( i=0; i<=current_line; i++ ) {
    SDL_BlitSurface( line_srf[i], NULL, \
		     render_text, &line_area[i] );
    //2SDL_UpdateRects( render_text, 1, &line_area[i] );
	//SDL_UpdateWindowSurface( line_srf[i] );
	SDL_FreeSurface( line_srf[i] );
  }
  free( line_txt );

  return render_text;
}



#ifndef HAVE_UNICODE_H

/* Skip over a utf8 character.
   Note that P cannot be const because then there is no useful return
   type for this function.  */
char *
unicode_next_utf8 (const char *p)
{
  if (*p)
    {
      for (++p; (*p & 0xc0) == 0x80; ++p)
        ;
    }
  return (char *) p;
}

/* Return length of a UTF8 string.  */
int
unicode_strlen (const char *p, int max)
{
  int len = 0;
  const char *start = p;
  /* special case for the empty string */
  if (!*p) 
    return 0;
  /* Note that the test here and the test in the loop differ subtly.
     In the loop we want to see if we've passed the maximum limit --
     for instance if the buffer ends mid-character.  Here at the top
     of the loop we want to see if we've just reached the last byte.  */
  while (max < 0 || p - start < max)
    {
      p = unicode_next_utf8 (p);
      ++len;
      if (! *p || (max > 0 && p - start > max))
        break;
    }
  return len;
}

#else

char * MyUnicode_U2L( const char * src )
{
  char *t_result, result[MyTTF_MAX_CHARS];
  unicode_char_t *from;
  char *to;
  int from_size, to_size;

  t_result = calloc( MyTTF_MAX_CHARS, sizeof( char ) );
/*   *t_result = 0; */
  from = (unicode_char_t *)src;
  from_size = sizeof( from );
  to = result;
  to_size = sizeof( result );
  if( unicode_iconv( u2l, \
		     (const char **) &from, &from_size, \
		     &to, &to_size) == -1 ) {
    SDLGUITK_LOG("MyUnicode_U2L(): Conversion failed\n" );
  }
  *to = 0;
  strcpy( t_result, result );
/*   printf ("_U2L: from_size=%d to_size=%d (%s->%s)\n", from_size, to_size, src, (char *)t_result); */
  return t_result;
}

#endif


char * MyUnicode_getUTF8( Uint16 * key_unicode )
{
#ifndef HAVE_UNICODE_H
  char *ch;

  ch = calloc( 1, sizeof( char ) );
  if ( (*key_unicode & 0xFF80) == 0 ) {
    *ch = *key_unicode & 0x7F;
  } else {
    SDLGUITK_LOG("PROT_editable_getUTF8(): An International character.\n");
    return (char *) NULL;
  }
  if( !isprint(*ch) ) {
    SDLGUITK_LOG("PROT_editable_getUTF8(): Non printable character.\n");
    return (char *) NULL;
  }
#else
  char * ch;
  Uint16 unitmp[3];
  if( !unicode_isprint(*key_unicode) ) {
    SDLGUITK_LOG("PROT_editable_getUTF8(): Non printable character.\n");
    return (char *) NULL;
  }

  unitmp[0] = *key_unicode; unitmp[1] = 0;
  ch = MyUnicode_U2L( (char *)unitmp );
#endif

  return ch;
}


