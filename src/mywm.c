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

#include "debug.h"
#include "mysdl.h"
#include "mywm.h"
#include "signal.h"
#include "list.h"
#include "theme_prot.h"
#include "context_prot.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "tooltips_prot.h"

/* #include "windows/window_prot.h" */


/* static int         current_id=1; */
/* static SDL_mutex * current_id_mutex=NULL; */

/* static SDLGuiTK_WMWidget * active_wmwidget=NULL; */
static SDLGuiTK_Widget    * active_widget=NULL;
static SDLGuiTK_Surface2D * active_2D=NULL;



SDLGuiTK_WMWidget * MyWM_WMWidget_New( SDLGuiTK_Widget * widget )
{
  SDLGuiTK_WMWidget * wm_widget;

  wm_widget = malloc( sizeof( struct SDLGuiTK_WMWidget ) );
  wm_widget->widget = widget;
  wm_widget->object = widget->object;

  wm_widget->border_width = 3;
  wm_widget->title_shown = 0;

/*   wm_widget->active = NULL; */
  wm_widget->enter = 0;
  wm_widget->moving = 0;
  wm_widget->srf = NULL;
  wm_widget->area.x = 0; wm_widget->area.y = 0;
  wm_widget->area.w = 0; wm_widget->area.h = 0;
  wm_widget->child_area.x = 0; wm_widget->child_area.y = 0;
  wm_widget->child_area.w = 0; wm_widget->child_area.h = 0;

  wm_widget->surface2D = NULL;
  wm_widget->surface2D_flag = 0;
/*   wm_widget->wmwidget_2D = MyWM_2DWidget_new(); */
/*   wm_widget->active_2D = MyWM_2DWidget_new(); */

  return wm_widget;
}

void              MyWM_WMWidget_Delete( SDLGuiTK_WMWidget * wm_widget )
{
  MySDL_FreeSurface( wm_widget->srf );
/*   MyWM_2DWidget_destroy( wm_widget->wmwidget_2D ); */
/*   MyWM_2DWidget_destroy( wm_widget->active_2D ); */
  free( wm_widget );
}


void MyWM_WMWidget_DrawUpdate( SDLGuiTK_WMWidget * wm_widget )
{
/*   SDLGuiTK_Widget * widget=wm_widget->widget; */

  wm_widget->child_area.x = wm_widget->border_width;
  wm_widget->child_area.y = wm_widget->border_width;

  wm_widget->area.x = wm_widget->widget->abs_area.x - wm_widget->border_width - wm_widget->widget->rel_area.x;
  wm_widget->area.y = wm_widget->widget->abs_area.y - wm_widget->border_width - wm_widget->widget->rel_area.y;
  wm_widget->area.w = wm_widget->child_area.w + 2*wm_widget->border_width;
  wm_widget->area.h = wm_widget->child_area.h + 2*wm_widget->border_width;

/*   if( widget->abs_area.x<(wm_widget->border_width) ) { */
/*     widget->abs_area.x = wm_widget->border_width; */
/*   } */
/*   widget->rel_area.x = wm_widget->border_width; */
/*   if( widget->abs_area.y<(wm_widget->border_width) ) { */
/*     widget->abs_area.y = wm_widget->border_width; */
/*   } */
/*   widget->rel_area.y = wm_widget->border_width; */
}

