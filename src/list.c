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

#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "list.h"


static SDLGuiTK_List_Element * Element_create( SDLGuiTK_Object * object )
{
  SDLGuiTK_List_Element * new_element;

  new_element = malloc( sizeof( struct SDLGuiTK_List_Element ) );
  new_element->object = object;

  return new_element;
}

static void Element_destroy( SDLGuiTK_List_Element * element )
{
  free( element );
}

static SDLGuiTK_List_Element * Element_find( SDLGuiTK_List * list, \
					     SDLGuiTK_Object * object )
{
  SDLGuiTK_List_Element * element;

  element = list->first;
  while( element!=NULL ) {
    if( element->object==object ) { return element; };
    element = element->next;
  }
  return NULL;
}


static int list_count=0;

SDLGuiTK_List * SDLGuiTK_list_new()
{
  SDLGuiTK_List * new_list;
#if DEBUG_LEVEL >= 2
  char tmpstr[64];
#endif

  new_list = malloc( sizeof( struct SDLGuiTK_List ) );
  new_list->mutex = SDL_CreateMutex();
  new_list->length = 0;
  new_list->first = NULL;
  new_list->last = NULL;
  new_list->ref = NULL;
  new_list->refrv = NULL;

  sprintf( new_list->name, "list_%d", list_count++ );
#if DEBUG_LEVEL >= 2
  sprintf( tmpstr, "list created: %s\n", new_list->name );
#endif
  SDLGUITK_LOG( tmpstr );

  return new_list;
}

SDLGuiTK_List * SDLGuiTK_list_copy( SDLGuiTK_List * list )
{
/*   SDLGuiTK_List * dst_list; */

  SDLGUITK_ERROR( "SDLGuiTK_list_copy(): TODO\n" );
/*   dst_list =  */

  return NULL;
}


void SDLGuiTK_list_destroy( SDLGuiTK_List * list )
{
#if DEBUG_LEVEL >= 2
  char tmpstr[64];
  sprintf( tmpstr, "list deleted: %s\n", list->name );
#endif

  if( list->length!=0 ) {
    SDLGUITK_LOG( "SDLGuiTK_list_destroy(): list not empty - FORCED\n" );
    SDLGuiTK_list_remove_all( list );
  }
  SDL_DestroyMutex( list->mutex );
  free( list );

  SDLGUITK_LOG( tmpstr );
}

void SDLGuiTK_list_lock( SDLGuiTK_List * list )
{
  SDL_mutexP( list->mutex );
}

void SDLGuiTK_list_unlock( SDLGuiTK_List * list )
{
  if( list->ref!=NULL ) {
    SDLGUITK_LOG( "_list_unlock(): list->ref!=NULL\n" );
/*     list->ref = NULL; */
  }
  if( list->refrv!=NULL ) {
    SDLGUITK_LOG( "_list_unlock(): list->refrv!=NULL\n" );
/*     list->refrv = NULL; */
  }
  SDL_mutexV( list->mutex );
}


void SDLGuiTK_list_append( SDLGuiTK_List * list, SDLGuiTK_Object * object )
{
  SDLGuiTK_List_Element * element;

  element = Element_create( object );

  if( list->length==0 ) {
    list->first = element;
    list->last = element;
    element->previous = NULL;
  } else {
    list->last->next = element;
    element->previous = list->last;
    list->last = element;
  }
  element->next = NULL;
  list->length++;
}

void SDLGuiTK_list_prepend( SDLGuiTK_List * list, SDLGuiTK_Object * object )
{
  SDLGuiTK_List_Element * element;

  element = Element_create( object );

  if( list->length==0 ) {
    list->first = element;
    list->last = element;
    element->next = NULL;
  } else {
    list->first->previous = element;
    element->next = list->first;
    list->first = element;
  }
  element->previous = NULL;
  list->length++;
}

static void RefCheckOnRemove( SDLGuiTK_List * list, \
			      SDLGuiTK_List_Element * element ) {
  if( list->ref==element ) {
    if( list->ref==list->last ) {
      if( list->last->previous!=NULL ) {
	list->ref = list->last->previous;
      } else {
	list->ref = NULL;
      }
    } else {
      list->ref = list->last->next;
    }
  }
  if( list->refrv==element ) {
    if( list->refrv==list->first ) {
      if( list->last->next!=NULL ) {
	list->refrv = list->last->next;
      } else {
	list->refrv = NULL;
      }
    } else {
      list->refrv = list->last->previous;
    }
  }
}

