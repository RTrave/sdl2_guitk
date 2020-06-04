/* 
   SDL_guitk - GUI toolkit designed for SDL environnements.

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
#include "../debug.h"
#include "../mysdl.h"
#include "../signal.h"
#include "../list.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../theme_prot.h"
#include "../container_prot.h"
#include "box_prot.h"

#include "boxchild.h"



static SDL_bool SDLGuiTK_IS_BOX( SDLGuiTK_Widget * widget )
{
  if( widget!=NULL ) {
    if( widget->container!=NULL ) {
      if( widget->container->box!=NULL ) {
	return SDL_TRUE;
      }
    }
  }
  SDLGUITK_ERROR( "SDLGuiTK_IS_BOX(): widget is not a box\n" );
  return SDL_FALSE;
}

SDLGuiTK_Box * SDLGuiTK_BOX( SDLGuiTK_Widget * widget )
{
  if( !SDLGuiTK_IS_BOX(widget) ) return NULL;
  return widget->container->box;
}


static int current_id=0;

static SDLGuiTK_Box * Box_create()
{
  SDLGuiTK_Box * new_box;

  new_box = malloc( sizeof( struct SDLGuiTK_Box ) );
  new_box->container = PROT__container_new();
  new_box->object = new_box->container->object;
  new_box->container->box = new_box;
  sprintf( new_box->object->name, "box%d", ++current_id );

  new_box->type = 0;
  new_box->spacing = 0;
  new_box->homogeneous = SDL_FALSE;
  new_box->homogeneous_size = 0;
  new_box->children = SDLGuiTK_list_new();

  return new_box;
}

static void Box_destroy( SDLGuiTK_Box * box )
{
  SDLGuiTK_list_destroy( box->children );

  PROT__container_destroy( box->object->widget->container );
  free( box );
}



static void * Box_DrawUpdate( SDLGuiTK_Widget * widget )
{
  SDLGuiTK_Box * box=widget->container->box;
  BoxChild * current_child;
  SDLGuiTK_Widget * current=NULL;

  /* INIT BOX VALUES */
  box->shown_nb = 0;
  box->expanded_nb = 0;
  box->expanded_size = 0;
  box->homogeneous_size = 0;
  box->container->children_area.w = 0;
  box->container->children_area.h = 0;

  /* START CHILDREN UPDATE */
  SDLGuiTK_list_lock( box->children );

  /* IF TOP WIDGET NO EXISTS ... */
  if( widget->top==NULL ) {
    SDLGUITK_ERROR( "Box_UpdateDim(): can't be a top widget\n" );
    return 0;
  }

  current_child = (BoxChild *) SDLGuiTK_list_ref_init( box->children );
  while( current_child!=NULL ) {
    current = current_child->child;
/*     SDL_mutexP( current->object->mutex ); */

    current->top = widget->top;
    current->parent = widget;

    if( widget->shown==1 && current_child->child->shown==1 ) {

    BoxChild_DrawUpdate( current_child );

    if( box->type==SDLGUITK_BOX_H ) {
      if( box->homogeneous_size<current_child->area.w ) {
	box->homogeneous_size = current_child->area.w;
      }
      if( box->shown_nb!=0 ) {
	box->container->children_area.w += box->spacing;
      }
      box->container->children_area.w += current_child->area.w;
      if( box->container->children_area.h<current_child->area.h ) {
	box->container->children_area.h = current_child->area.h;
      }
    }
    if( box->type==SDLGUITK_BOX_V ) {
      if( box->homogeneous_size<current_child->area.h ) {
	box->homogeneous_size = current_child->area.h;
      }
      if( box->shown_nb!=0 ) {
	box->container->children_area.h += box->spacing;
      }
      if( box->container->children_area.w<current_child->area.w ) {
	box->container->children_area.w = current_child->area.w;
      }
      box->container->children_area.h += current_child->area.h;
    }

    box->shown_nb++;
    }

    current_child = (BoxChild *) SDLGuiTK_list_ref_next( box->children );
  }  


  if( box->homogeneous==SDL_TRUE ) {
    if( box->type==SDLGUITK_BOX_H ) {
      box->container->children_area.w = \
	box->shown_nb*box->homogeneous_size + (box->shown_nb-1)*box->spacing;
    }    
    if( box->type==SDLGUITK_BOX_V ) {
      box->container->children_area.h = \
	box->shown_nb*box->homogeneous_size + (box->shown_nb-1)*box->spacing;
    }    
  }

  PROT__container_DrawUpdate( box->container );

  return (void *) NULL;
}

