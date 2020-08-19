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

#include "mysdl.h"
#include "debug.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "signal.h"


static int         current_id=1;
static SDL_mutex * current_id_mutex=NULL;


static SDLGuiTK_Object * Object_create()
{
  SDLGuiTK_Object * new_object;

  new_object = malloc( sizeof( struct SDLGuiTK_Object ) );
/*   new_object->mutex = SDL_CreateMutex(); */

  SDL_mutexP( current_id_mutex );
  new_object->id = current_id++;
  SDL_mutexV( current_id_mutex );

  new_object->widget = NULL;
  new_object->adjustment = NULL;
  //new_object->tooltips = NULL;

  strcpy( new_object->name, "noname" );

  new_object->signalhandler = PROT__signalhandler_new();

  return new_object;
}

static void Object_destroy( SDLGuiTK_Object * object )
{
  PROT__signalhandler_destroy( object->signalhandler );
/*   SDL_DestroyMutex( object->mutex ); */
  free( object );
}

void PROT__object_init()
{
  if( current_id_mutex==NULL ) {
    current_id_mutex = SDL_CreateMutex();
  }
  SDL_mutexP( current_id_mutex );
  current_id = 1;
  SDL_mutexV( current_id_mutex );
}

void PROT__object_uninit()
{
  SDL_DestroyMutex( current_id_mutex );
  current_id_mutex = NULL;
  current_id = 1;
}


SDLGuiTK_Object * PROT__object_new()
{
  SDLGuiTK_Object * object;

  object = Object_create();

  return object;
}

void PROT__object_destroy( SDLGuiTK_Object * object )
{
  Object_destroy( object );
}


SDLGuiTK_Object * SDLGuiTK_OBJECT( SDLGuiTK_Widget *widget )
{
  return widget->object;
}


void SDLGuiTK_object_set_name (SDLGuiTK_Object *object, char *name)
{
    char txt[256];
    sprintf(txt, "Changing name %s in %s\n", object->name, name);
    SDLGUITK_LOG (txt);
    strcpy(object->name, name);
}

char * SDLGuiTK_object_get_name (SDLGuiTK_Object *object)
{
    return object->name;
}


