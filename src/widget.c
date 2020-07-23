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

#include "debug.h"
#include "mysdl.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "signal.h"
#include "context_prot.h"

#include "misc_prot.h"
#include "container_prot.h"
#include "tooltips_prot.h"

#include "windows/window_prot.h"


static int         current_id=1;
static SDL_mutex * current_id_mutex=NULL;

static SDLGuiTK_List * destroy_list=NULL;


/* DEFAULT FUNCTIONS when unset */

static SDLGuiTK_Widget * Unset_RecursiveEntering( SDLGuiTK_Widget * widget, \
						  int x, int y )
{

#if DEBUG_LEVEL >= 1
  char tmpstr[512];
  sprintf( tmpstr, "*RecursiveEntering: not setted in %s.\n", \
	   widget->object->name );
  SDLGUITK_ERROR( tmpstr );
#endif
  return NULL;
}

static void * Unset_RecursiveDestroy(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
  char tmpstr[512];
  sprintf( tmpstr, "*RecursiveDestroy: not setted in %s.\n", \
	   widget->object->name );
  SDLGUITK_ERROR( tmpstr );
#endif
  return (void *) NULL;
}

static int Unset_UpdateActive(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
  /* char tmpstr[512]; */
  /* sprintf( tmpstr, "*UpdateActive: not setted in %s.\n", \ */
	 /*   widget->object->name ); */
  /* SDLGUITK_ERROR( tmpstr ); */
#endif
  return 0;
}

static void * Unset_Free(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
  char tmpstr[512];
  sprintf( tmpstr, "*Free: fct not setted in %s.\n", \
	   widget->object->name );
  SDLGUITK_ERROR( tmpstr );
#endif
  return (void *) NULL;
}

static void * Unset_DrawUpdate(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
  char tmpstr[512];
  sprintf( tmpstr, "*DrawUpdate: fct not setted in %s.\n", \
	   widget->object->name );
  SDLGUITK_ERROR( tmpstr );
#endif
  return (void *) NULL;
}

static void * Unset_DrawBlit(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
  char tmpstr[512];
  sprintf( tmpstr, "*DrawBlit: fct not setted in %s.\n", \
	   widget->object->name );
  SDLGUITK_ERROR( tmpstr );
#endif
  return (void *) NULL;
}


/* WIDGET HANDLING */

static SDLGuiTK_Widget * Widget_create()
{
  SDLGuiTK_Widget * new_widget;

  new_widget = malloc( sizeof( struct SDLGuiTK_Widget ) );
  new_widget->object = PROT__object_new();
  new_widget->object->widget = new_widget;

  SDL_mutexP( current_id_mutex );
  new_widget->id = current_id++;
  SDL_mutexV( current_id_mutex );

  new_widget->shown = 0;
  new_widget->activable = 0; new_widget->activable_child = 0;
  new_widget->enter = 0;
  new_widget->parent = NULL;
  new_widget->top = NULL;
  new_widget->tooltipsdata = NULL;

/*   new_widget->changed = 0; */

  new_widget->misc = NULL;
  new_widget->container = NULL;
  new_widget->entry = NULL;
  new_widget->editable = NULL;
  new_widget->menushell = NULL;
  new_widget->scrollbar = NULL;

  new_widget->width_request = -1;
  new_widget->height_request = -1;
/*   new_widget->requisition.width = 0; */
/*   new_widget->requisition.height = 0; */

  new_widget->srf = MySDL_Surface_new ("Widget_srf");
  //new_widget->act_srf = MySDL_Surface_new ("Widget_act_srf");
  new_widget->act_srf = NULL;
  new_widget->act_alpha = 0.5;
/*   new_widget->srf_create = 0; */
  new_widget->req_area.x = 0; new_widget->req_area.y = 0;
  new_widget->req_area.w = 0; new_widget->req_area.h = 0;
  new_widget->rel_area.x = 0; new_widget->rel_area.y = 0;
  new_widget->rel_area.w = 0; new_widget->rel_area.h = 0;
  new_widget->abs_area.x = 0; new_widget->abs_area.y = 0;
  new_widget->abs_area.w = 0; new_widget->abs_area.h = 0;
  new_widget->act_area.x = 0; new_widget->act_area.y = 0;
  new_widget->act_area.w = 0; new_widget->act_area.h = 0;

  return new_widget;
}

static void Widget_destroy( SDLGuiTK_Widget * widget )
{
/*   printf( "DESTR: %s\n", widget->object->name ); */
  if( widget->tooltipsdata!=NULL )
    PROT__TooltipsData_Destroy( widget->tooltipsdata );
  MySDL_Surface_free( widget->srf );
/*   MySDL_FreeSurface( widget->act_srf ); */
  PROT__object_destroy( widget->object );
  free( widget );
}

void PROT__widget_init()
{
  current_id_mutex = SDL_CreateMutex();
  SDL_mutexP( current_id_mutex );
  current_id = 1;
  SDL_mutexV( current_id_mutex );

  destroy_list = SDLGuiTK_list_new();
}