static void * Box_DrawBlit( SDLGuiTK_Widget * widget )
{
  SDLGuiTK_Box * box=widget->container->box;
  BoxChild * current_child;
  //SDLGuiTK_Widget * current=NULL;

  box->current_x=box->container->border_width;
  box->current_y=box->container->border_width;

  if( box->shown_nb<1 ) {
    current_child = (BoxChild *) SDLGuiTK_list_ref_init( box->children );
    while( current_child!=NULL ) {
      //current = current_child->child;
/*       SDL_mutexV( current->object->mutex ); */
      current_child = (BoxChild *) SDLGuiTK_list_ref_next( box->children );
    } //TODO loop useless

    PROT__container_DrawBlit( box->container );
    SDLGuiTK_list_unlock( box->children );
    SDLGUITK_ERROR( "Box_DrawBlit(): box->shown_nb<1\n" );
    return 0;
  }

  if( widget->abs_area.w>widget->rel_area.w ) {
    if( box->type==SDLGUITK_BOX_H ) {
      box->homogeneous_size = 
	(widget->abs_area.w - 2*box->container->border_width
	 - (box->shown_nb-1)*box->spacing )
	/ box->shown_nb ;
      box->expanded_size = widget->abs_area.w - widget->rel_area.w;
    }
  }
  if( widget->abs_area.h>widget->rel_area.h ) {
    if( box->type==SDLGUITK_BOX_V ) {
      box->homogeneous_size = 
	(widget->abs_area.h - 2*box->container->border_width
	 - (box->shown_nb-1)*box->spacing )
	/ box->shown_nb ;
      box->expanded_size = widget->abs_area.h - widget->rel_area.h;
    }
  }

  box->container->children_area.w = \
    widget->abs_area.w - 2*box->container->border_width;
  box->container->children_area.h = \
    widget->abs_area.h - 2*box->container->border_width;

  PROT__container_DrawBlit( box->container );

  if( widget->top==NULL ) {
    SDLGuiTK_list_unlock( box->children );
    SDLGUITK_ERROR( "Box_UpdateDim(): can't be a top widget\n" );
    return 0;
  }

  box->shown_nb = 0;

  current_child = (BoxChild *) SDLGuiTK_list_ref_init( box->children );
  while( current_child!=NULL ) {
    //current = current_child->child;

    if( widget->shown==1 && current_child->child->shown==1  ) {

    current_child->area.x = box->current_x;
    current_child->area.y = box->current_y;
    if( box->homogeneous==SDL_TRUE ) {
      if( box->type==SDLGUITK_BOX_H ) {
	current_child->area.w = box->homogeneous_size;
	current_child->area.h = box->container->children_area.h;
      }
      if( box->type==SDLGUITK_BOX_V ) {
	current_child->area.w = box->container->children_area.w;
	current_child->area.h = box->homogeneous_size;
      }
    } else if( current_child->expand==SDL_TRUE ) {
      if( box->type==SDLGUITK_BOX_H ) {
	current_child->area.w += (int)(box->expanded_size/box->expanded_nb);
	current_child->area.h = box->container->children_area.h;
      }
      if( box->type==SDLGUITK_BOX_V ) {
	current_child->area.w = box->container->children_area.w;
	current_child->area.h += (int)(box->expanded_size/box->expanded_nb);
      }
    } else {
      if( box->type==SDLGUITK_BOX_H ) {
	current_child->area.h = box->container->children_area.h;
      }
      if( box->type==SDLGUITK_BOX_V ) {
	current_child->area.w = box->container->children_area.w;
      }
    }

    BoxChild_DrawBlit( current_child );

    if( box->type==SDLGUITK_BOX_H ) {
      box->current_x += ( current_child->area.w + box->spacing );
    }
    if( box->type==SDLGUITK_BOX_V ) {
      box->current_y += ( current_child->area.h + box->spacing );
    }

    box->shown_nb++;
    }

/*     SDL_mutexV( current->object->mutex ); */
    current_child = (BoxChild *) SDLGuiTK_list_ref_next( box->children );
  }

  SDLGuiTK_list_unlock( box->children );

  widget->rel_area.w = widget->abs_area.w;
  widget->rel_area.h = widget->abs_area.h;

  widget->act_area.x = widget->abs_area.x;
  widget->act_area.y = widget->abs_area.y;
  widget->act_area.w = widget->abs_area.w;
  widget->act_area.h = widget->abs_area.h;

  return (void *) NULL;
}


static SDLGuiTK_Widget * Box_RecursiveEntering( SDLGuiTK_Widget * widget, \
						int x, int y )
{
  SDLGuiTK_Box * box=widget->container->box;
  BoxChild * current_child;
  SDLGuiTK_Widget * child;
  SDLGuiTK_Widget * active;

  SDLGuiTK_list_lock( box->children );
  current_child = (BoxChild *) SDLGuiTK_list_ref_init( box->children );
  while( current_child!=NULL ) {
    child = current_child->child;
/*     SDL_mutexP( child->object->mutex ); */
    if( child->shown==1 ) {
      active = PROT__widget_is_entering( child, x, y );
      if( active!=NULL ) {
/*       SDL_mutexV( child->object->mutex ); */
	PROT_List_ref_reinit( box->children );
	SDLGuiTK_list_unlock( box->children );
	return active;
      }
    }
/*     SDL_mutexV( child->object->mutex ); */
    current_child = (BoxChild *) SDLGuiTK_list_ref_next( box->children );
  }
  

  SDLGuiTK_list_unlock( box->children );

  return NULL;
  
}

