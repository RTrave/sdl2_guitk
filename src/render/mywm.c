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


#include "mywm.h"
#include "../widget_prot.h"
#include "../wmwidget.h"
#include "../signal.h"
#include "../list.h"
#include "../context_prot.h"


static SDLGuiTK_Widget    * focused_widget=NULL;
static SDLGuiTK_Widget    * keyboard_focus=NULL;
static SDL_bool             is_textinput=SDL_FALSE;
static SDLGuiTK_Surface2D * animate_2D=NULL;


void MyWM_set_keyboard_focus(SDLGuiTK_Widget * widget)
{
    if(keyboard_focus==widget) return;
    if(keyboard_focus) {
        MyWM_unset_keyboard_focus(keyboard_focus);
    }
    keyboard_focus = widget;
    keyboard_focus->has_default = SDL_TRUE;
}

void MyWM_unset_keyboard_focus(SDLGuiTK_Widget * widget)
{
    if(keyboard_focus!=widget) return;
    keyboard_focus->has_default = SDL_FALSE;
    keyboard_focus = NULL;
}

void MyWM_start_textinput()
{
    if(!is_textinput)
        SDL_StartTextInput();
}

void MyWM_stop_textinput()
{
    if(is_textinput && !keyboard_focus)
        SDL_StartTextInput();
}

static void CheckFocus( int x, int y )
{
    SDLGuiTK_Widget * current, * parent;

    if( focused_widget ) {
        current=PROT__widget_is_entering(focused_widget,x,y);
        if( !current ) {
            while( focused_widget->parent!=focused_widget && current==NULL ) {
                parent = focused_widget->parent;
                focused_widget = parent;
                current=PROT__widget_is_entering(focused_widget,x,y);
            }
            focused_widget = current;
            return;
        } else {
            focused_widget = current;
            return;
        }
    }

    if( current_context->focused ) {
        current =
            PROT__widget_is_entering(current_context->focused->widget, x, y);
        if( current ) {
            focused_widget = current;
        }
    }

}

static void UnFocus( SDLGuiTK_WMWidget * wm_widget )
{
    SDLGuiTK_Widget * current, * parent=NULL;

    current = focused_widget;
    while( current!=NULL ) {
        parent = current->parent;
        current->has_focus = SDL_FALSE;
        PROT__signal_push( current->object, \
                           SDLGUITK_SIGNAL_TYPE_LEAVE );
        if( current==parent ) return;
        current = parent;
    }
}

static int Is_MOUSEMOTION_in( SDLGuiTK_WMWidget * wm_widget, int x, int y ) {
    if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE)
        if (wm_widget->render!=current_context->active_render &&
            !(wm_widget->is_wmchild &&
             wm_widget->wmparent->render==current_context->active_render))
            return 0;
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
    if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE)
        if (wm_widget->render!=current_context->active_render &&
            !(wm_widget->is_wmchild &&
             wm_widget->wmparent->render==current_context->active_render))
            return 0;
    if( Is_MOUSEMOTION_in( wm_widget, x, y )==1 )
    {
        if( wm_widget->enter==0 ) {
            if(current_context->focused!=wm_widget ) {
                if( current_context->focused ) {
                    UnFocus( current_context->focused );
                    focused_widget = NULL;
                    current_context->focused->enter = 0;
                }
            }
            current_context->focused = wm_widget;
            wm_widget->enter = 1;
            CheckFocus( x, y );
        } else {
            CheckFocus( x, y );
        }
        return 1;
    } else {
        wm_widget->enter = 0;
        CheckFocus( x, y );
    }
    return 0;
}

static int Is_MOUSEMOTION_moving( SDL_Event * event )
{
    SDLGuiTK_WMWidget * active=current_context->focused;

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

    if( focused_widget ) {
        focused_widget->has_focus = SDL_FALSE;
        PROT__signal_push( focused_widget->object,
                           SDLGUITK_SIGNAL_TYPE_LEAVE );
        while(focused_widget->parent!=focused_widget  ) {
            focused_widget = focused_widget->parent;
            focused_widget->has_focus = SDL_FALSE;
            PROT__signal_push( focused_widget->object,
                               SDLGUITK_SIGNAL_TYPE_LEAVE );
        }
        focused_widget = NULL;
    }
    if( current_context->focused ) {
        current_context->focused->enter = 0;
        current_context->focused = NULL;
    }
}

