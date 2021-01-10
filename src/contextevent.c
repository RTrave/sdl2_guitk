/*
   sdl_guitk - GUI toolkit designed for SDL environnements (GTK-style).

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

#include "context_prot.h"
#include "render/surface2d.h"
#include "render/render.h"
#include "render/mywm.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "render/tooltip_prot.h"
#include "signal.h"
#include "debug.h"
#include "list.h"



static SDLGuiTK_WMWidget * Context_getwmwidget_withwindowid(int id)
{
    SDLGuiTK_WMWidget * current=NULL;
    SDLGuiTK_Context * current_context = PROT__context_current ();
    current = (SDLGuiTK_WMWidget *) \
                  SDLGuiTK_list_refrv_init( current_context->activables );
    while(current) {
        if(SDL_GetWindowID(current->render->window)==id) {
            // be sure its the top wmwidget
            if(!current->is_wmchild) {
                SDLGuiTK_list_refrv_reset( current_context->activables );
                return current;
            }
        }
        current = (SDLGuiTK_WMWidget *) \
                  SDLGuiTK_list_refrv_next( current_context->activables );
    }
    SDLGUITK_ERROR ("No WMWidget found corresponding to window\n");
    return NULL;
}

int  SDLGuiTK_pushevent( SDL_Event *event )
{
    SDLGuiTK_Context * current_context = PROT__context_current ();
    int flag=0;

    SDL_mutexP( current_context->mutex );

    switch(event->type) {

    case SDL_MOUSEMOTION:

        flag = MyWM_MOUSEMOTION( event );
        break;

    case SDL_WINDOWEVENT:

        switch(event->window.event) {

        case SDL_WINDOWEVENT_ENTER:
#if DEBUG_LEVEL >= 2
            SDL_Log("Mouse ENTER %d", event->window.windowID);
#endif
            if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE) {
                SDLGuiTK_WMWidget * wmwidget = \
                        Context_getwmwidget_withwindowid(event->window.windowID);
                if(wmwidget==current_context->active_wmwidget) {
                    MyWM_inhibit_mousemotion(SDL_FALSE);
                    MyWM_WidgetFocus ();
                }
                else
                    MyWM_inhibit_mousemotion(SDL_TRUE);
            }
            break;

        case SDL_WINDOWEVENT_FOCUS_GAINED:
#if DEBUG_LEVEL >= 2
            SDL_Log("Mouse FOCUS_GAINED %d", event->window.windowID);
#endif
            if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE) {
                SDLGuiTK_WMWidget * wmwidget = \
                        Context_getwmwidget_withwindowid(event->window.windowID);
                if(wmwidget) {
#if DEBUG_LEVEL >= 2
                    printf("Entering TOP WMWidget %s\n", wmwidget->object->name);
 #endif
                    current_context->active_wmwidget = wmwidget;
                    MyWM_inhibit_mousemotion(SDL_FALSE);
                }
            }
            MyWM_UpdateFocused();
            break;

        case SDL_WINDOWEVENT_LEAVE:
 #if DEBUG_LEVEL >= 2
            SDL_Log("Mouse LEAVE %d", event->window.windowID);
#endif
            MyWM_WidgetUnFocus();
            break;

        case SDL_WINDOWEVENT_FOCUS_LOST:
#if DEBUG_LEVEL >= 2
            SDL_Log("Mouse FOCUS_LOST %d", event->window.windowID);
#endif
            MyWM_WidgetUnFocus();
            if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE) {
#if DEBUG_LEVEL >= 2
                printf("Leaving TOP WMWidget %s\n", current_context->active_wmwidget->object->name);
#endif
                current_context->active_wmwidget = NULL;
            }
            break;

        }
        break;

    }

    if( current_context->focused ) {
        switch( event->type ) {
        case SDL_MOUSEBUTTONDOWN:
            flag = MyWM_MOUSEBUTTONDOWN( event );
            SDL_mutexV( current_context->mutex );
            return flag;
        case SDL_MOUSEBUTTONUP:
            flag = MyWM_MOUSEBUTTONUP( event );
            SDL_mutexV( current_context->mutex );
            return flag;
        case SDL_KEYDOWN:
            flag = MyWM_KEYDOWN( event );
            SDL_mutexV( current_context->mutex );
            return flag;
        case SDL_TEXTINPUT:
            flag = MyWM_TEXTINPUT( event );
            SDL_mutexV( current_context->mutex );
            return flag;
        default:
            SDL_mutexV( current_context->mutex );
            return 1;
        }
    }

    if( event->type==SDL_QUIT ) {
        if( current_context->type==SDLGUITK_CONTEXT_MODE_SELF ) {
            SDLGuiTK_main_quit();
            flag = 1;
        }
        /* if( current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE ) { */
        /*     if(current_context->active_wmwidget) */
        /*     { */
        /*         MyWM_WidgetUnFocus(); */
        /*         SDLGuiTK_widget_destroy (current_context->active_wmwidget->widget); */
        /*         current_context->active_wmwidget = NULL; */
        /*     } */
        /* } */
    }

    SDL_mutexV( current_context->mutex );
    return flag;

}