static void * Box_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
  SDLGuiTK_Widget * current;
  BoxChild * child;

  SDLGuiTK_list_lock( widget->container->box->children );

  child = \
    (BoxChild *) SDLGuiTK_list_ref_init( widget->container->box->children );
  while( child!=NULL ) {
    current = child->child;
    SDLGuiTK_widget_destroy( current );
    child = \
      (BoxChild *) SDLGuiTK_list_ref_next( widget->container->box->children );
  }

  SDLGuiTK_list_unlock( widget->container->box->children );

  return (void *) NULL;
}

static void * Box_Free( SDLGuiTK_Widget * widget )
{
  Box_destroy( widget->container->box );

  return (void *) NULL;
}

static void * Box_Realize( SDLGuiTK_Widget * widget, \
			   void * data, void * event )
{
  return (void *) NULL;
}

static void * Box_Show( SDLGuiTK_Widget * widget, \
			  void * data, void * event )
{
  widget->shown = 1;
  if( widget->top!=NULL ) {
    PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
  }

  return (void *) NULL;
}

static void * Box_Hide( SDLGuiTK_Widget * widget, \
			void * data, void * event )
{
  widget->shown = 0;
  if( widget->top!=NULL ) {
    PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
  }

  return (void *) NULL;
}



static void Box_Init_functions( SDLGuiTK_Box * box )
{
  SDLGuiTK_SignalHandler * handler;

  handler = (SDLGuiTK_SignalHandler *) box->object->signalhandler;

  box->object->widget->RecursiveEntering = Box_RecursiveEntering;
  box->object->widget->RecursiveDestroy = Box_RecursiveDestroy;
  box->object->widget->Free = Box_Free;

  box->object->widget->DrawUpdate = Box_DrawUpdate;
  box->object->widget->DrawBlit = Box_DrawBlit;

  handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
    Box_Realize;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
    Box_Show;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
    Box_Hide;
}


SDLGuiTK_Widget * SDLGuiTK_hbox_new()
{
  SDLGuiTK_Box * hbox;

  hbox = Box_create();
  hbox->type = SDLGUITK_BOX_H;
  Box_Init_functions( hbox );
  PROT__signal_push( hbox->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

  return hbox->object->widget;
}

SDLGuiTK_Widget * SDLGuiTK_vbox_new()
{
  SDLGuiTK_Box * vbox;

  vbox = Box_create();
  vbox->type = SDLGUITK_BOX_V;
  Box_Init_functions( vbox );
  PROT__signal_push( vbox->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

  return vbox->object->widget;
}



void SDLGuiTK_box_pack_start( SDLGuiTK_Box * box, \
			      SDLGuiTK_Widget * widget, \
			      SDL_bool expand,
			      SDL_bool fill,
			      int padding )
{
  BoxChild * child;

/*   SDL_mutexP( widget->object->mutex ); */
  child = BoxChild_create( widget );
  child->expand = expand;
  child->fill = fill;
  child->padding = padding;

  widget->parent = box->object->widget;
  widget->top = box->object->widget->top;

/*   SDL_mutexV( widget->object->mutex ); */

  SDLGuiTK_list_lock( box->children );
  SDLGuiTK_list_append( box->children, (SDLGuiTK_Object *) child );
  SDLGuiTK_list_unlock( box->children );
}



void SDLGuiTK_box_set_spacing( SDLGuiTK_Box * box, int spacing )
{
/*   SDL_mutexP( box->object->mutex ); */
  box->spacing = spacing;
/*   SDL_mutexV( box->object->mutex ); */
}


int  SDLGuiTK_box_get_spacing( SDLGuiTK_Box * box )
{
  int spacing;

/*   SDL_mutexP( box->object->mutex ); */
  spacing = box->spacing;
/*   SDL_mutexV( box->object->mutex ); */

  return spacing;
}


void SDLGuiTK_box_set_homogeneous( SDLGuiTK_Box * box, SDL_bool homogeneous )
{
/*   SDL_mutexP( box->object->mutex ); */
  box->homogeneous = homogeneous;
/*   SDL_mutexV( box->object->mutex ); */
}

SDL_bool  SDLGuiTK_box_get_homogeneous( SDLGuiTK_Box * box )
{
  SDL_bool homogeneous;

/*   SDL_mutexP( box->object->mutex ); */
  homogeneous = box->homogeneous;
/*   SDL_mutexV( box->object->mutex ); */

  return homogeneous;
}


void PROT__box_remove( SDLGuiTK_Box * this_box, \
		       SDLGuiTK_Widget * widget )
{
  BoxChild * child;
#if DEBUG_LEVEL > 1
  char tmplog[256];
  sprintf( tmplog, "__box_remove():  %s from %s\n", widget->object->name, this_box->object->name );
  SDLGUITK_LOG( tmplog );
#endif

  if( (child=BoxChild_remove(this_box,widget))!=NULL ) {
    widget->parent = NULL;
    widget->top = NULL;
    BoxChild_destroy( child );
  } else {
    SDLGUITK_ERROR( "PROT__box_remove(): widget not found in box\n" );
  }
}

