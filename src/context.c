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

#include "debug.h"
#include "mysdl.h"
#include "mywm.h"
#include "signal.h"
#include "object_prot.h"
#include "context_prot.h"
#include "widget_prot.h"
#include "tooltips_prot.h"


static SDLGuiTK_Context * Context_create()
{
  SDLGuiTK_Context * new_context;
 
  new_context = malloc( sizeof( struct SDLGuiTK_Context ) );
  new_context->surface = NULL;
  new_context->type = SDLGUITK_CONTEXT_MODE_NULL;
  new_context->mutex = SDL_CreateMutex();

  new_context->activables = SDLGuiTK_list_new();
  new_context->ref = SDLGuiTK_list_new();
  new_context->unref = SDLGuiTK_list_new();
  new_context->active_wmwidget = NULL;

  return new_context;
}

static void Context_destroy( SDLGuiTK_Context * context )
{
  SDLGuiTK_list_destroy( context->ref );
  SDLGuiTK_list_destroy( context->unref );
  SDLGuiTK_list_destroy( context->activables );
  SDL_DestroyMutex( context->mutex );
  free( context );
}


void PROT__context_new( SDL_Surface *surface, int type )
{
    current_context = Context_create();
    current_context->surface = surface;
    current_context->type = type;
}


void PROT__context_quit()
{
  SDLGuiTK_WMWidget * rest=NULL;

  SDLGuiTK_list_lock( current_context->activables );
  rest = (SDLGuiTK_WMWidget *) \
    SDLGuiTK_list_ref_init( current_context->activables );
  while( rest!=NULL ) {
    SDLGuiTK_widget_destroy( rest->widget );
    rest = (SDLGuiTK_WMWidget *) \
      SDLGuiTK_list_ref_next( current_context->activables );
  }  
  SDLGuiTK_list_unlock( current_context->activables );

/*   SDLGuiTK_list_lock( current_context->activables ); */
/*   while( SDLGuiTK_list_length(current_context->activables)!=0 ) { */
/*     SDLGuiTK_list_unlock( current_context->activables ); */
/*     SDL_Delay(100); */
/*     SDLGuiTK_list_lock( current_context->activables ); */
/*   } */
/*   SDLGuiTK_list_unlock( current_context->activables ); */

  PROT__Tooltips_DestroyAll();  /* Hack to free tooltips struct */
    PROT__signal_check();
    SDL_Delay( 200 );
    PROT__signal_check();
}

void PROT__context_uninit()
{
  Context_destroy( current_context );
}


void PROT__context_ref_wmwidget( SDLGuiTK_WMWidget * wm_widget )
{
  SDLGuiTK_list_lock( current_context->ref );
  SDLGuiTK_list_append( current_context->ref, \
			(SDLGuiTK_Object *) wm_widget );
  SDLGuiTK_list_unlock( current_context->ref );
/*   PROT_MyWM_checkfornew( wm_widget->widget ); */
#if DEBUG_LEVEL >= 2
  char tmpstr[512];
    sprintf( tmpstr, \
	     "_context_ref_wmwidget(): %s\n", wm_widget->object->name );
    SDLGUITK_LOG( tmpstr );
#endif
}

void PROT__context_unref_wmwidget( SDLGuiTK_WMWidget * wm_widget )
{
#if DEBUG_LEVEL >= 2
  char tmpstr[512];
#endif
  SDLGuiTK_WMWidget * rem_widget=NULL;

  SDLGuiTK_list_lock( current_context->activables );

  rem_widget = \
    (SDLGuiTK_WMWidget *) SDLGuiTK_list_remove(current_context->activables,\
					       (SDLGuiTK_Object *) wm_widget );
  if( rem_widget!=NULL ) {
  SDLGuiTK_list_lock( current_context->unref );
  SDLGuiTK_list_append( current_context->unref, \
			(SDLGuiTK_Object *) wm_widget->surface2D );
  SDLGuiTK_list_unlock( current_context->unref );
  if( rem_widget==wm_widget ) {
#if DEBUG_LEVEL >= 2
    sprintf( tmpstr, \
	     "_context_unref_wmwidget(): %s\n", rem_widget->object->name );
    SDLGUITK_LOG( tmpstr );
#endif
  } else {
    SDLGUITK_ERROR( "_context_unref_wmwidget(): cancel!\n" );
    exit(0);
  }
  }

/*   PROT_MyWM_checkactive( rem_widget->widget ); */
/*   if( current_context->active_wmwidget==rem_widget ) { */
/*     PROT__signal_push( current_context->active_wmwidget->widget->object, \ */
/* 		       SDLGUITK_SIGNAL_TYPE_LEAVE ); */
/*     current_context->active_wmwidget = NULL; */
/*     SDLGUITK_LOG( "_context_unref_wmwidget(): Active widget unref\n" ); */
/*   } */

  SDLGuiTK_list_unlock( current_context->activables );
  PROT_MyWM_leaveall();
  PROT_MyWM_checkactive( rem_widget->widget );
}