SDLGuiTK_Object * SDLGuiTK_list_pop_head( SDLGuiTK_List * list )
{
  SDLGuiTK_Object * object;
  SDLGuiTK_List_Element * element;

  if( list->first!=NULL ) {
    RefCheckOnRemove( list, list->first );
    object = list->first->object;
    if( list->length==1 ) {
      Element_destroy( list->first );
      list->first = NULL;
      list->last = NULL;
    } else {
      element = list->first->next;
      Element_destroy( list->first );
      list->first = element;
      element->previous = NULL;
    }
    list->length--;
    return object;
  }
  return NULL;
}

SDLGuiTK_Object * SDLGuiTK_list_pop_tail( SDLGuiTK_List * list )
{
  SDLGuiTK_Object * object;
  SDLGuiTK_List_Element * element;

  if( list->last!=NULL ) {
    RefCheckOnRemove( list, list->last );
    object = list->last->object;
    if( list->length==1 ) {
      Element_destroy( list->last );
      list->first = NULL;
      list->last = NULL;
    } else {
      element = list->last->previous;
      Element_destroy( list->last );
      list->last = element;
      element->next = NULL;
    }
    list->length--;
    return object;
  }

  return NULL;
}

SDLGuiTK_Object * SDLGuiTK_list_remove( SDLGuiTK_List * list, \
					SDLGuiTK_Object * object )
{
  SDLGuiTK_List_Element * element, * element_p, * element_n;

  element = Element_find( list, object );
  if( element==NULL ) { return NULL; };

  RefCheckOnRemove( list, element );

  if( list->length==1 ) {
    list->first = NULL;
    list->last = NULL;
    list->length = 0;
    Element_destroy( element );
    return object;
  }

  element_p = element->previous;
  element_n = element->next;

  if( element_p==NULL ) {
    list->first = element_n;
    if( element_n!=NULL ) { element_n->previous = NULL; };
  } else {
    if( element_n!=NULL ) { element_n->previous = element_p; };
  }
  if( element_n==NULL ) {
    list->last = element_p;
    if( element_p!=NULL ) { element_p->next = NULL; };
  } else {
    if( element_p!=NULL ) { element_p->next = element_n; };
  }
  list->length--;
  Element_destroy( element );

  return object;
}

void SDLGuiTK_list_remove_all( SDLGuiTK_List * list )
{
  SDLGuiTK_List_Element * element, * element_n;

  if( list->length>=1 ) {
    element = list->first;
    while( element!=NULL ) {
      element_n = element->next;
      Element_destroy( element );
      element = element_n;
      list->length--;
    }
    list->first = NULL;
    list->last = NULL;
  }
}

Uint32 SDLGuiTK_list_length( SDLGuiTK_List * list )
{
  return list->length;
}

SDLGuiTK_Object * SDLGuiTK_list_find( SDLGuiTK_List * list, \
				      SDLGuiTK_Object * object )
{
  SDLGuiTK_List_Element * element;

  element = Element_find( list, object );
  if( element==NULL ) { return NULL; };

  return object;
}

SDLGuiTK_Object * SDLGuiTK_list_ref_init( SDLGuiTK_List * list )
{
  if( list->ref!=NULL ) {
    SDLGUITK_ERROR( "_list_ref_init(): list->ref!=NULL\n" );
    //exit(1);
  }
  if( list->first==NULL ) { return NULL; };
  list->ref = list->first;
  return list->ref->object;
}

SDLGuiTK_Object * SDLGuiTK_list_ref_next( SDLGuiTK_List * list )
{
  if( list->ref==NULL ) return NULL;
  if( list->ref==list->last ) {
    list->ref = NULL;
    return NULL;
  }
  list->ref = list->ref->next;
  return list->ref->object;
}

SDLGuiTK_Object * SDLGuiTK_list_refrv_init( SDLGuiTK_List * list )
{
  if( list->refrv!=NULL ) {
    SDLGUITK_ERROR( "_list_refrv_init(): list->refrv!=NULL\n" );
    //exit(1);
  }
  if( list->last==NULL ) { return NULL; };
  list->refrv = list->last;
  return list->refrv->object;
}

SDLGuiTK_Object * SDLGuiTK_list_refrv_next( SDLGuiTK_List * list )
{
  if( list->refrv==NULL ) return NULL;
  if( list->refrv==list->first ) {
    list->refrv = NULL;
    return NULL;
  }
  list->refrv = list->refrv->previous;
  return list->refrv->object;
}

void PROT_List_ref_reinit( SDLGuiTK_List * list ) {
  list->ref = NULL;
}

void PROT_List_refrv_reinit( SDLGuiTK_List * list ) {
  list->refrv = NULL;
}
