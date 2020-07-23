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

#include "../debug.h"
#include "../mysdl.h"
#include "../myttf.h"
#include "../mycursor.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../signal.h"
//#include "../mywm.h"
#include "../theme_prot.h"
#include "../context_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"
#include "../wmwidget.h"
#include "panel_prot.h"


#define PANEL_BORDER_SIZE ((int) 1)


static SDL_bool SDLGuiTK_IS_PANEL( SDLGuiTK_Widget * widget )
{
  if( widget!=NULL ) {
    if( widget->container!=NULL ) {
      if( widget->container->bin!=NULL ) {
	if( widget->container->bin->panel!=NULL ) {
	  return SDL_TRUE;
	}
      }
    }
  }
  SDLGUITK_ERROR( "SDLGuiTK_IS_PANEL(): widget is not a panel\n" );
  return SDL_FALSE;
}

SDLGuiTK_Panel * SDLGuiTK_PANEL( SDLGuiTK_Widget * widget )
{
  if( !SDLGuiTK_IS_PANEL(widget) ) return NULL;
  return widget->container->bin->panel;
}


static int current_id=0;


static SDLGuiTK_Panel * Panel_create()
{
  SDLGuiTK_Panel * new_panel;

  new_panel = malloc( sizeof( struct SDLGuiTK_Panel ) );
  new_panel->bin = PROT__bin_new();
  new_panel->object = new_panel->bin->object;
  new_panel->bin->panel = new_panel;
  sprintf( new_panel->object->name, "panel%d", ++current_id );
  new_panel->object->widget->top = NULL;
  new_panel->object->widget->parent = NULL;

/*   strcpy( new_panel->title, new_panel->object->name ); */
  new_panel->position = SDLGUITK_PANEL_BOTTOM;

  new_panel->srf = MySDL_Surface_new ("Panel_srf");
  new_panel->area.x = 0; new_panel->area.y = 0;
  new_panel->area.w = 0; new_panel->area.h = 0;
/*   new_panel->title_srf = NULL; */
/*   new_panel->title_area.x = 0; new_panel->title_area.y = 0; */
/*   new_panel->title_area.w = 0; new_panel->title_area.h = 0; */

  new_panel->wm_widget = WMWidget_New( new_panel->object->widget );

  return new_panel;
}

static void Panel_destroy( SDLGuiTK_Panel * panel )
{
/*   MySDL_FreeSurface( panel->active_srf ); */
/*   MySDL_FreeSurface( panel->shaded_srf ); */
/*   MySDL_FreeSurface( panel->title_srf ); */
  MySDL_Surface_free( panel->srf );

  PROT__bin_destroy( panel->bin );
  WMWidget_Delete( panel->wm_widget );
  free( panel );
}


/* static void Panel_MakeTitleSurface( SDLGuiTK_Panel * panel ) */
/* { */
/*   SDLGuiTK_Theme * theme; */

/*   theme = PROT__theme_get_and_lock(); */
/*   panel->title_srf = MyTTF_Render_Blended( panel->title_srf, \ */
/* 					    panel->title, 18, \ */
/* 					    MyTTF_STYLE_BOLD, \ */
/* 					    theme->bgcolor ); */
/*   PROT__theme_unlock( theme ); */
/* } */

/* static void Panel_DrawTitleSurface( SDLGuiTK_Panel * panel ) */
/* { */
/*   Uint32 bdcolor; */
/*   SDLGuiTK_Theme * theme; */

/*   theme = PROT__theme_get_and_lock(); */
/*   bdcolor = SDL_MapRGBA( panel->srf->format, \ */
/* 			 theme->bdcolor.r, \ */
/* 			 theme->bdcolor.g, \ */
/* 			 theme->bdcolor.b, \ */
/* 			 255 ); */
/*   PROT__theme_unlock( theme ); */

/*   panel->title_area.x = 0; */
/*   panel->title_area.y = 0; */
/*   panel->title_area.w = panel->srf->w; */
/*   panel->title_area.h = panel->title_srf->h; */
/*   SDL_FillRect( panel->srf, &panel->title_area, bdcolor ); */
/*   SDL_UpdateRects( panel->srf, 1, &panel->title_area ); */

/*   panel->title_area.x = \ */
/*     ( panel->srf->w - panel->title_srf->w )/2; */
/*   panel->title_area.y = 0; */
/*   panel->title_area.w = panel->title_srf->w; */
/*   panel->title_area.h = panel->title_srf->h; */


/*   SDL_BlitSurface( panel->title_srf, NULL, \ */
/* 		   panel->srf, &panel->title_area ); */
/*   SDL_UpdateRect( panel->srf, 0, 0, 0, 0 ); */
/* } */


