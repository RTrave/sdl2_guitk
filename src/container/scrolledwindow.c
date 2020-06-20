/*
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

   Copyright (C) 2020 Trave Roman

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
#include "../mywm.h"
#include "../theme_prot.h"
#include "../context_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"
#include "scrolledwindow_prot.h"

static SDL_bool SDLGuiTK_IS_SCROLLEDWINDOW( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->scrolledwindow!=NULL ) {
                    return SDL_TRUE;
                }
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_SCROLLEDWINDOW(): widget is not a scrolledwindow\n" );
    return SDL_FALSE;
}

SDLGuiTK_ScrolledWindow * SDLGuiTK_SCROLLEDWINDOW( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_SCROLLEDWINDOW(widget) ) return NULL;
    return widget->container->bin->scrolledwindow;
}


static int current_id=0;

static SDLGuiTK_ScrolledWindow * ScrolledWindow_create()
{
    SDLGuiTK_ScrolledWindow * new_scrolledwindow;

    new_scrolledwindow = malloc( sizeof( struct SDLGuiTK_ScrolledWindow ) );
    new_scrolledwindow->bin = PROT__bin_new();
    new_scrolledwindow->object = new_scrolledwindow->bin->object;
    new_scrolledwindow->bin->scrolledwindow = new_scrolledwindow;
    sprintf( new_scrolledwindow->object->name, "scrolledwindow%d", ++current_id );
    new_scrolledwindow->object->widget->top = NULL;
    new_scrolledwindow->object->widget->parent = NULL;

    new_scrolledwindow->srf = MySDL_Surface_new("ScrolledWindow_srf");
    new_scrolledwindow->area.x = 0;
    new_scrolledwindow->area.y = 0;
    new_scrolledwindow->area.w = 0;
    new_scrolledwindow->area.h = 0;

    return new_scrolledwindow;
}

static void ScrolledWindow_destroy( SDLGuiTK_ScrolledWindow * scrolledwindow )
{
    MySDL_Surface_free( scrolledwindow->srf );

    PROT__bin_destroy( scrolledwindow->bin );
    //MyWM_WMWidget_Delete( scrolledwindow->wm_widget );
    free( scrolledwindow );
}

static void * ScrolledWindow_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;

    PROT__widget_reset_req_area( widget );

    PROT__bin_DrawUpdate( scrolledwindow->bin );

    return (void *) NULL;
}

static void * ScrolledWindow_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;

    PROT__bin_DrawBlit( scrolledwindow->bin );

    if( scrolledwindow->bin->child!=NULL ) {
        /*     SDL_mutexP( button->bin->child->object->mutex ); */
        if( scrolledwindow->bin->child->shown==1 ) {
            MySDL_BlitSurface(  scrolledwindow->bin->child->srf, NULL, \
                                widget->srf, &scrolledwindow->bin->child->rel_area );
            //2SDL_UpdateRects( widget->srf, 1, &button->bin->child->rel_area );
            //SDL_UpdateWindowSurface( button->bin->child->srf );
            //MySDL_BlitSurface(  srf, NULL, \
            //                    widget->srf, &scrolledwindow->bin->child->rel_area );

        }
        /*     SDL_mutexV( button->bin->child->object->mutex ); */
    }

    return (void *) NULL;
}


static void * ScrolledWindow_Realize( SDLGuiTK_Widget * widget, \
                              void * data, void * event )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;

    /*   widget->changed = 0; */

    return (void *) NULL;
}

static void * ScrolledWindow_Show( SDLGuiTK_Widget * widget, \
                           void * data, void * event )
{
    widget->shown = 1;
    /*   widget->changed = 1; */
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }

    return (void *) NULL;
}

static void * ScrolledWindow_Hide( SDLGuiTK_Widget * widget, \
                           void * data, void * event )
{
    widget->shown = 0;
    /*   widget->changed = 1; */
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }

    return (void *) NULL;
}

static SDLGuiTK_Widget * ScrolledWindow_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{

    return NULL;
}

static void * ScrolledWindow_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * child;

    child = SDLGuiTK_bin_get_child( widget->container->bin );
    PROT__bin_remove( widget->container->bin, child );

    SDLGuiTK_widget_destroy( child );

    return (void *) NULL;
}