void MyWM_WMWidget_DrawBlit( SDLGuiTK_WMWidget * wm_widget )
{
/*   SDLGuiTK_Widget * widget=wm_widget->widget; */
  //Uint32 bgcolor;
  Uint32 bdcolor;
  SDLGuiTK_Theme * theme;

  wm_widget->srf = \
    MySDL_CreateRGBSurface( wm_widget->srf,\
			    wm_widget->area.w, wm_widget->area.h );

  /* Load theme values */
  theme = PROT__theme_get_and_lock();
  bdcolor = SDL_MapRGBA( wm_widget->srf->format, \
			 theme->bdcolor.r, \
			 theme->bdcolor.g, \
			 theme->bdcolor.b, \
			 255 );
  /* bgcolor = SDL_MapRGBA( wm_widget->srf->format, \ */
		/* 	 theme->bgcolor.r, \ */
		/* 	 theme->bgcolor.g, \ */
		/* 	 theme->bgcolor.b, \ */
		/* 	 255 ); */
  PROT__theme_unlock( theme );


  /* Blit border, backgroud and title */
  SDL_FillRect( wm_widget->srf, NULL, bdcolor );
  //SDL_UpdateRect( wm_widget->srf, 0, 0, 0, 0 );

/*   wm_widget->area.x = wm_widget->widget->abs_area.x - wm_widget->border_width - wm_widget->widget->rel_area.x; */
/*   wm_widget->area.y = wm_widget->widget->abs_area.y - wm_widget->border_width - wm_widget->widget->rel_area.y; */
}







/* return 1 if mouse move in the widget, or 0. */
/* static int Is_MOUSEMOTION_entering( SDL_Event *event, \ */
/* 				    SDLGuiTK_Widget * widget ) */
/* { */
/*   if( ( (event->motion.x)>=(widget->act_area.x) && */
/*         event->motion.x<=(widget->act_area.x+ \ */
/* 			  widget->act_area.w) ) && */
/*       ( event->motion.y>=widget->act_area.y && */
/*         event->motion.y<=(widget->act_area.y+ \ */
/* 			  widget->act_area.h) ) ) */
/*     { */
/*       if( widget->enter==0 ) { */
/* 	if( active_widget!=NULL ) { */
/* 	  if( widget!=active_widget ) { */
/* 	    SDL_mutexP( active_widget->object->mutex ); */
/* 	    active_widget->enter = 0; */
/* 	    PROT__signal_push( active_widget->object, SDLGUITK_SIGNAL_TYPE_LEAVE ); */
/* 	    SDL_mutexV( active_widget->object->mutex ); */
/* 	  } */
/* 	} */
/* 	active_widget = widget; */
/*         widget->enter = 1; */
/* 	PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_ENTER ); */
/*       } */
/*       return 1; */
/*     } */
/*   return 0; */
/* } */


/* static SDLGuiTK_List * activable_widgets=NULL; */

static void Activables_check( int x, int y )
{
  SDLGuiTK_Widget * current, * parent;

  if( active_widget!=NULL ) {
    if( (current=PROT__widget_is_entering(active_widget,x,y))==NULL ) {
      while( active_widget->parent!=active_widget && current==NULL ) {
	parent = active_widget->parent;
	active_widget = parent;
	current=PROT__widget_is_entering(active_widget,x,y);
      }
      active_widget = current;
      return;
    } else {
      active_widget = current;
      return;
    }
  }

  if( current_context->active_wmwidget!=NULL ) {
    current = 
      PROT__widget_is_entering(current_context->active_wmwidget->widget, x, y);
    if( current==NULL ) {
      
    } else {
      active_widget = current;
    }
  }

}

/* static void Activables_load( SDLGuiTK_Widget * widget ) */
/* { */
/*   SDLGuiTK_Window * window; */

/*   if( (window=SDLGuiTK_WINDOW(widget))!=NULL ) { */
/*     activables = window->activables; */
/*   } */
/*   if( active!=NULL ) { */
/*       SDL_mutexP( active->object->mutex ); */
/*       PROT__signal_push( active->object, \ */
/* 			 SDLGUITK_SIGNAL_TYPE_LEAVE ); */
/*       SDL_mutexV( active->object->mutex ); */
/*       active = NULL; */
/*   } */
  
/* } */

/* static void Activables_unload( SDLGuiTK_Widget * window ) */
/* { */
/*   activables = NULL; */
/*   if( active!=NULL ) { */
/*       SDL_mutexP( active->object->mutex ); */
/*       PROT__signal_push( active->object, \ */
/* 			 SDLGUITK_SIGNAL_TYPE_LEAVE ); */
/*       SDL_mutexV( active->object->mutex ); */
/*       active = NULL; */
/*   } */
/* } */