void SDLGuiTK_context_check()
{
  SDLGuiTK_WMWidget * current=NULL;
  SDLGuiTK_Surface2D * current2D=NULL;

  SDL_mutexP( current_context->mutex );
  SDLGuiTK_list_lock( current_context->activables );
  SDLGuiTK_list_lock( current_context->ref );
  SDLGuiTK_list_lock( current_context->unref );

  current = \
    (SDLGuiTK_WMWidget *) SDLGuiTK_list_pop_head( current_context->ref );
  while( current!=NULL ) {
    current->surface2D = MySDL_surface2D_new();
    SDLGuiTK_list_append( current_context->activables, \
			  (SDLGuiTK_Object *) current );
    SDLGuiTK_list_unlock( current_context->activables );
    PROT_MyWM_leaveall();
    PROT_MyWM_checkactive( current->widget );
    SDLGuiTK_list_lock( current_context->activables );
    current = \
      (SDLGuiTK_WMWidget *) SDLGuiTK_list_pop_head( current_context->ref );
  }

  current2D = \
    (SDLGuiTK_Surface2D *) SDLGuiTK_list_pop_head( current_context->unref );
  while( current2D!=NULL ) {
    MySDL_surface2D_destroy( current2D );
    current2D = \
      (SDLGuiTK_Surface2D *) SDLGuiTK_list_pop_head( current_context->unref );
  }

  SDLGuiTK_list_unlock( current_context->unref );
  SDLGuiTK_list_unlock( current_context->ref );
  SDLGuiTK_list_unlock( current_context->activables );
  SDL_mutexV( current_context->mutex );

  PROT__signal_check();
}

void SDLGuiTK_context_blitsurfaces()
{
  SDLGuiTK_WMWidget * current=NULL;

  SDL_mutexP( current_context->mutex );
  SDLGuiTK_list_lock( current_context->activables );

  if( SDLGuiTK_list_length( current_context->activables )==0 ) {
    SDLGuiTK_list_unlock( current_context->activables );
    SDL_mutexV( current_context->mutex );
    return;
  }
#ifdef HAVE_GL_GL_H
  MySDL_GL_Enter2DMode();
#endif
  current = (SDLGuiTK_WMWidget *) \
    SDLGuiTK_list_ref_init( current_context->activables );
  while( current!=NULL ) {

    MyWM_blitsurface( current );

    current = (SDLGuiTK_WMWidget *) \
      SDLGuiTK_list_ref_next( current_context->activables );
  }
#ifdef HAVE_GL_GL_H
  MySDL_GL_Leave2DMode();
#endif

  SDLGuiTK_list_unlock( current_context->activables );
  SDL_mutexV( current_context->mutex );
}


int  SDLGuiTK_context_pushevent( SDL_Event *event )
{
  int flag;

  SDL_mutexP( current_context->mutex );

  if( event->type==SDL_MOUSEMOTION ) {
    flag = MyWM_push_MOUSEMOTION( event );
    SDL_mutexV( current_context->mutex );
    return flag;
  }

  SDLGuiTK_list_lock( current_context->activables );

  if( current_context->active_wmwidget!=NULL ) {
    SDLGuiTK_list_unlock( current_context->activables );
    switch( event->type ) {
    case SDL_MOUSEBUTTONDOWN:
      flag = MyWM_push_MOUSEBUTTONDOWN( event );
      SDL_mutexV( current_context->mutex );
      return flag;
    case SDL_MOUSEBUTTONUP:
      flag = MyWM_push_MOUSEBUTTONUP( event );
      SDL_mutexV( current_context->mutex );
      return flag;
/*     case SDL_KEYUP: */
/*       return MyWM_push_KEYUP( event ); */
    case SDL_KEYDOWN:
      flag = MyWM_push_KEYDOWN( event );
      SDL_mutexV( current_context->mutex );
      return flag;
    case SDL_TEXTINPUT:
      flag = MyWM_push_TEXTINPUT( event );
      SDL_mutexV( current_context->mutex );
      return flag;
    default:
      SDL_mutexV( current_context->mutex );
      return 0;
    }
  } else {
    SDLGuiTK_list_unlock( current_context->activables );
    SDL_mutexV( current_context->mutex );
  }

  if( event->type==SDL_QUIT ) {
    if( current_context->type==SDLGUITK_CONTEXT_MODE_SELF ) {
      SDLGuiTK_main_quit();
    }
  }

  return 0;
}


void  SDLGuiTK_threads_enter()
{
  SDL_mutexP( current_context->mutex );
}

void  SDLGuiTK_threads_leave()
{
  SDL_mutexV( current_context->mutex );
}