static void Panel_UpdatePosition( SDLGuiTK_Panel * panel )
{
  SDLGuiTK_Widget * widget=panel->object->widget;
  SDL_Surface * surface;

  surface = MySDL_GetVideoSurface();
  switch( panel->position ) {
  case SDLGUITK_PANEL_TOP:
    break;
  case SDLGUITK_PANEL_BOTTOM:
    widget->abs_area.x = 0;
    widget->abs_area.y = \
      surface->h - widget->abs_area.h;
    break;
  case SDLGUITK_PANEL_LEFT:
    
    break;
  case SDLGUITK_PANEL_RIGHT:
    
    break;
  default:
    break;
  }
}


static void Panel_MakeBaseSurface( SDLGuiTK_Panel * panel )
{
  //Uint32 bgcolor, bdcolor;
  SDLGuiTK_Theme * theme;

/*   Panel_MakeTitleSurface( panel ); */

  /* Prepare surface */
  MySDL_CreateRGBSurface(   panel->srf,\
			                panel->wm_widget->child_area.w, \
			                panel->wm_widget->child_area.h );

  /* Load theme values */
  theme = PROT__theme_get_and_lock();
  /* bdcolor = SDL_MapRGBA( panel->srf->format, \ */
		/* 	 theme->bdcolor.r, \ */
		/* 	 theme->bdcolor.g, \ */
		/* 	 theme->bdcolor.b, \ */
		/* 	 255 ); */
  /* bgcolor = SDL_MapRGBA( panel->srf->format, \ */
		/* 	 theme->bgcolor.r, \ */
		/* 	 theme->bgcolor.g, \ */
		/* 	 theme->bgcolor.b, \ */
		/* 	 255 ); */
  PROT__theme_unlock( theme );

/*   Panel_DrawTitleSurface( panel ); */

  /* Blit border, backgroud and title */
/*   SDL_FillRect( panel->active_srf, NULL, bdcolor ); */
/*   SDL_UpdateRect( panel->active_srf, 0, 0, 0, 0 ); */

  panel->area.x = 0;
  panel->area.y = 0;
  panel->area.w = panel->object->widget->abs_area.w;
  panel->area.h = panel->object->widget->abs_area.h;

/*   panel->wm_widget->area.y -= panel->title_srf->h; */

/*   SDL_FillRect( panel->active_srf, &panel->area, bgcolor ); */
/*   SDL_UpdateRect( panel->active_srf, 0, 0, 0, 0 ); */

/*   printf( " panel->area %d %d %d %d\n", panel->area.x, panel->area.y, panel->area.w, panel->area.h ); */

  MySDL_BlitSurface(    panel->object->widget->srf, NULL, \
		                panel->srf, &panel->area );
  //2SDL_UpdateRects( panel->srf, 1, &panel->area );
  //SDL_UpdateWindowSurface( panel->object->widget->srf );
}


static void Panel_MakeShadedSurface( SDLGuiTK_Panel * panel )
{
/*   panel->shaded_srf = MySDL_CopySurface_with_alpha( panel->shaded_srf, \ */
/* 						     panel->active_srf,\ */
/* 						     128 ); */
}


static void * Panel_DrawUpdate( SDLGuiTK_Widget * widget )
{
  SDLGuiTK_Panel * panel=widget->container->bin->panel;

/*   Panel_MakeTitleSurface( panel ); */

  /* First placement for widget */
  widget->parent = widget;
  widget->top = widget;

  PROT__bin_DrawUpdate( panel->bin );

/*   if( widget->rel_area.w<panel->title_srf->w ) { */
/*     widget->rel_area.w = panel->title_srf->w; */
/*     widget->abs_area.w = panel->title_srf->w; */
/*   } */
  panel->wm_widget->child_area.w = widget->abs_area.w;
  panel->wm_widget->child_area.h = widget->abs_area.h;

  panel->wm_widget->surface2D_flag = 1;

  WMWidget_DrawUpdate( panel->wm_widget );

  return (void *) NULL;
}