static void Desactive_WMWidget( SDLGuiTK_WMWidget * wm_widget )
{
  SDLGuiTK_Widget * current, * parent=NULL;

/*   SDL_mutexP( wm_widget->widget->object->mutex ); */
  current = active_widget;
  while( current!=NULL ) {
/*     SDL_mutexP( current->object->mutex ); */
    parent = current->parent;
    current->enter = 0;
    PROT__signal_push( current->object, \
		       SDLGUITK_SIGNAL_TYPE_LEAVE );
/*     SDL_mutexV( current->object->mutex ); */
    if( current==parent ) return;
    current = parent;
  }
/*   wm_widget->enter = 0; */
/*   SDL_mutexV( wm_widget->widget->object->mutex ); */
}

static int Is_MOUSEMOTION_in( SDLGuiTK_WMWidget * wm_widget, int x, int y ) {
  if( ( x>=wm_widget->area.x &&
        x<=(wm_widget->area.x+wm_widget->area.w) ) &&
      ( y>=wm_widget->area.y &&
        y<=(wm_widget->area.y+wm_widget->area.h) ) )
    {
      return 1;
    }
  return 0;
}
static int Is_MOUSEMOTION_enter( int x, int y,				\
				 SDLGuiTK_WMWidget * wm_widget )
{
/*   SDLGuiTK_WMWidget * active; */
/*   active = (SDLGuiTK_WMWidget *) current_context->active_wmwidget; */

/*   if( ( event->motion.x>=wm_widget->area.x && */
/*         event->motion.x<=(wm_widget->area.x+wm_widget->area.w) ) && */
/*       ( event->motion.y>=wm_widget->area.y && */
/*         event->motion.y<=(wm_widget->area.y+wm_widget->area.h) ) ) */
  if( Is_MOUSEMOTION_in( wm_widget, x, y )==1 ) 
    {
      if( wm_widget->enter==0 ) {
	if(current_context->active_wmwidget!=wm_widget ) {
	  if( current_context->active_wmwidget!=NULL ) {
	    Desactive_WMWidget( current_context->active_wmwidget );
	    active_widget = NULL;
	    current_context->active_wmwidget->enter = 0;
/* 	    PROT__signal_push( current_context->active_wmwidget->widget->object, \ */
/* 			       SDLGUITK_SIGNAL_TYPE_LEAVE ); */
	  }
	}
	current_context->active_wmwidget = wm_widget;
        wm_widget->enter = 1;
/* 	PROT__signal_push( current_context->active_wmwidget->widget->object, \ */
/* 			   SDLGUITK_SIGNAL_TYPE_ENTER ); */
	Activables_check(  x, y );
      } else {
	Activables_check( x, y );
      }
      return 1;
    } else {
    wm_widget->enter = 0;
     Activables_check( x, y );
    }
  return 0;
}

static int Is_MOUSEMOTION_moving( SDL_Event * event )
{
  SDLGuiTK_WMWidget * active=current_context->active_wmwidget;

  if( active->moving==1 ) {
    active->area.x += event->motion.xrel;
    active->area.y += event->motion.yrel;
    active->widget->abs_area.x += event->motion.xrel;
    active->widget->abs_area.y += event->motion.yrel;
/*     printf("TEST1\n"); */
    return 1;
  }
/*     printf("TEST2\n"); */

  return 0;
}

static int Is_MOUSEBUTTONDOWN_move_start( SDL_Event * event, \
					  SDLGuiTK_WMWidget * window )
{
  if( event->button.x>=window->area.x && \
      event->button.x<=window->area.x+window->area.w && \
      event->button.y>=window->area.y && \
      event->button.y<=window->area.y+window->area.h )
    {
      window->moving = 1;
      return 1;
    }
  return 0;
}

