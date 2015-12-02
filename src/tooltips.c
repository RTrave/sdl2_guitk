/* 
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

   Copyright (C) 2007 Trave Roman

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
#include "myttf.h"
#include "mycursor.h"
#include "theme_prot.h"
#include "object_prot.h"
#include "widget_prot.h"

#include "tooltips_prot.h"


static SDLGuiTK_List *tooltips_list=NULL;


SDLGuiTK_Tooltips *SDLGuiTK_TOOLTIPS( SDLGuiTK_Tooltips *tooltips ) {
  return tooltips; /* ??? */
}


SDLGuiTK_Tooltips *SDLGuiTK_tooltips_new() {

  SDLGuiTK_Tooltips * new_tooltips;

  new_tooltips = malloc( sizeof( struct SDLGuiTK_Tooltips ) );
  new_tooltips->object = PROT__object_new();
  new_tooltips->object->tooltips = new_tooltips;
  new_tooltips->enabled = 1;
  new_tooltips->srf = NULL;
  new_tooltips->surface2D = MySDL_surface2D_new();

  if( !tooltips_list ) tooltips_list = SDLGuiTK_list_new();
  SDLGuiTK_list_append( tooltips_list, (SDLGuiTK_Object *) new_tooltips );

  return new_tooltips;
}

static void Tooltips_Destroy( SDLGuiTK_Tooltips *tooltips ) {

  MySDL_surface2D_destroy( tooltips->surface2D );
  PROT__object_destroy( tooltips->object );
  free(tooltips);
}

void SDLGuiTK_tooltips_enable ( SDLGuiTK_Tooltips *tooltips ) {

  tooltips->enabled = 1;
}

void SDLGuiTK_tooltips_disable ( SDLGuiTK_Tooltips *tooltips ) {

  tooltips->enabled = 0;
}

static SDLGuiTK_TooltipsData * TTData_Create() {
  SDLGuiTK_TooltipsData *new_ttdata;
  new_ttdata = malloc( sizeof(struct SDLGuiTK_TooltipsData) );
  new_ttdata->object = PROT__object_new();
  return new_ttdata;
}

static void TTData_Destroy( SDLGuiTK_TooltipsData *ttdata ) {
  PROT__object_destroy( ttdata->object );
  free( ttdata );
}


void SDLGuiTK_tooltips_set_tip   (SDLGuiTK_Tooltips *tooltips,
				  SDLGuiTK_Widget *widget,
				  const char *tip_text,
				  const char *tip_private) {
  if( widget->tooltipsdata ) TTData_Destroy( widget->tooltipsdata );
  widget->tooltipsdata = TTData_Create();
  widget->tooltipsdata->tooltips = tooltips;
  widget->tooltipsdata->tip_text = tip_text;
  widget->tooltipsdata->widget = widget;
  widget->tooltipsdata->updated = 0;
}

int
PROT__TooltipsData_update( SDLGuiTK_TooltipsData *tooltipsdata ) {
  SDLGuiTK_Theme * theme;
  if( tooltipsdata->tooltips->enabled==0 ) return 0;
  if( tooltipsdata->updated==1 ) return 0;

  theme = PROT__theme_get_and_lock();
  tooltipsdata->tooltips->srf = MyTTF_Render_Solid_Block( tooltipsdata->tooltips->srf, \
					 (char *) tooltipsdata->tip_text, \
					 10, MyTTF_STYLE_ITALIC, \
					 theme->ftcolor, \
					 40 );
  PROT__theme_unlock( theme );
  

  tooltipsdata->updated = 1;
  return 1;
}


void 
PROT__Tooltips_DestroyAll() {
  SDLGuiTK_Tooltips *tooltips;
  if( !tooltips_list ) return;
  tooltips = (SDLGuiTK_Tooltips *) SDLGuiTK_list_pop_tail( tooltips_list );
  while(tooltips) {
    Tooltips_Destroy( tooltips );
    tooltips = (SDLGuiTK_Tooltips *) SDLGuiTK_list_pop_tail( tooltips_list );
  }
  SDLGuiTK_list_destroy( tooltips_list );
  tooltips_list = NULL;
}

void 
PROT__TooltipsData_Destroy( SDLGuiTK_TooltipsData *tooltipsdata ) {

  TTData_Destroy( tooltipsdata );
}

