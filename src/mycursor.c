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

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <SDL2/SDL.h>

#include "debug.h"
#include "mycursor.h"

/* XPM */
static const char *arrow[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  " .                              ",
  ".X.                             ",
  ".XX.                            ",
  ".XXX.                           ",
  ".XXXX.                          ",
  ".XXXXX.                         ",
  ".XXXXXX.                        ",
  ".XXXXXXX.                       ",
  ".XXXXXXXX.                      ",
  ".XXXXXXXXX.                     ",
  ".XXXXXXXXXX.                    ",
  ".XXXXXXXXXXX.                   ",
  ".XXXXX......                    ",
  ".XXXX.                          ",
  ".XXX.                           ",
  ".XX.                            ",
  ".X.                             ",
  " .                              ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,0"
};

static const char *barre[] = {
  /* width height num_colors chars_per_pixel */
  "    32    32        3            1",
  /* colors */
  "X c #000000",
  ". c #ffffff",
  "  c None",
  /* pixels */
  " ...                            ",
  ".XXX.                           ",
  " .X.                            ",
  " .X.                            ",
  " .X.                            ",
  " .X.                            ",
  " .X.                            ",
  " .X.                            ",
  " .X. ..                         ",
  " .X..XX.                        ",
  " .X.X..X.                       ",
  " .X.XXXX.                       ",
  " .X.X..X.                       ",
  " .X.X..X.                       ",
  " .X..  .                        ",
  ".XXX.                           ",
  " ...                            ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "0,7"
};

static SDL_Cursor *Load_cursor(const char *image[])
{
  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int hot_x, hot_y;

  i = -1;
  for ( row=0; row<32; ++row ) {
    for ( col=0; col<32; ++col ) {
      if ( col % 8 ) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[4+row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }
  sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
/*   printf( "test: hot spot(%d %d)\n", hot_x, hot_y ); */
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}


static SDL_Cursor * cursor_x;
static SDL_Cursor * cursor_default;
static SDL_Cursor * cursor_text;


void MyCursor_Init()
{
  cursor_x = SDL_GetCursor();
  cursor_default = Load_cursor( arrow );
  cursor_text = Load_cursor( barre );
/*   SDL_SetCursor( cursor_default ); */
  SDLGUITK_LOG( "MyCursor_Init() OK\n" );
}

void MyCursor_Uninit()
{
  SDLGUITK_LOG( "MyCursor_Uninit() OK\n" );
  SDL_FreeCursor( cursor_default );
  SDL_FreeCursor( cursor_text );
}


void MyCursor_Set( int cursor )
{
  switch( cursor ) {
  case SDLGUITK_CURSOR_DEFAULT:
/*     printf("TEST A\n"); */
    SDL_SetCursor( cursor_default );
/*     SDL_SetCursor( Load_cursor( arrow ) ); */
/*     printf( " TESTB1: %s\n", SDL_GetError() ); */
    break;
  case SDLGUITK_CURSOR_TEXT:
    SDL_SetCursor( cursor_text );
/*     SDL_SetCursor( Load_cursor( barre ) ); */
/*     printf( " TESTB2: %s\n", SDL_GetError() ); */
    break;
  default:
    SDL_SetCursor( cursor_default );
/*     SDL_SetCursor( Load_cursor( arrow ) ); */
/*     printf( " TESTB3: %s\n", SDL_GetError() ); */
    break;
  }
}

void MyCursor_Unset()
{
  SDL_SetCursor( cursor_x );
}