static void * ScrolledWindow_Free( SDLGuiTK_Widget * widget )
{
    ScrolledWindow_destroy( widget->container->bin->scrolledwindow );

    return (void *) NULL;
}

static void * ScrolledWindow_MouseEnter( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;

    //if( button->srf!=button->active_srf ) {
        //button->srf = button->active_srf;
        /*     if( widget->top!=NULL ) { */
        /*       PROT__signal_push( widget->top->object,SDLGUITK_SIGNAL_TYPE_FRAMEEVENT ); */
        /*     } */
    //}
    //ButtonCache( widget );
    //widget->act_srf = button->active_srf;
    /*   widget->top->container->bin->window->wm_widget->active_2D->texture_flag = 1; */

    return (void *) NULL;
}

static void * ScrolledWindow_MouseLeave( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;

    /*   if( button->srf!=button->inactive_srf ) { */
    /*     button->srf = button->inactive_srf; */

// CODE TO AVOID NO-GL non refresh trouble.
    /*   if( widget->top!=NULL ) { */
    /*     PROT__signal_push( widget->top->object,SDLGUITK_SIGNAL_TYPE_FRAMEEVENT ); */
    /*   } */
    /*   } */
    //widget->act_srf = NULL;
    //widget->act_alpha = 0.5;
    //button->active_alpha_mod = -0.01;
    //button->pressed_flag = 0;

    return (void *) NULL;
}

static void * ScrolledWindow_MousePressed( SDLGuiTK_Widget * widget, \
                                   void * data, void * event )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;

    /*   SDL_mutexP( button->mutex ); */
    /*   if( button->activate_srf!=button->button->clicked_srf ) { */
    /*     button->activate_srf = button->button->clicked_srf; */
    /*   } */
    /*   SDL_mutexV( button->mutex ); */

    //button->pressed_flag = 1;

    return (void *) NULL;
}

static void * ScrolledWindow_MouseReleased( SDLGuiTK_Widget * widget, \
                                    void * data, void * event )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;
    /*   SDL_mutexP( button->mutex ); */
    /*   if( button->activate_srf==button->button->clicked_srf ) { */
    /*     button->activate_srf = button->button->active_srf; */
    /*   } */
    /*   SDL_mutexV( button->mutex ); */

    /*if( button->pressed_flag==1 ) {
        button->pressed_flag = 0;
        PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_CLICKED );
    }
*/
    return (void *) NULL;
}

static void ScrolledWindow_set_functions( SDLGuiTK_ScrolledWindow * scrolledwindow )
{
    SDLGuiTK_SignalHandler * handler;

    /*   button->widget->Show = Button_Show; */
    /*   button->widget->Hide = Button_Hide; */
    scrolledwindow->object->widget->RecursiveEntering = ScrolledWindow_RecursiveEntering;
    scrolledwindow->object->widget->RecursiveDestroy = ScrolledWindow_RecursiveDestroy;
    //scrolledwindow->object->widget->UpdateActive = ScrolledWindow_UpdateActive;
    scrolledwindow->object->widget->Free = ScrolledWindow_Free;

    scrolledwindow->object->widget->DrawUpdate = ScrolledWindow_DrawUpdate;
    scrolledwindow->object->widget->DrawBlit = ScrolledWindow_DrawBlit;

    handler = (SDLGuiTK_SignalHandler *) scrolledwindow->object->signalhandler;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
            ScrolledWindow_Realize;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
            ScrolledWindow_Show;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
            ScrolledWindow_Hide;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_ENTER]->function = \
            ScrolledWindow_MouseEnter;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_LEAVE]->function = \
            ScrolledWindow_MouseLeave;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_PRESSED]->function = \
            ScrolledWindow_MousePressed;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_RELEASED]->function = \
            ScrolledWindow_MouseReleased;
}

SDLGuiTK_Widget * SDLGuiTK_scrolled_window_new()
{
    SDLGuiTK_ScrolledWindow * scrolledwindow;

    scrolledwindow = ScrolledWindow_create();
    /*   strcpy( button->text, "." ); */
    ScrolledWindow_set_functions( scrolledwindow );
    //Button_make_surface( button );
    /*   button->srf = button->inactive_srf; */
    PROT__signal_push( scrolledwindow->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return scrolledwindow->object->widget;
}


