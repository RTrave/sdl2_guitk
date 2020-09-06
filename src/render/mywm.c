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


#include "../debug.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../wmwidget.h"
#include "../signal.h"
#include "../list.h"
#include "../context_prot.h"
#include "mywm.h"


static SDLGuiTK_Widget    * focused_widget=NULL;
static SDLGuiTK_Widget    * keyboard_focus=NULL;
static SDLGuiTK_Widget    * mouse_focus=NULL;
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

void MyWM_set_mouse_focus(SDLGuiTK_Widget * widget)
{
    if(mouse_focus==widget) return;
    if(mouse_focus) {
        MyWM_unset_mouse_focus(mouse_focus);
    }
    mouse_focus = widget;
}

void MyWM_unset_mouse_focus(SDLGuiTK_Widget * widget)
{
    if(mouse_focus!=widget) return;
    mouse_focus = NULL;
}


void MyWM_start_textinput()
{
    if(!is_textinput)
        SDL_StartTextInput();
}

void MyWM_stop_textinput()
{
    if(is_textinput && !keyboard_focus)
        SDL_StopTextInput();
}

static void WidgetFocus( int x, int y )
{
    SDLGuiTK_Widget * current, * parent;

    if(current_context->focused &&
       (x==0 || y==0 ||
        x==current_context->focused->srf->srf->w ||
        y==current_context->focused->srf->srf->h))
        return; // inhibit first pixel borders coords
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

void MyWM_WidgetFocus()
{
    int x, y;
    SDL_GetMouseState( &x, &y );
    WidgetFocus( x, y );
}

void MyWM_WidgetUnFocus()
{
    SDLGuiTK_Widget * current, * parent=NULL;

    if(current_context->focused) {
        current_context->focused->enter = SDL_FALSE;
        current_context->focused = NULL;
    }
    mouse_focus = NULL;
    if(keyboard_focus) {
        MyWM_unset_keyboard_focus (keyboard_focus);
        MyWM_stop_textinput ();
    }

    current = focused_widget;
    while( current!=NULL ) {
        parent = current->parent;
        current->has_focus = SDL_FALSE;
        PROT__signal_push( current->object, \
                           SDLGUITK_SIGNAL_TYPE_LEAVE );
        if( current==parent ) {
            focused_widget = NULL;
            return;
        }
        current = parent;
    }
}


static void Focused_IsMoving( int xrel, int yrel )
{
    SDLGuiTK_WMWidget * focused=current_context->focused;
    focused->area.x += xrel;
    focused->area.y += yrel;
    focused->widget->abs_area.x += xrel;
    focused->widget->abs_area.y += yrel;
}


// Inhibition of mouse motion in MODE_MULTIPLES if mouse is not on focused
static SDL_bool inhibit_mousemotion=SDL_FALSE;
void MyWM_inhibit_mousemotion(SDL_bool is_inhibit)
{
    inhibit_mousemotion = is_inhibit;
}

static int UpdateFocused(int x, int y)
{
    SDLGuiTK_WMWidget *focused;
    if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE) {
        if(inhibit_mousemotion)
            return 0;
        if(current_context->active_wmwidget) {
            current_context->focused =
                WMWidget_is_entered (current_context->active_wmwidget, x, y);
            if(current_context->focused)
                WidgetFocus (x, y);
        }
        return 1;
    }
    SDLGuiTK_WMWidget * current;
    current = (SDLGuiTK_WMWidget *)
              SDLGuiTK_list_refrv_init( current_context->activables );
    while( current ) {
        focused = WMWidget_is_entered (current, x, y);
        if(focused && focused==current_context->focused) {
            SDLGuiTK_list_refrv_reset( current_context->activables );
            WidgetFocus (x, y);
            return 1;
        }
        if(focused && focused!=current_context->focused) {
            if(current_context->focused) {
#if DEBUG_LEVEL >= 2
                printf("Leaving WMWidget %s\n", current_context->focused->object->name);
#endif
                MyWM_WidgetUnFocus();
            }
            current_context->focused = focused;
            current_context->focused->enter = SDL_TRUE;
#if DEBUG_LEVEL >= 2
            printf("Entering WMWidget %s\n", current_context->focused->object->name);
#endif
            SDLGuiTK_list_refrv_reset( current_context->activables );
            WidgetFocus (x, y);
            return 1;
        }
        current = (SDLGuiTK_WMWidget *)
                  SDLGuiTK_list_refrv_next( current_context->activables );
    }
    if(current_context->focused) {
#if DEBUG_LEVEL >= 2
        printf("Leaving WMWidget %s\n", current_context->focused->object->name);
#endif
        MyWM_WidgetUnFocus();
        return 0;
    }
    return 0;
}

void MyWM_UpdateFocused()
{
    int x, y;
    SDL_GetMouseState( &x, &y );
    UpdateFocused( x, y );
}

int MyWM_MOUSEMOTION( SDL_Event *event )
{
    // A widget have grab mouse focus: interact with it
    if(mouse_focus) {
        (*mouse_focus->RecursiveEntering) (mouse_focus,
                                           event->motion.x, event->motion.y );
        return 1;
    }
    // A WMwidget have focus, did it interact (move, resize, ..) ?
    if(current_context->type!=SDLGUITK_CONTEXT_MODE_MULTIPLE) {
        if(current_context->focused) {
            if(current_context->focused->moving) {
                Focused_IsMoving (event->motion.xrel, event->motion.yrel);
                return 1;
            }
        }
    }
    return UpdateFocused(event->motion.x, event->motion.y);
}

int MyWM_MOUSEBUTTONDOWN( SDL_Event *event )
{
    if(focused_widget) {
        PROT__signal_push(focused_widget->object,
                          SDLGUITK_SIGNAL_TYPE_PRESSED);
        return 1;
    }

    SDLGuiTK_list_remove(current_context->activables,
                         (SDLGuiTK_Object *) current_context->focused);
    SDLGuiTK_list_append(current_context->activables,
                         (SDLGuiTK_Object *) current_context->focused);

    if(current_context->type!=SDLGUITK_CONTEXT_MODE_MULTIPLE &&
       current_context->focused) {
        PROT__signal_push(current_context->focused->object,
                          SDLGUITK_SIGNAL_TYPE_PRESSED);
        SDLGUITK_LOG("Start moving WMWidget\n");
        current_context->focused->moving = SDL_TRUE;
        return 1;
    }
    return 0;
}

int MyWM_MOUSEBUTTONUP( SDL_Event *event )
{
    if(focused_widget) {
        PROT__signal_push(focused_widget->object,
                          SDLGUITK_SIGNAL_TYPE_RELEASED);
        return 1;
    }
    if(current_context->type!=SDLGUITK_CONTEXT_MODE_MULTIPLE &&
       current_context->focused &&  current_context->focused->moving) {
        PROT__signal_push(current_context->focused->widget->object,
                          SDLGUITK_SIGNAL_TYPE_RELEASED);
        SDLGUITK_LOG("Stop moving WMWidget\n");
        current_context->focused->moving = SDL_FALSE;
        return 1;
    }
    return 0;
}

int MyWM_TEXTINPUT( SDL_Event *event )
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

int MyWM_KEYDOWN( SDL_Event *event )
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

