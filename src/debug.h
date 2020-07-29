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


/* Debug libs are empty with DEBUG_LEVEL=0 */
/* Call debug_init before all others init functions */

#pragma once

#include <SDL2/SDL_guitk.h>

//#define DEBUG_LEVEL 2

#if DEBUG_LEVEL >= 2
#define SDLGUITK_LOG(log_message) \
        ( PROT__debug_log("L", log_message) )
#else
#define SDLGUITK_LOG(log_message)
#endif

#if DEBUG_LEVEL >= 1
#define SDLGUITK_ERROR(error_message) \
        ( PROT__debug_log("E", error_message) )
#define SDLGUITK_ERROR2(error_message) \
        ( PROT__debug_log("", error_message) )
#define SDLGUITK_OUTPUT(error_message) \
        ( PROT__debug_log("O", error_message) )
#define SDLGUITK_THREAD_REF(name) \
        ( PROT__debug_threads_ref( name ) )
#define SDLGUITK_THREAD_UNREF() \
        ( PROT__debug_threads_unref() )
#else
#define SDLGUITK_ERROR(error_message)
#define SDLGUITK_ERROR2(error_message)
#define SDLGUITK_OUTPUT(error_message)
#define SDLGUITK_THREAD_REF(name)
#define SDLGUITK_THREAD_UNREF()
#endif


/* Init the debug libs */
extern
void PROT__debug_init();
/* Stop the debug libs, and write counts and results outputs */
extern
void PROT__debug_stop();


/* Called by debug lib's clients to simply write */
extern
void PROT__debug_log( char *flag, char *log_message );


/* Thread referencement functions: */
/* association between pid and debug name of the thread */
#define MAX_THREADS_DBG 32
extern
void PROT__debug_threads_ref( char *name );
extern
void PROT__debug_threads_unref();


