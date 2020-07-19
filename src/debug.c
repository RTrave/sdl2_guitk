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

/* #if DEBUG_LEVEL >= 1 */

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

#include "debug.h"


static SDL_mutex * debug_mutex=NULL;

static int  current_id;
static int  threads_pid[MAX_THREADS_DBG];
static char threads_names[8][MAX_THREADS_DBG];


void PROT__debug_init()
{
  int i;

#if DEBUG_LEVEL >= 1
  printf("SGTK LOG INIT\n");

  debug_mutex = SDL_CreateMutex();

  current_id = 0;
  for( i=0; i<MAX_THREADS_DBG; i++ ) {
    threads_pid[i] = 0;
    strcpy( &threads_names[0][i], "" );
  }
  SDLGUITK_THREAD_REF("main");
#endif
}

void PROT__debug_stop()
{
#if DEBUG_LEVEL >= 1
  SDLGUITK_THREAD_UNREF();
  SDL_DestroyMutex( debug_mutex );

  printf("SGTK LOG UNINIT\n");
#endif
}


void PROT__debug_log( char *flag, char *log_message )
{
  int i, pid;

  SDL_mutexP( debug_mutex );

  pid = (int) SDL_ThreadID(  );
  i = 0;
  while( i!=MAX_THREADS_DBG && pid!=threads_pid[i] ) { i++; };
  if( i==MAX_THREADS_DBG && strcmp(flag, "")!=0) {
      printf( "[SGTK-%d]", pid );
  } else if (strcmp(flag, "")!=0) {
      printf( "[SGTK-%s]", &threads_names[i][0] );
  }
  if( log_message!=NULL ) {
    if(strcmp(flag, "")!=0)
      printf( "\t%s %s", flag, log_message);
    else
      printf( "%s\n", log_message);
  }else{
    printf( "\t%s UNCOMMENTED\n", flag );
  }

  SDL_mutexV( debug_mutex );
}


void PROT__debug_threads_ref( char *name )
{
  char tmpstr[64];

  SDL_mutexP( debug_mutex );

  threads_pid[current_id] = (int) SDL_ThreadID();
  strcpy( &threads_names[current_id][0], name );
  current_id++;

  SDL_mutexV( debug_mutex );

  sprintf( tmpstr, "New thread referenced: %s\n", name );
  PROT__debug_log( "T", tmpstr );
}

void PROT__debug_threads_unref()
{
  int i, pid;

  PROT__debug_log( "T", "Thread unreferenced\n" );

  SDL_mutexP( debug_mutex );

  pid = (int) SDL_ThreadID();
  i = 0;
  while( i!=MAX_THREADS_DBG&&pid!=threads_pid[i] ) { i++; };
  if( pid==threads_pid[i] )
    {
      threads_pid[i] = 0;
      strcpy( &threads_names[current_id][0], "" );
    }

  SDL_mutexV( debug_mutex );
}