static int Is_MOUSEBUTTONDOWN_move_stop( SDLGuiTK_WMWidget * window )
{
  if( window->moving==1 ) {
    window->moving = 0;
/*     Intern__SDLGuiTK_window_update_relwin( window ); */
    return 1;
  }
  return 0;
}

static void LeaveAll() {
  
  if( active_widget!=NULL ) {
    PROT__signal_push( active_widget->object,		\
		       SDLGUITK_SIGNAL_TYPE_LEAVE );
    active_widget->enter = 0;
    while(active_widget->top!=active_widget  ) {
      if( active_widget->parent==NULL ) {
	active_widget = active_widget->top;
      } else {
	active_widget = active_widget->parent;
      }
      PROT__signal_push( active_widget->object,			\
			 SDLGUITK_SIGNAL_TYPE_LEAVE );
      active_widget->enter = 0;
    }
    active_widget = NULL;
  }
  if( current_context->active_wmwidget!=NULL ) {
    current_context->active_wmwidget->enter = 0;
    current_context->active_wmwidget = NULL;
  }  
}

int MyWM_push_MOUSEMOTION_in( int x, int y )
{
  SDLGuiTK_WMWidget * current=NULL;

  SDLGuiTK_list_lock( current_context->activables );

  if( current_context->active_wmwidget!=NULL ) {

/*     if( Is_MOUSEMOTION_moving(event)==1 ) { */
/*       SDLGuiTK_list_unlock( current_context->activables ); */
/*       return 1; */
/*     } */

    current = (SDLGuiTK_WMWidget *) \
      SDLGuiTK_list_refrv_init( current_context->activables );
    while( current!=NULL && current!=current_context->active_wmwidget ) {
      if( Is_MOUSEMOTION_enter(x,y,current)==1 ) {
	Activables_check( x, y );
	PROT_List_refrv_reinit( current_context->activables );
	SDLGuiTK_list_unlock( current_context->activables );
	return 1;
      }
      current = (SDLGuiTK_WMWidget *) \
	SDLGuiTK_list_refrv_next( current_context->activables );
    }

    if( Is_MOUSEMOTION_enter(x,y,current_context->active_wmwidget)==0 ) {
      current_context->active_wmwidget->enter = 0;
      current_context->active_wmwidget = NULL;
      Activables_check( x, y );
    } else {
    }
  }

  PROT_List_refrv_reinit( current_context->activables );

  current = (SDLGuiTK_WMWidget *) \
    SDLGuiTK_list_refrv_init( current_context->activables );

  while( current!=NULL ) {
/*     printf("Check %s ...\n", current->widget->object->name); */
    if( Is_MOUSEMOTION_enter(x,y,current)==1 ) {
/*       printf("OK\n"); */
      current_context->active_wmwidget = current;
/*       current_context->active_wmwidget->enter = 1; */
/*       PROT__signal_push( current->widget->object,		\ */
/* 			 SDLGUITK_SIGNAL_TYPE_ENTER ); */
      Activables_check( x, y );
      PROT_List_refrv_reinit( current_context->activables );
      SDLGuiTK_list_unlock( current_context->activables );
      return 1;
    } else {
/*       printf("no\n"); */
    }
    current = (SDLGuiTK_WMWidget *) \
      SDLGuiTK_list_refrv_next( current_context->activables );
  }

  SDLGuiTK_list_unlock( current_context->activables );
  return 0;
}

/* void PROT_MyWM_checkactive( SDLGuiTK_Widget * widget ) */
/* { */
/*   if( active_widget!=NULL ) { */
/*     if( active_widget->top==widget ) { */
/*       PROT__signal_push( active_widget->object, \ */
/* 			 SDLGUITK_SIGNAL_TYPE_LEAVE ); */
/*       active_widget->enter = 0; */
/*       active_widget = NULL; */
/*     } */
/*   } */
/* } */

void PROT_MyWM_leaveall()
{
  LeaveAll();
}

