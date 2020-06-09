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

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <SDL2/SDL_guitk.h>

#include "../debug.h"
#include "../mysdl.h"
#include "../myttf.h"
#include "../signal.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../theme_prot.h"
#include "../misc_prot.h"

#include "label_prot.h"


static SDL_bool SDLGuiTK_IS_LABEL( SDLGuiTK_Widget * widget )
{
  if( widget!=NULL ) {
    if( widget->misc!=NULL ) {
      if( widget->misc->label!=NULL ) {
	return SDL_TRUE;
      }
    }
  }
  SDLGUITK_ERROR( "SDLGuiTK_IS_LABEL(): widget is not a label\n" );
  return SDL_FALSE;
}

SDLGuiTK_Label * SDLGuiTK_LABEL( SDLGuiTK_Widget * widget )
{
  if( !SDLGuiTK_IS_LABEL(widget) ) return NULL;
  return widget->misc->label;
}


static int current_id=0;


static SDLGuiTK_Label * Label_create()
{
  SDLGuiTK_Label * new_label;

  new_label = malloc( sizeof( struct SDLGuiTK_Label ) );
  new_label->misc = PROT__misc_new();
  new_label->misc->label = new_label;
  new_label->object = new_label->misc->object;
  sprintf( new_label->object->name, "label%d", ++current_id );

  new_label->text = calloc( 256, sizeof( char ) );
  strcpy( new_label->text, new_label->object->name );

  new_label->text_flag = 1;
  //new_label->text_srf = NULL;
  new_label->text_area.x = 0; new_label->text_area.y = 0;
  new_label->text_area.w = 0; new_label->text_area.h = 0;
  new_label->srf = NULL;

  return new_label;
}

static void Label_destroy( SDLGuiTK_Label * label )
{
  //MySDL_FreeSurface( label->text_srf );
  MySDL_FreeSurface( label->srf );

  PROT__misc_destroy( label->misc );
  free( label->text );
  free( label );
}


static void Label_make_surface( SDLGuiTK_Label * label )
{
  SDLGuiTK_Theme * theme;

  theme = PROT__theme_get_and_lock();
  label->srf = MyTTF_Render_Solid_Block( label->srf, \
					 label->text, \
					 16, MyTTF_STYLE_NORMAL, \
					 theme->ftcolor, \
					 40 );
  PROT__theme_unlock( theme );

  label->text_flag = 0;
  //label->srf = MySDL_CopySurface( label->srf, label->text_srf );
}


static void * Label_DrawUpdate( SDLGuiTK_Widget * widget )
{
  SDLGuiTK_Label * label=widget->misc->label;

    PROT__widget_reset_req_area( widget );
  Label_make_surface( label );
  
  label->misc->area.w = label->srf->w;
  label->misc->area.h = label->srf->h;

  PROT__misc_DrawUpdate( label->misc );

  return (void *) NULL;
}


static void * Label_active_area( SDLGuiTK_Label * label )
{
  SDLGuiTK_Widget * widget=label->misc->widget;

  widget->act_area.x = \
    widget->abs_area.x; // + label->misc->area.x;
  widget->act_area.y = \
    widget->abs_area.y; // + label->misc->area.y;
  widget->act_area.w = widget->abs_area.w;  /* label->srf->w */
  widget->act_area.h = widget->abs_area.h;  /* label->srf->h */

  return (void *) NULL;
}

static void * Label_DrawBlit( SDLGuiTK_Widget * widget )
{
  SDLGuiTK_Label * label=widget->misc->label;

  PROT__misc_DrawBlit( label->misc );

  //label->srf = MySDL_CopySurface( label->srf, label->text_srf );

  Label_active_area( label );

  //widget->rel_area.w = widget->abs_area.w;
  //widget->rel_area.h = widget->abs_area.h;

  if( widget->top!=NULL ) {
    SDL_BlitSurface( label->srf, NULL, \
		     widget->srf, &label->misc->area );
    //SDL_UpdateRect( widget->srf, 0, 0, 0, 0 );
  }

  return (void *) NULL;
}

static void * Label_Realize( SDLGuiTK_Widget * widget, \
			     void * data, void * event )
{
  if( widget->misc->label->text_flag!=0 ) {
    Label_make_surface( widget->misc->label );
  }

  return (void *) NULL;
}

static void * Label_Show( SDLGuiTK_Widget * widget, \
			  void * data, void * event )
{
  widget->shown = 1;
  if( widget->top!=NULL ) {
    PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
  }

  return (void *) NULL;
}

static void * Label_Hide( SDLGuiTK_Widget * widget, \
			  void * data, void * event )
{
  widget->shown = 0;
  if( widget->top!=NULL ) {
    PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
  }

  return (void *) NULL;
}

static SDLGuiTK_Widget * Label_RecursiveEntering( SDLGuiTK_Widget * widget, \
						  int x, int y )
{

  return NULL;
}

static void * Label_RecursiveDestroy( SDLGuiTK_Widget * widget )
{

  return (void *) NULL;
}

static void * Label_Free( SDLGuiTK_Widget * widget )
{
  Label_destroy( widget->misc->label );
  
  return (void *) NULL;
}


static void Label_set_functions( SDLGuiTK_Label * label )
{
  SDLGuiTK_SignalHandler * handler;

  handler = (SDLGuiTK_SignalHandler *) label->object->signalhandler;

  label->object->widget->RecursiveEntering = Label_RecursiveEntering;
  label->object->widget->RecursiveDestroy = Label_RecursiveDestroy;
  label->object->widget->Free = Label_Free;

  label->object->widget->DrawUpdate = Label_DrawUpdate;
  label->object->widget->DrawBlit = Label_DrawBlit;

  handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
    Label_Realize;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
    Label_Show;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
    Label_Hide;
}



SDLGuiTK_Widget * SDLGuiTK_label_new( const char *str )
{
  SDLGuiTK_Label * label;

  label = Label_create();
    //SDLGuiTK_misc_set_padding( label->misc, 5, 5);
  strcpy( label->text, str );
  Label_set_functions( label );
  PROT__signal_push( label->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

  return label->object->widget;
}

void SDLGuiTK_label_set_text( SDLGuiTK_Widget * label, const char *str )
{
/*   SDL_mutexP( label->object->mutex ); */
  strcpy( label->misc->label->text, str );
  label->misc->label->text_flag = 1;
/*   SDL_mutexV( label->object->mutex ); */
  PROT__signal_push( label->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
}

