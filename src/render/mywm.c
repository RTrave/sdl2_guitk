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
#include "../signal.h"
#include "../list.h"
#include "../theme_prot.h"
#include "../context_prot.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../wmwidget.h"
#include "../tooltips_prot.h"
#include "surface2d.h"
#include "mywm.h"


static SDLGuiTK_Widget    * active_widget=NULL;
static SDLGuiTK_Surface2D * active_2D=NULL;


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

static void Desactive_WMWidget( SDLGuiTK_WMWidget * wm_widget )
{
    SDLGuiTK_Widget * current, * parent=NULL;

    current = active_widget;
    while( current!=NULL ) {
        parent = current->parent;
        current->enter = 0;
        PROT__signal_push( current->object, \
                           SDLGUITK_SIGNAL_TYPE_LEAVE );
        if( current==parent ) return;
        current = parent;
    }
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
    if( Is_MOUSEMOTION_in( wm_widget, x, y )==1 )
    {
        if( wm_widget->enter==0 ) {
            if(current_context->active_wmwidget!=wm_widget ) {
                if( current_context->active_wmwidget!=NULL ) {
                    Desactive_WMWidget( current_context->active_wmwidget );
                    active_widget = NULL;
                    current_context->active_wmwidget->enter = 0;
                }
            }
            current_context->active_wmwidget = wm_widget;
            wm_widget->enter = 1;
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
        return 1;
    }

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
        if( Is_MOUSEMOTION_enter(x,y,current)==1 ) {
            current_context->active_wmwidget = current;
            Activables_check( x, y );
            PROT_List_refrv_reinit( current_context->activables );
            SDLGuiTK_list_unlock( current_context->activables );
            return 1;
        }
        current = (SDLGuiTK_WMWidget *) \
                  SDLGuiTK_list_refrv_next( current_context->activables );
    }

    SDLGuiTK_list_unlock( current_context->activables );
    return 0;
}

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
        return 1;
    }
    return 0;
}


void * MyWM_blitsurface( SDLGuiTK_WMWidget * wm_widget )
{
    SDLGuiTK_Surface2D * surface2D=wm_widget->surface2D;
    int mouse_x, mouse_y;
    SDL_GetMouseState( &mouse_x, &mouse_y );

    if ( wm_widget->surface2D_flag!=0 ) {
        Surface2D_update( surface2D, wm_widget->srf->srf );
        wm_widget->surface2D_flag = 0;
    }

    Surface2D_blitsurface( surface2D, \
                           wm_widget->area.x, wm_widget->area.y );

    if( active_widget!=NULL && active_widget->act_srf!=NULL ) {
        if( active_widget->top==wm_widget->widget ) {
            int act_flag=0;
            /*     if( ! active_2D ) { active_2D = MySDL_surface2D_new(); }; */
            /* Call update function here */
            act_flag = (int) (*active_widget->UpdateActive) ( active_widget );
            if ( act_flag!=0 ) { /*  */
                Surface2D_update( active_2D, \
                                  active_widget->act_srf->srf );
                /*       active_2D->texture_flag = 0; */
            }
            active_2D->alpha = (GLfloat) active_widget->act_alpha;
            if( active_widget->act_srf->srf==active_2D->srf) {
                Surface2D_blitsurface( active_2D,		    \
                                       active_widget->act_area.x,	\
                                       active_widget->act_area.y );
            }
            if( active_widget->tooltipsdata!=NULL ) {
                act_flag = PROT__TooltipsData_update( active_widget->tooltipsdata );
                if ( act_flag==1 ) { /*  */
                    Surface2D_update( active_widget->tooltipsdata->tooltips->surface2D,		\
                                      active_widget->tooltipsdata->tooltips->srf );
                    /*       active_2D->texture_flag = 0; */
                }
                if ( act_flag!=-1 ) { /*  */
                    Surface2D_blitsurface( active_widget->tooltipsdata->tooltips->surface2D, \
                                           mouse_x+16,				\
                                           mouse_y+16 );
                }
            }
        }
    }

    return (void *) NULL;
}


void MyWM_Init()
{
    active_2D = Surface2D_new();
}

void MyWM_Uninit()
{
    Surface2D_destroy( active_2D );
}