void PROT_MyWM_checkactive( SDLGuiTK_Widget * widget )
{
  int x, y;
  SDL_GetMouseState( &x, &y );
  MyWM_push_MOUSEMOTION_in( x, y );
}

int MyWM_push_MOUSEMOTION( SDL_Event *event )
{
  SDLGuiTK_list_lock( current_context->activables );
  if( current_context->active_wmwidget!=NULL ) {

    if( Is_MOUSEMOTION_moving(event)==1 ) {
      SDLGuiTK_list_unlock( current_context->activables );
      return 1;
    }

  }
  SDLGuiTK_list_unlock( current_context->activables );
  return MyWM_push_MOUSEMOTION_in( event->motion.x, event->motion.y );
}

int MyWM_push_MOUSEBUTTONDOWN( SDL_Event *event )
{
  if( active_widget!=NULL ) {
    PROT__signal_push( active_widget->object, \
		       SDLGUITK_SIGNAL_TYPE_PRESSED );
    return 1;
  }

  SDLGuiTK_list_lock( current_context->activables );
  SDLGuiTK_list_remove( current_context->activables, \
			(SDLGuiTK_Object *) current_context->active_wmwidget );
  SDLGuiTK_list_append( current_context->activables, \
			(SDLGuiTK_Object *) current_context->active_wmwidget );
  SDLGuiTK_list_unlock( current_context->activables );
  

  if( Is_MOUSEBUTTONDOWN_move_start(event,current_context->active_wmwidget)==1 )
    {
      PROT__signal_push( current_context->active_wmwidget->object, \
			 SDLGUITK_SIGNAL_TYPE_PRESSED );
      return 1;
    }

  return 0;
}

int MyWM_push_MOUSEBUTTONUP( SDL_Event *event )
{
  if( active_widget!=NULL ) {
    PROT__signal_push( active_widget->object, \
		       SDLGUITK_SIGNAL_TYPE_RELEASED );
    return 1;
  }
  if( current_context->active_wmwidget!=NULL ) {
    PROT__signal_push( current_context->active_wmwidget->widget->object, \
		       SDLGUITK_SIGNAL_TYPE_RELEASED );
    return Is_MOUSEBUTTONDOWN_move_stop( current_context->active_wmwidget );
  }
  return 0;
}

int MyWM_push_TEXTINPUT( SDL_Event *event )
{
  if( active_widget!=NULL ) {
    PROT__signal_textinput( active_widget->object, \
			  SDLGUITK_SIGNAL_TYPE_TEXTINPUT, \
			  &event->text );
/*     printf( "Test Key %d\n", event->key.keysym.sym ); */
    return 1;
  }
  return 0;
}

int MyWM_push_KEYDOWN( SDL_Event *event )
{
  if( active_widget!=NULL ) {
    PROT__signal_pushkey( active_widget->object, \
			  SDLGUITK_SIGNAL_TYPE_KEYBOARD, \
			  &event->key );
/*     printf( "Test Key %d\n", event->key.keysym.sym ); */
    return 1;
  }
/*   if( current_context->active_window!=NULL ) { */
/*     PROT__signal_push( current_context->active_window->object, \ */
/* 		       SDLGUITK_SIGNAL_TYPE_RELEASED ); */
/*     return Is_MOUSEBUTTONDOWN_move_stop( current_context->active_window ); */
/*   } */
  return 0;
}




/* void MyWM_append_activable( SDLGuiTK_Widget * widget ) */
/* { */
/*   SDLGuiTK_Window * window; */

/*   if( widget->top!=NULL ) { */
/*     if( (window=SDLGuiTK_WINDOW(widget->top))!=NULL ) { */
/*       PROT__window_append_activable( window, widget ); */
/*     } */
/*   } */
/* } */



#ifdef HAVE_GL_GL_H