static void * Panel_DrawBlit( SDLGuiTK_Widget * widget )
{
  SDLGuiTK_Panel   * panel=widget->container->bin->panel;
  int wdiff=0, hdiff=0;

  if( panel->bin->child!=NULL ) {

    wdiff = \
      widget->abs_area.w - (2*widget->container->border_width) \
      - panel->bin->child->rel_area.w;
    hdiff = \
      widget->abs_area.h - (2*widget->container->border_width) \
      - panel->bin->child->rel_area.h;
    
    if( wdiff>0 ) {
      widget->container->children_area.w = \
	panel->bin->child->rel_area.w + wdiff;
      widget->container->children_area.x = widget->container->border_width;
    }
    if( hdiff>0 ) {
      widget->container->children_area.h = \
	panel->bin->child->rel_area.h + hdiff;
      widget->container->children_area.y = widget->container->border_width;
    }
  }

  PROT__bin_DrawBlit( panel->bin );

  if( panel->bin->child!=NULL ) {
/*     SDL_mutexP( panel->bin->child->object->mutex ); */
    if( panel->bin->child->shown==1 ) {
      MySDL_BlitSurface(    panel->bin->child->srf, NULL, \
		                    widget->srf, &panel->bin->child->rel_area );
      //2SDL_UpdateRects( widget->srf, 1, &panel->bin->child->rel_area );
	  //SDL_UpdateWindowSurface( panel->bin->child->srf );
    }
/*     SDL_mutexV( panel->bin->child->object->mutex ); */
  }

  Panel_MakeBaseSurface( panel );

  Panel_MakeShadedSurface( panel );

  widget->rel_area.w = widget->abs_area.w;
  widget->rel_area.h = widget->abs_area.h;

  widget->act_area.x = widget->abs_area.x;
  widget->act_area.y = widget->abs_area.y;
  widget->act_area.w = widget->abs_area.w;
  widget->act_area.h = widget->abs_area.h;

  WMWidget_DrawBlit( panel->wm_widget, panel->srf);

  //MySDL_BlitSurface(    panel->srf, NULL,
    //		                panel->wm_widget->srf, &panel->wm_widget->child_area );
  //2SDL_UpdateRects( panel->wm_widget->srf, 1, &panel->wm_widget->child_area );
  //SDL_UpdateWindowSurface( panel->srf );

  return (void *) NULL;
}


static SDLGuiTK_Widget * Panel_RecursiveEntering( SDLGuiTK_Widget * widget, \
						   int x, int y )
{
  SDLGuiTK_Widget * child;
  SDLGuiTK_Widget * active;

  child = SDLGuiTK_bin_get_child( widget->container->bin );
  if( child==NULL ) {
    SDLGUITK_ERROR( "Panel_RecursiveEntering(): child==NULL\n" );
    return NULL;
  }
  if( child->shown==0 ) {
    return NULL;
  }
  active = PROT__widget_is_entering( child, x, y );

  return active;
}

static void * Panel_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
  SDLGuiTK_Widget * child;

  PROT__context_unref_wmwidget( widget->container->bin->panel->wm_widget );
  child = SDLGuiTK_bin_get_child( widget->container->bin );
/*   PROT__bin_remove( widget->container->bin, child ); */

  if( child!=NULL ) {
    SDLGuiTK_widget_destroy( child );
  }

  return (void *) NULL;
}

static void * Panel_Free( SDLGuiTK_Widget * widget )
{
  Panel_destroy( widget->container->bin->panel );

  return (void *) NULL;
}




static void * Panel_Realize( SDLGuiTK_Widget * widget, \
			      void * data, void * event )
{

  Panel_UpdatePosition( widget->container->bin->panel );
  Panel_DrawUpdate( widget );
  Panel_DrawBlit( widget );

  return (void *) NULL;
}

static void * Panel_Destroy( SDLGuiTK_Widget * widget, \
			      void * data, void * event )
{
/*   SDLGuiTK_Panel * panel=widget->container->bin->panel; */

/*   SDL_mutexV( widget->object->mutex ); */
/*   PROT__context_unref_wmwidget( panel->wm_widget ); */
/*   SDL_mutexP( widget->object->mutex ); */
  widget->shown = 0;

  return (void *) NULL;
}

static void * Panel_Show( SDLGuiTK_Widget * widget, \
			   void * data, void * event )
{
  SDLGuiTK_Panel * panel=widget->container->bin->panel;

  if( widget->shown==1 ) return (void *) NULL;
  widget->shown = 1;
  Panel_UpdatePosition( panel );
/*   SDL_mutexV( widget->object->mutex ); */
  PROT__context_ref_wmwidget( panel->wm_widget );
/*   SDL_mutexP( widget->object->mutex ); */

  return (void *) NULL;
}

static void * Panel_Hide( SDLGuiTK_Widget * widget, \
			   void * data, void * event )
{
  SDLGuiTK_Panel * panel=widget->container->bin->panel;

  if( widget->shown==0 ) return (void *) NULL;
/*   SDL_mutexV( widget->object->mutex ); */
  PROT__context_unref_wmwidget( panel->wm_widget );
/*   SDL_mutexP( widget->object->mutex ); */
  widget->shown = 0;

  return (void *) NULL;
}

