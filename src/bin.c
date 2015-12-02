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

#include "debug.h"
#include "mysdl.h"
#include "signal.h"

#include "object_prot.h"
#include "widget_prot.h"
#include "theme_prot.h"

#include "container_prot.h"
#include "bin_prot.h"


static SDL_bool SDLGuiTK_IS_BIN( SDLGuiTK_Widget * widget )
{
  if( widget!=NULL ) {
    if( widget->container!=NULL ) {
      if( widget->container->bin!=NULL ) {
	return SDL_TRUE;
      }
    }
  }
  SDLGUITK_ERROR( "SDLGuiTK_IS_BIN(): widget is not a bin\n" );
  return SDL_FALSE;
}

SDLGuiTK_Bin * SDLGuiTK_BIN( SDLGuiTK_Widget * widget )
{
  if( !SDLGuiTK_IS_BIN(widget) ) return NULL;
  return widget->container->bin;
}


static SDLGuiTK_Bin * Bin_create()
{
  SDLGuiTK_Bin * new_bin;

  new_bin = malloc( sizeof( struct SDLGuiTK_Bin ) );
  new_bin->container = PROT__container_new();
  new_bin->object = new_bin->container->object;
  new_bin->container->bin = new_bin;

  new_bin->window = NULL;
  new_bin->panel = NULL;
  new_bin->alignment = NULL;
  new_bin->button = NULL;
  new_bin->menuitem = NULL;

  new_bin->child = NULL;

  new_bin->srf = NULL;

  return new_bin;
}

static void Bin_destroy( SDLGuiTK_Bin * bin )
{
  PROT__container_destroy( bin->container );
  MySDL_FreeSurface( bin->srf );
  free( bin );
}


SDLGuiTK_Bin * PROT__bin_new()
{
  SDLGuiTK_Bin * bin;

  bin = Bin_create();

  return bin;
}

void PROT__bin_destroy( SDLGuiTK_Bin * bin )
{
  Bin_destroy( bin );
}


SDLGuiTK_Widget * SDLGuiTK_bin_get_child( SDLGuiTK_Bin * bin )
{
  return bin->child;
}


void PROT__bin_add( SDLGuiTK_Bin * bin, SDLGuiTK_Widget * widget )
{
#if DEBUG_LEVEL >= 2
  char tmpstr[512];
#endif

  if( bin->child!=NULL ) {
    SDLGUITK_ERROR( "__bin_add(): yet have a child! LOOSING IT!\n" );
  }
  bin->child = widget;
  widget->parent = bin->object->widget;
  if( bin->object->widget->top!=NULL ) {
    widget->top = bin->object->widget->top;
  } else {
    widget->top = bin->object->widget;
  }

#if DEBUG_LEVEL >= 2
  sprintf( tmpstr, "__bin_add(): %s added to %s\n", \
	   widget->object->name, widget->parent->object->name );
  SDLGUITK_LOG( tmpstr );
#endif

  PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
}

void PROT__bin_remove( SDLGuiTK_Bin * bin, SDLGuiTK_Widget * widget )
{
#if DEBUG_LEVEL > 1
  char tmplog[256];
  sprintf( tmplog, "__bin_remove():  %s from %s\n", widget->object->name, bin->object->name );
  SDLGUITK_LOG( tmplog );
#endif

  if( bin->child!=widget ) {
    SDLGUITK_ERROR( "__bin_remove(): Bin child is not the one you think!\n" );
    return;
  }

  bin->child = NULL;
  widget->parent = NULL;
/*   widget->top = NULL; */

  if( bin->object->widget->top!=NULL ) {
    PROT__signal_push( bin->object->widget->top->object, \
		       SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
  } else if( bin->object!=NULL ) {
    PROT__signal_push( bin->object, \
		       SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
  }
}


void PROT__bin_DrawUpdate( SDLGuiTK_Bin * bin )
{
  SDLGuiTK_Widget *widget=bin->object->widget;

  if(bin->child!=NULL) {

    /* LOCK CHILD */
/*     SDL_mutexP( bin->child->object->mutex ); */

    /* UPDATE CHILD ASCENDENTS */
    bin->child->parent = widget;
    if( widget->top!=NULL ) {
      bin->child->top = widget->top;
    } else {
      bin->child->top = widget;
    }

    /* IF CHILD SHOWN */
    if( bin->child->shown==1 ) {

      (*bin->child->DrawUpdate)( bin->child );
      bin->container->children_area.w = bin->child->rel_area.w;
      bin->container->children_area.h = bin->child->rel_area.h;
    }

    /* ... IF NO CHILD OR CHILD HIDEN */
  } else {

    bin->container->children_area.w = 0;
    bin->container->children_area.h = 0;

  }

  PROT__container_DrawUpdate( bin->container );

}

void PROT__bin_DrawBlit(   SDLGuiTK_Bin * bin )
{
  SDLGuiTK_Widget *widget=bin->object->widget;

  PROT__container_DrawBlit( bin->container );

  if( bin->child==NULL ) {
    SDLGUITK_LOG( "PROT__bin_DrawBlit(): 'child==NULL'\n" );
    return;
  }

  /* CHILD POSITION SUGGESTION  */
    bin->child->abs_area.x = \
      bin->container->children_area.x + widget->abs_area.x;
    bin->child->abs_area.y = \
      bin->container->children_area.y + widget->abs_area.y;

    bin->child->rel_area.x = \
      bin->container->children_area.x;
    bin->child->rel_area.y = \
      bin->container->children_area.y;

  /* CHILD SIZE SUGGESTION  */
  if( bin->child->shown==1 /* && bin->child->hided_parent==0 */ ) {
    bin->child->abs_area.w = bin->container->children_area.w;
    bin->child->abs_area.h = bin->container->children_area.h;
    (*bin->child->DrawBlit)( bin->child );
/*     SDL_BlitSurface( bin->child->srf, NULL, \ */
/* 		     widget->srf, &bin->child->rel_area ); */
/*     SDL_UpdateRects( widget->srf, 1, &bin->child->rel_area ); */
  } else {
    bin->child->abs_area.w = 0;
    bin->child->abs_area.h = 0;
  }

/*   SDL_mutexV( bin->child->object->mutex ); */

}