void PROT__widget_uninit()
{
  SDLGuiTK_list_destroy( destroy_list );
  SDL_DestroyMutex( current_id_mutex );
  current_id_mutex = NULL;
  current_id = 1;
}


SDLGuiTK_Widget * PROT__widget_new()
{
  SDLGuiTK_Widget * widget;

  widget = Widget_create();

  widget->RecursiveEntering = Unset_RecursiveEntering;
  widget->RecursiveDestroy = Unset_RecursiveDestroy;
  widget->UpdateActive = Unset_UpdateActive;
  widget->Free = Unset_Free;

  widget->DrawUpdate = Unset_DrawUpdate;
  widget->DrawBlit = Unset_DrawBlit;

  return widget;
}

void PROT__widget_destroy( SDLGuiTK_Widget * widget )
{
  Widget_destroy( widget );
}




void SDLGuiTK_widget_show( SDLGuiTK_Widget * widget )
{
/*   SDL_mutexP( widget->object->mutex ); */
  PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_SHOW );

/*   if( widget->container!=NULL ) { */
/*     PROT__container_show_children( widget->container ); */
/*   } */
  if( widget->top!=NULL ) {
/*     SDL_mutexV( widget->object->mutex ); */
/*     SDL_mutexP( widget->top->object->mutex ); */
    PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
/*     SDL_mutexV( widget->top->object->mutex ); */
/*     SDL_mutexP( widget->object->mutex ); */
  }
/*   SDL_mutexV( widget->object->mutex ); */
}

void SDLGuiTK_widget_hide( SDLGuiTK_Widget * widget )
{
/*   SDLGuiTK_Window * window; */
#if DEBUG_LEVEL > 1
  char tmplog[256];
  sprintf( tmplog, "SDLGuiTK_widget_hide():  %s\n", widget->object->name );
  SDLGUITK_LOG( tmplog );
#endif

/*   SDL_mutexP( widget->object->mutex ); */
/*   window = SDLGuiTK_WINDOW( widget ); */
/*   if( window!=NULL ) { */
/*     PROT__context_unref_wmwidget( window->wm_widget ); */
/*   } */
  PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_HIDE );

/*   if( widget->container!=NULL ) { */
/*     PROT__container_hide_children( widget->container ); */
/*   } */
  if( widget->top!=NULL ) {
/*     SDL_mutexV( widget->object->mutex ); */
/*     SDL_mutexP( widget->top->object->mutex ); */
    PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
/*     SDL_mutexV( widget->top->object->mutex ); */
/*     SDL_mutexP( widget->object->mutex ); */
  }
/*   SDL_mutexV( widget->object->mutex ); */
}

void SDLGuiTK_widget_destroy( SDLGuiTK_Widget * widget )
{
/*   SDLGuiTK_Window * window; */
/*   SDLGuiTK_MenuShell * menushell; */
  SDLGuiTK_Container * container;
#if DEBUG_LEVEL > 1
  char tmplog[256];
  sprintf( tmplog, "SDLGuiTK_widget_destroy():  %s\n", widget->object->name );
  SDLGUITK_LOG( tmplog );
#endif

/*   window = SDLGuiTK_WINDOW( widget ); */
/*   if( window!=NULL ) { */
/*     PROT__context_unref_wmwidget( window->wm_widget ); */
/*   } */
/*   SDLGuiTK_widget_hide( widget ); */
/*   PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_HIDE ); */
/*   SDL_mutexP( widget->object->mutex ); */
  (*widget->RecursiveDestroy)( widget );
  PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_DESTROY );
/*   SDL_mutexV( widget->object->mutex ); */

/*   SDL_mutexP( widget->object->mutex ); */
  if( widget->top!=NULL ) {
/*     SDL_mutexV( widget->object->mutex ); */
/*     SDL_mutexP( widget->top->object->mutex ); */
    PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
/*     SDL_mutexV( widget->top->object->mutex ); */
/*     SDL_mutexP( widget->object->mutex ); */
    widget->top = NULL;
  }
/*   SDL_mutexV( widget->object->mutex ); */
  if( widget->parent!=NULL ) {
    SDLGUITK_LOG( "SDLGuiTK_widget_destroy(): widget->parent!=NULL\n" );
    container = SDLGuiTK_CONTAINER(widget->parent);
    if( container!=NULL ) {
      SDLGuiTK_container_remove( container, widget );
    } else {
      SDLGUITK_ERROR( "SDLGuiTK_widget_destroy(): widget->parent not known\n" );
    }
    widget->parent = NULL;
  }


  SDLGuiTK_list_lock( destroy_list );
  SDLGuiTK_list_append( destroy_list, (SDLGuiTK_Object *) widget );
  SDLGuiTK_list_unlock( destroy_list );
}