static void * Panel_FrameEvent( SDLGuiTK_Widget * widget, \
				 void * data, void * event )
{
  Panel_DrawUpdate( widget );
  Panel_DrawBlit( widget );
  //PROT_MyWM_checkactive( widget ); //TODO: wm call here ?

  return (void *) NULL;
}

static void * Panel_MouseEnter( SDLGuiTK_Widget * widget, \
				 void * data, void * event )
{
/*   widget->WM_srf = widget->container->bin->panel->active_srf; */
/*   if( widget->absolute_srf!=widget->container->bin->panel->active_srf ) { */
/*     widget->absolute_srf = widget->container->bin->panel->active_srf; */
/*   } */
/*   PROT__box_activate( panel->panel->main_box->box ); */
  MyCursor_Set( SDLGUITK_CURSOR_DEFAULT );

  return (void *) NULL;
}

static void * Panel_MouseLeave( SDLGuiTK_Widget * widget, \
				 void * data, void * event )
{
/*   widget->WM_srf = widget->container->bin->panel->shaded_srf; */
/*   if( widget->absolute_srf!=widget->container->bin->panel->shaded_srf ) { */
/*     widget->absolute_srf = widget->container->bin->panel->shaded_srf; */
/*   } */
/*   PROT__box_update_relwin( panel->panel->main_box->box ); */
/*   Intern__SDLGuiTK_events_unref_widgets(); */
  MyCursor_Unset();

  return (void *) NULL;
}



/* static void * Panel_MousePressed( SDLGuiTK_Widget * panel, \ */
/* 				   void * data ) */
/* { */
/*   int x, y; */
/*   if( SDL_GetMouseState( &x, &y) ) { */
/*     printf( "Mouse pressed in: x=%d y=%d\n", x, y ); */
/*     panel->moving = 1; */
/*   } */

/*   return (void *) NULL; */
/* } */

static void * Panel_MouseReleased( SDLGuiTK_Widget * panel, \
				    void * data, void * event )
{
/*   int x, y; */

/*   if( SDL_GetMouseState( &x, &y) ) { */
/*     printf( "Mouse released in: x=%d y=%d\n", x, y ); */
/*   } */
/*   printf( "Mouse released in: x=%d y=%d\n", x, y ); */
/*   PROT__box_update_relwin( panel->panel->main_box->box ); */
  Panel_DrawUpdate( panel );
  Panel_DrawBlit( panel );

  return (void *) NULL;
}


static void Panel_setsignals( SDLGuiTK_Panel * panel )
{
  SDLGuiTK_SignalHandler * handler;

  handler = (SDLGuiTK_SignalHandler *) panel->object->signalhandler;

  handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
    Panel_Realize;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_DESTROY]->function = \
    Panel_Destroy;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
    Panel_Show;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
    Panel_Hide;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_FRAMEEVENT]->function = \
    Panel_FrameEvent;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_ENTER]->function = \
    Panel_MouseEnter;
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_LEAVE]->function = \
    Panel_MouseLeave;

/*   handler->fdefault[SDLGUITK_SIGNAL_TYPE_PRESSED]->function = \ */
/*     Panel_MousePressed; */
  handler->fdefault[SDLGUITK_SIGNAL_TYPE_RELEASED]->function = \
    Panel_MouseReleased;
}

SDLGuiTK_Widget * SDLGuiTK_panel_new()
{
  SDLGuiTK_Panel * panel;

  panel = Panel_create();

  panel->object->widget->RecursiveEntering = Panel_RecursiveEntering;
  panel->object->widget->RecursiveDestroy = Panel_RecursiveDestroy;
  panel->object->widget->Free = Panel_Free;

  panel->object->widget->DrawUpdate = Panel_DrawUpdate;
  panel->object->widget->DrawBlit = Panel_DrawBlit;

  Panel_setsignals( panel );

  panel->object->widget->parent = NULL;
  panel->object->widget->top = NULL;

  PROT__signal_push( panel->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

  return panel->object->widget;
}

/* void SDLGuiTK_panel_set_title( SDLGuiTK_Widget * panel,\ */
/* 				const char *title ) */
/* { */
/* /\*   SDL_mutexP( panel->object->mutex ); *\/ */
/*   strcpy( panel->container->bin->panel->title, title ); */
/* /\*   SDL_mutexV( panel->object->mutex ); *\/ */

/*   PROT__signal_push( panel->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT ); */
/* } */

void SDLGuiTK_panel_set_position( SDLGuiTK_Widget * panel, \
				   int position, \
				   void *data )
{
/*   SDL_mutexP( panel->object->mutex ); */

  panel->container->bin->panel->position = position;
  PROT__signal_push( panel->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );

/*   SDL_mutexV( panel->object->mutex ); */
}