static void * MyWM_blitsurface_GL( SDLGuiTK_WMWidget * wm_widget )
{
  SDLGuiTK_Surface2D * surface2D=wm_widget->surface2D;
  int mouse_x, mouse_y;
  SDL_GetMouseState( &mouse_x, &mouse_y );
 
  if ( wm_widget->surface2D_flag!=0 ) {
    MySDL_surface2D_update( surface2D, wm_widget->srf );
    wm_widget->surface2D_flag = 0;
  }

  MySDL_surface2D_blitsurface( surface2D, \
			       wm_widget->area.x, wm_widget->area.y );

  if( active_widget!=NULL && active_widget->act_srf!=NULL ) {
  if( active_widget->top==wm_widget->widget ) {
    int act_flag=0;
/*     if( ! active_2D ) { active_2D = MySDL_surface2D_new(); }; */
    /* Call update function here */
    act_flag = (int) (*active_widget->UpdateActive) ( active_widget );
    if ( act_flag!=0 ) { /*  */
      MySDL_surface2D_update( active_2D, \
			      active_widget->act_srf );
/*       active_2D->texture_flag = 0; */
    }
    active_2D->alpha = (GLfloat) active_widget->act_alpha;
    if( active_widget->act_srf==active_2D->srf) {
      MySDL_surface2D_blitsurface( active_2D,		    \
				   active_widget->act_area.x,	\
				   active_widget->act_area.y );
    }
    if( active_widget->tooltipsdata!=NULL ) {
      act_flag = PROT__TooltipsData_update( active_widget->tooltipsdata );
      if ( act_flag==1 ) { /*  */
	MySDL_surface2D_update( active_widget->tooltipsdata->tooltips->surface2D,		\
				active_widget->tooltipsdata->tooltips->srf );
/*       active_2D->texture_flag = 0; */
      }
      if ( act_flag!=-1 ) { /*  */
	MySDL_surface2D_blitsurface( active_widget->tooltipsdata->tooltips->surface2D, \
				     mouse_x+16,				\
				     mouse_y+16 );
      }
    }
  }
  }

  return (void *) NULL;
}

/* GL blit with active directly blitted on his parent before GL blit */
/* like the *_noGL one (bug in active surface TODO before use it) */
/* static void * MyWM_blitsurface_GL_2( SDLGuiTK_WMWidget * wm_widget ) */
/* { */
/*   int act_flag=0; */
/*   SDLGuiTK_2DWidget * wmwidget_2D=wm_widget->wmwidget_2D; */
  
/*   if ( wmwidget_2D->texture_flag!=0 ) { */

/*     wmwidget_2D->srf = MySDL_CopySurface( wmwidget_2D->srf, wm_widget->srf ); */
/*     wmwidget_2D->w = wm_widget->srf->w; */
/*     wmwidget_2D->h = wm_widget->srf->h; */

/*   } */

/*   if( active_widget!=NULL && active_widget->act_srf!=NULL ) { */
/*   if( active_widget->top==wm_widget->widget ) { */

/*     SDL_Rect act_area; */
/*     /\* Call update function here *\/ */
/*     act_flag = (int) (*active_widget->UpdateActive) ( active_widget ); */

/*     if ( act_flag!=0 ) { */
/*       wmwidget_2D->srf = MySDL_CopySurface( wmwidget_2D->srf, wm_widget->srf ); */
/*   SDL_SetAlpha( wmwidget_2D->srf, SDL_RLEACCEL, 255 ); */
/*   SDL_SetAlpha( active_widget->act_srf, 0, 255 ); */
/*       wmwidget_2D->w = wm_widget->srf->w; */
/*       wmwidget_2D->h = wm_widget->srf->h; */
/*       act_area.x = active_widget->act_area.x - wm_widget->area.x; */
/*       act_area.y = active_widget->act_area.y - wm_widget->area.y; */
/*       act_area.w = active_widget->act_srf->w; */
/*       act_area.h = active_widget->act_srf->h; */
/*       SDL_BlitSurface( active_widget->act_srf, NULL, \ */
/* 		       wmwidget_2D->srf, &act_area); */
/*       SDL_UpdateRects( wmwidget_2D->srf, 1, &act_area); */
/*       wm_widget->active_2D->texture_flag = 0; */
/*       wm_widget->wmwidget_2D->texture_flag = 1; */
/*     } */