int MyWM_push_MOUSEMOTION_in( int x, int y )
{
    SDLGuiTK_WMWidget * current=NULL;

    SDLGuiTK_list_lock( current_context->activables );

    if( current_context->focused ) {
        current = (SDLGuiTK_WMWidget *)
                  SDLGuiTK_list_refrv_init( current_context->activables );
        while( current!=NULL && current!=current_context->focused ) {
            if(current->render==current_context->active_render ||
               (current->is_wmchild &&
                current->wmparent->render==current_context->active_render))
            {
                if( Is_MOUSEMOTION_enter(x,y,current)==1 ) {
                    current_context->focused = current;
                    CheckFocus (x, y);
                    PROT_List_refrv_reinit( current_context->activables );
                    SDLGuiTK_list_unlock( current_context->activables );
                    return 1;
                }
            }
            current = (SDLGuiTK_WMWidget *) \
                      SDLGuiTK_list_refrv_next( current_context->activables );
        }

        if( Is_MOUSEMOTION_enter(x,y,current_context->focused)==0 ) {
            current_context->focused->enter = 0;
            current_context->focused = NULL;
            CheckFocus( x, y );
        } else {
        }
    }

    PROT_List_refrv_reinit( current_context->activables );

    current = (SDLGuiTK_WMWidget *) \
              SDLGuiTK_list_refrv_init( current_context->activables );

    while( current!=NULL ) {
        if( Is_MOUSEMOTION_enter(x,y,current)==1 ) {
            current_context->focused = current;
            CheckFocus( x, y );
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
    if( current_context->focused ) {

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
    if( focused_widget ) {
        PROT__signal_push( focused_widget->object,
                           SDLGUITK_SIGNAL_TYPE_PRESSED );
        return 1;
    }

    SDLGuiTK_list_lock( current_context->activables );
    SDLGuiTK_list_remove( current_context->activables, \
                          (SDLGuiTK_Object *) current_context->focused );
    SDLGuiTK_list_append( current_context->activables, \
                          (SDLGuiTK_Object *) current_context->focused );
    SDLGuiTK_list_unlock( current_context->activables );


    if( Is_MOUSEBUTTONDOWN_move_start(event,current_context->focused)==1 )
    {
        PROT__signal_push( current_context->focused->object,
                           SDLGUITK_SIGNAL_TYPE_PRESSED );
        return 1;
    }

    return 0;
}

int MyWM_push_MOUSEBUTTONUP( SDL_Event *event )
{
    if( focused_widget ) {
        PROT__signal_push( focused_widget->object, \
                           SDLGUITK_SIGNAL_TYPE_RELEASED );
        return 1;
    }
    if( current_context->focused ) {
        PROT__signal_push( current_context->focused->widget->object, \
                           SDLGUITK_SIGNAL_TYPE_RELEASED );
        return Is_MOUSEBUTTONDOWN_move_stop( current_context->focused );
    }
    return 0;
}

int MyWM_push_TEXTINPUT( SDL_Event *event )
{
    if( keyboard_focus ) {
        PROT__signal_push_event( keyboard_focus->object, \
                                 SDLGUITK_SIGNAL_TYPE_TEXTINPUT, \
                                 event );
        return 1;
    }
    else if( focused_widget ) {
        PROT__signal_push_event( focused_widget->object, \
                                 SDLGUITK_SIGNAL_TYPE_TEXTINPUT, \
                                 event );
        return 1;
    }
    return 0;
}

int MyWM_push_KEYDOWN( SDL_Event *event )
{
    if( keyboard_focus ) {
        PROT__signal_push_event( keyboard_focus->object, \
                                 SDLGUITK_SIGNAL_TYPE_KEYBOARD, \
                                 event );
        return 1;
    }
    else if( focused_widget ) {
        PROT__signal_push_event( focused_widget->object, \
                                 SDLGUITK_SIGNAL_TYPE_KEYBOARD, \
                                 event );
        return 1;
    }
    return 0;
}

int MyWM_push_WINDOWEVENT( SDL_Event *event )
{
    switch (event->window.event) {
    case SDL_WINDOWEVENT_SHOWN:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d shown", event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_HIDDEN:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d hidden", event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_EXPOSED:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d exposed", event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_MOVED:
        //SDL_Log("Window %d moved to %d,%d",
        //        event->window.windowID, event->window.data1,
        //        event->window.data2);
        break;
    case SDL_WINDOWEVENT_RESIZED:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d resized to %dx%d",
                event->window.windowID, event->window.data1,
                event->window.data2);
#endif
        break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d size changed to %dx%d",
                event->window.windowID, event->window.data1,
                event->window.data2);
#endif
        break;
    case SDL_WINDOWEVENT_MINIMIZED:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d minimized", event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_MAXIMIZED:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d maximized", event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_RESTORED:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d restored", event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_ENTER:
#if DEBUG_LEVEL >= 3
        SDL_Log("Mouse entered window %d",
                event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_LEAVE:
#if DEBUG_LEVEL >= 3
        SDL_Log("Mouse left window %d", event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d gained keyboard focus",
                event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d lost keyboard focus",
                event->window.windowID);
#endif
        break;
    case SDL_WINDOWEVENT_CLOSE:
#if DEBUG_LEVEL >= 3
       SDL_Log("Window %d closed", event->window.windowID);
#endif
        break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
    case SDL_WINDOWEVENT_TAKE_FOCUS:
#if DEBUG_LEVEL >= 3
        SDL_Log("Window %d is offered a focus (%d activables)",
                event->window.windowID,
                SDLGuiTK_list_length (current_context->activables));
#endif
        break;
    case SDL_WINDOWEVENT_HIT_TEST:
 #if DEBUG_LEVEL >= 3
       SDL_Log("Window %d has a special hit test", event->window.windowID);
#endif
       break;
#endif
    }
    /* if( active_widget!=NULL ) { */
    /*     PROT__signal_pushkey( active_widget->object, \ */
    /*                           SDLGUITK_SIGNAL_TYPE_KEYBOARD, \ */
    /*                           &event->key ); */
    /*     return 1; */
    /* } */
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

    if( focused_widget && focused_widget->active_srf ) {
        if( focused_widget->top==wm_widget->widget ) {
            int act_flag=0;
            /*     if( ! active_2D ) { active_2D = MySDL_surface2D_new(); }; */
            /* Call update function here */
            act_flag = (int) (*focused_widget->UpdateActive) ( focused_widget );
            if ( act_flag!=0 ) { /*  */
                Surface2D_update( animate_2D,
                                  focused_widget->active_srf->srf );
                /*       active_2D->texture_flag = 0; */
            }
            animate_2D->alpha = (GLfloat) focused_widget->active_alpha;
            //if( focused_widget->act_srf->srf==animate_2D->srf) {
                Surface2D_blitsurface( animate_2D,		    \
                                       focused_widget->active_area.x,	\
                                       focused_widget->active_area.y );
            //}
        }
    }

    return (void *) NULL;
}


void MyWM_Init()
{
    animate_2D = Surface2D_new();
}

void MyWM_Uninit()
{
    Surface2D_destroy( animate_2D );
}