void  PROT__widget_DrawUpdate( SDLGuiTK_Widget * widget )
{
    /* int width, height; */

    /* if( widget->req_area.w<widget->width_request ) */
    /*     width = widget->width_request; */
    /* else */
    /*     width = widget->req_area.w; */
    /* if( widget->req_area.h<widget->height_request ) */
    /*     height = widget->height_request; */
    /* else */
    /*     height = widget->req_area.h; */

    /* widget->abs_area.w = width; */
    /* widget->abs_area.h = height; */
    /* widget->rel_area.w = width; */
    /* widget->rel_area.h = height; */

    if( widget->req_area.w<widget->width_request )
        widget->req_area.w = widget->width_request;
    if( widget->req_area.h<widget->height_request )
        widget->req_area.h = widget->height_request;
    /*
    if( widget->abs_area.w<widget->width_request ) {
        widget->abs_area.w = widget->width_request;
    }
    if( widget->abs_area.h<widget->height_request ){
        widget->abs_area.h = widget->height_request;
    }
    if( widget->abs_area.w<widget->rel_area.w ) {
        widget->abs_area.w = widget->rel_area.w;
    }
    if( widget->abs_area.h<widget->rel_area.h ) {
        widget->abs_area.h = widget->rel_area.h;
    }
*/
}

void PROT__widget_DrawBlit(   SDLGuiTK_Widget * widget )
{
/*   if( widget->srf_create==1 ) { */

    widget->abs_area.w = widget->req_area.w;
    widget->abs_area.h = widget->req_area.h;
    widget->rel_area.w = widget->req_area.w;
    widget->rel_area.h = widget->req_area.h;

    MySDL_CreateRGBSurface( widget->srf, \
			                widget->abs_area.w, \
			                widget->abs_area.h );
    if(widget->srf->srf==NULL)
        SDLGUITK_ERROR("No srf created in Widget DrawBlit()\n");
    //SDL_SetAlpha( widget->srf, SDL_RLEACCEL, 255 );
/*   } */

  /* ACTIVABLE CODE TODO */
/*   if( widget->misc!=NULL ) { */
/*     MyWM_append_activable( widget ); */
/*   } */
}

void PROT__widget_reset_req_area( SDLGuiTK_Widget *widget )
{
    widget->req_area.w = 0;
    widget->req_area.h = 0;
}

void PROT__widget_set_req_area( SDLGuiTK_Widget *widget, int req_w, int req_h )
{
    if(req_w>widget->req_area.w)
        widget->req_area.w = req_w;
    if(req_h>widget->req_area.h)
        widget->req_area.h = req_h;
}

void PROT__widget_destroypending()
{
  SDLGuiTK_Widget * widget;

  SDLGuiTK_list_lock( destroy_list );
  widget = (SDLGuiTK_Widget *) SDLGuiTK_list_pop_head(destroy_list);
  while( widget!=NULL ) {
    (*widget->Free)( widget );
    widget = (SDLGuiTK_Widget *) SDLGuiTK_list_pop_head(destroy_list);
  }
  SDLGuiTK_list_unlock( destroy_list );

}

void SDLGuiTK_widget_set_size_request( SDLGuiTK_Widget * widget, \
				       int width, int height )
{
/*   SDL_mutexP( widget->object->mutex ); */
  widget->width_request = width;
  widget->height_request = height;
/*   SDL_mutexV( widget->object->mutex ); */

  if( widget->top!=NULL ) {
    PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
  }
}


SDLGuiTK_Widget * PROT__widget_is_entering( SDLGuiTK_Widget * widget, \
					    int x, int y )
{
  SDLGuiTK_Widget * current=NULL;

/*   printf( "Test entering (%s); %d %d  ...\n ", widget->object->name, x, y ); */
  if( ( x>=widget->act_area.x &&
	x<=(widget->act_area.x+ \
	    widget->act_area.w) ) &&
      ( y>=widget->act_area.y &&
        y<=(widget->act_area.y+ \
	    widget->act_area.h) ) )
    {
      if( widget->enter==0 ) {
/* 	SDL_mutexP( widget->object->mutex ); */
/*   printf( "TESTED: %s (%d/%d %d)(%d/%d %d)\n", widget->object->name, \ */
/* 	  widget->act_area.x, widget->act_area.x+widget->act_area.w, \ */
/* 	  event->motion.x, \ */
/* 	  widget->act_area.y, widget->act_area.y+widget->act_area.h, \ */
/* 	  event->motion.y ); */
	widget->enter = 1;
	PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_ENTER );
/* 	SDL_mutexV( widget->object->mutex ); */
      }
/*       printf( "Test entering; RecursiveEntering\n " ); */
      current = (*widget->RecursiveEntering) (widget, x, y );
      if( current==NULL ) {
/* 	printf( "Test entering no new\n " ); */
	return widget;
      } else {
/* 	printf( "Test entering founded %s\n ", current->object->name ); */
	return current;
      }
    } else {
      if( widget->enter==1 ) {
/* 	SDL_mutexP( widget->object->mutex ); */
	widget->enter = 0;
	if( widget->tooltipsdata!=NULL ) widget->tooltipsdata->updated = 0;
	PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_LEAVE );
/* 	SDL_mutexV( widget->object->mutex ); */
      }
      return NULL;
    }

  return NULL;
}