/*   } */
/*   } */

/*   if ( wm_widget->wmwidget_2D->texture_flag!=0 ) { */
/*     MyWM_make2Dsurface( wm_widget->wmwidget_2D, wmwidget_2D->srf ); */
/*     wm_widget->wmwidget_2D->texture_flag = 0; */
/*   } */
/*   MyWM_blit2Dsurface( wm_widget->wmwidget_2D, \ */
/* 		      wm_widget->area.x, wm_widget->area.y ); */

/*   return (void *) NULL; */
/* } */


#endif



static void * MyWM_blitsurface_noGL( SDLGuiTK_WMWidget * wm_widget )
{
  int act_flag=0;
  SDLGuiTK_Surface2D * surface2D=wm_widget->surface2D;
  
  if( active_widget!=NULL && active_widget->act_srf!=NULL ) {
  if( active_widget->top==wm_widget->widget ) {
    SDL_Rect act_area;
    /* Call update function here */
    act_flag = (int) (*active_widget->UpdateActive) ( active_widget );

    if ( act_flag!=0 ) {
      surface2D->srf = MySDL_CopySurface( surface2D->srf, wm_widget->srf );
      surface2D->w = wm_widget->srf->w;
      surface2D->h = wm_widget->srf->h;
      wm_widget->surface2D_flag = 0;
      act_area.x = active_widget->act_area.x - wm_widget->area.x;
      act_area.y = active_widget->act_area.y - wm_widget->area.y;
      act_area.w = active_widget->act_srf->w;
      act_area.h = active_widget->act_srf->h;
      //SDL_SetAlpha( active_widget->act_srf, SDL_RLEACCEL, 64 ); //SDL_SRCALPHA|
      SDL_BlitSurface( active_widget->act_srf, NULL, \
		       surface2D->srf, &act_area);
      //2SDL_UpdateRects( surface2D->srf, 1, &act_area);
	  //SDL_UpdateWindowSurface( active_widget->act_srf );
    }
  }
  }

  if ( wm_widget->surface2D_flag!=0 ) {
    surface2D->srf = MySDL_CopySurface( surface2D->srf, wm_widget->srf );
    surface2D->w = wm_widget->srf->w;
    surface2D->h = wm_widget->srf->h;
    wm_widget->surface2D_flag = 0;
  }

/*   SDL_SetAlpha( wmwidget_2D->srf, SDL_SRCALPHA|SDL_RLEACCEL, 0 ); */
  SDL_BlitSurface( surface2D->srf, NULL, \
		   current_context->surface, &wm_widget->area);
  //2SDL_UpdateRects( current_context->surface, 1, &wmwidget_2D->wmwidget->area);
  //SDL_UpdateWindowSurface( surface2D->srf );

  return (void *) NULL;
}


void * (*MyWM_blitsurface)( SDLGuiTK_WMWidget * wm_widget );

void MyWM_blitsurface_setopengl( int flag )
{
  if( flag==0 ) {
    MyWM_blitsurface = MyWM_blitsurface_noGL;
  } else {
#ifdef HAVE_GL_GL_H
    MyWM_blitsurface = MyWM_blitsurface_GL;
#else
    SDLGUITK_ERROR( "blitsurface_setopengl(): GL asked but not dispo!\n" );
#endif
  }
}



void MyWM_Init()
{
/*   current_id_mutex = SDL_CreateMutex(); */
  active_2D = MySDL_surface2D_new();
}

void MyWM_Uninit()
{
  MySDL_surface2D_destroy( active_2D );
/*   SDL_DestroyMutex( current_id_mutex ); */
/*   current_id_mutex = NULL; */
}

