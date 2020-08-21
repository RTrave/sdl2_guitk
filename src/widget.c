/*
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

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
#include "object_prot.h"
#include "widget_prot.h"
#include "signal.h"
#include "render/surface2d.h"
#include "wmwidget.h"
#include "context_prot.h"

#include "misc_prot.h"
#include "container_prot.h"

#include "windows/window_prot.h"


static int         current_id=0;

static SDLGuiTK_List * destroy_list=NULL;


/* DEFAULT FUNCTIONS when unset */

static SDLGuiTK_Widget * Unset_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{

#if DEBUG_LEVEL >= 1
    char tmpstr[512];
    sprintf( tmpstr, "*RecursiveEntering: not setted in %s.\n", \
             widget->object->name );
    SDLGUITK_ERROR( tmpstr );
#endif
    return NULL;
}

static void * Unset_RecursiveDestroy(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
    char tmpstr[512];
    sprintf( tmpstr, "*RecursiveDestroy: not setted in %s.\n", \
             widget->object->name );
    SDLGUITK_ERROR( tmpstr );
#endif
    return (void *) NULL;
}

static int Unset_UpdateActive(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
    /* char tmpstr[512]; */
    /* sprintf( tmpstr, "*UpdateActive: not setted in %s.\n", \ */
    /*   widget->object->name ); */
    /* SDLGUITK_ERROR( tmpstr ); */
#endif
    return 0;
}

static void * Unset_Free(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
    char tmpstr[512];
    sprintf( tmpstr, "*Free: fct not setted in %s.\n", \
             widget->object->name );
    SDLGUITK_ERROR( tmpstr );
#endif
    return (void *) NULL;
}

static void * Unset_DrawUpdate(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
    char tmpstr[512];
    sprintf( tmpstr, "*DrawUpdate: fct not setted in %s.\n", \
             widget->object->name );
    SDLGUITK_ERROR( tmpstr );
#endif
    return (void *) NULL;
}

static void * Unset_DrawBlit(SDLGuiTK_Widget * widget)
{
#if DEBUG_LEVEL >= 1
    char tmpstr[512];
    sprintf( tmpstr, "*DrawBlit: fct not setted in %s.\n", \
             widget->object->name );
    SDLGUITK_ERROR( tmpstr );
#endif
    return (void *) NULL;
}


/* WIDGET HANDLING */

static SDLGuiTK_Widget * Widget_create()
{
    SDLGuiTK_Widget * new_widget;

    new_widget = malloc( sizeof( struct SDLGuiTK_Widget ) );
    new_widget->object = PROT__object_new();
    new_widget->object->widget = new_widget;

    new_widget->id = current_id++;

    new_widget->visible = SDL_FALSE;
    new_widget->can_focus = SDL_TRUE;
    new_widget->has_focus = SDL_FALSE;
    new_widget->has_default = SDL_FALSE;
    new_widget->has_tooltip = SDL_FALSE;
    new_widget->tooltip_text = NULL;
    new_widget->activable_child = 0;

    new_widget->parent = NULL;
    new_widget->top = NULL;

    new_widget->misc = NULL;
    new_widget->container = NULL;
    new_widget->entry = NULL;
    new_widget->menushell = NULL;
    new_widget->scrollbar = NULL;

    new_widget->width_request = -1;
    new_widget->height_request = -1;

    new_widget->srf = MySDL_Surface_new ("Widget_srf");
    new_widget->act_srf = NULL;
    new_widget->act_alpha = 0.5;
    new_widget->req_area.x = 0;
    new_widget->req_area.y = 0;
    new_widget->req_area.w = 0;
    new_widget->req_area.h = 0;
    new_widget->rel_area.x = 0;
    new_widget->rel_area.y = 0;
    new_widget->rel_area.w = 0;
    new_widget->rel_area.h = 0;
    new_widget->abs_area.x = 0;
    new_widget->abs_area.y = 0;
    new_widget->abs_area.w = 0;
    new_widget->abs_area.h = 0;
    new_widget->act_area.x = 0;
    new_widget->act_area.y = 0;
    new_widget->act_area.w = 0;
    new_widget->act_area.h = 0;

    return new_widget;
}

static void Widget_destroy( SDLGuiTK_Widget * widget )
{
    MySDL_Surface_free( widget->srf );
    PROT__object_destroy( widget->object );
    free( widget );
}

void PROT__widget_init()
{
    current_id = 0;
    destroy_list = SDLGuiTK_list_new();
}

void PROT__widget_uninit()
{
    SDLGuiTK_list_destroy( destroy_list );
    current_id = 0;
}


static void * Widget_MouseEnter( SDLGuiTK_Signal * signal, void * data )
{
    if(signal->object->widget->has_tooltip)
        PROT__context_ref_tooltip (signal->object->widget);
    return (void *) NULL;
}

static void * Widget_MouseLeave( SDLGuiTK_Signal * signal, void * data )
{
    if(signal->object->widget->has_tooltip)
        PROT__context_unref_tooltip ();
    return (void *) NULL;
}

static void * Widget_ChildNotify( SDLGuiTK_Signal * signal, void * data )
{
    SDLGuiTK_Widget * widget=signal->object->widget;
    if(widget->parent && widget->parent!=widget)
        PROT__signal_push( widget->parent->object,
                           SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    return (void *) NULL;
}

static void Widget_set_functions( SDLGuiTK_Widget * widget )
{
    PROT_signal_connect(widget->object, SDLGUITK_SIGNAL_TYPE_ENTER,
                        Widget_MouseEnter, SDLGUITK_SIGNAL_LEVEL1);

    PROT_signal_connect(widget->object, SDLGUITK_SIGNAL_TYPE_LEAVE,
                        Widget_MouseLeave, SDLGUITK_SIGNAL_LEVEL1);

    PROT_signal_connect(widget->object, SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY,
                        Widget_ChildNotify, SDLGUITK_SIGNAL_LEVEL1);
}


SDLGuiTK_Widget * PROT__widget_new()
{
    SDLGuiTK_Widget * widget;

    widget = Widget_create();

    widget->RecursiveEntering = Unset_RecursiveEntering;
    widget->RecursiveDestroy = Unset_RecursiveDestroy;
    widget->UpdateActive = Unset_UpdateActive;
    widget->Free = Unset_Free;
    widget->DrawUpdate = Unset_DrawUpdate;
    widget->DrawBlit = Unset_DrawBlit;

    Widget_set_functions( widget );

    return widget;
}

void PROT__widget_destroy( SDLGuiTK_Widget * widget )
{
    Widget_destroy( widget );
}

void PROT__widget_set_top( SDLGuiTK_Widget *widget, SDLGuiTK_Widget *top)
{
    widget->top = top;
    if(widget->container)
        PROT__container_set_top (widget->container, top);
}



void SDLGuiTK_widget_show( SDLGuiTK_Widget * widget )
{
    if(widget->visible) return;
    widget->visible = SDL_TRUE;
    PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_SHOW );

    if( widget->parent && widget->parent!=widget ) {
        PROT__signal_push( widget->parent->object, SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    }
}

void SDLGuiTK_widget_hide( SDLGuiTK_Widget * widget )
{
    if(!widget->visible) return;
#if DEBUG_LEVEL > 1
    char tmplog[256];
    sprintf( tmplog, "SDLGuiTK_widget_hide():  %s\n", widget->object->name );
    SDLGUITK_LOG( tmplog );
#endif

    widget->visible = SDL_FALSE;
    PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_HIDE );

    if( widget->parent && widget->parent!=widget ) {
        PROT__signal_push( widget->parent->object, SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    }
}

void SDLGuiTK_widget_destroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Container * container;
#if DEBUG_LEVEL > 1
    char tmplog[256];
    sprintf( tmplog, "SDLGuiTK_widget_destroy():  %s\n", widget->object->name );
    SDLGUITK_LOG( tmplog );
#endif

    (*widget->RecursiveDestroy)( widget );
    PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_DESTROY );

    if( widget->parent && widget->parent!=widget ) {
        SDLGUITK_LOG( "SDLGuiTK_widget_destroy(): widget->parent!=NULL\n" );
        container = SDLGuiTK_CONTAINER(widget->parent);
        if( container!=NULL ) {
            SDLGuiTK_container_remove( container, widget );
        } else {
            SDLGUITK_ERROR( "SDLGuiTK_widget_destroy(): widget->parent not known\n" );
        }
        widget->top = NULL;
        widget->parent = NULL;
    }

    SDLGuiTK_list_lock( destroy_list );
    SDLGuiTK_list_append( destroy_list, (SDLGuiTK_Object *) widget );
    SDLGuiTK_list_unlock( destroy_list );
}


void  PROT__widget_DrawUpdate( SDLGuiTK_Widget * widget )
{
    if( widget->req_area.w<widget->width_request )
        widget->req_area.w = widget->width_request;
    if( widget->req_area.h<widget->height_request )
        widget->req_area.h = widget->height_request;
}

void PROT__widget_DrawBlit(   SDLGuiTK_Widget * widget )
{
    widget->abs_area.w = widget->req_area.w;
    widget->abs_area.h = widget->req_area.h;
    widget->rel_area.w = widget->req_area.w;
    widget->rel_area.h = widget->req_area.h;

    MySDL_CreateRGBSurface( widget->srf, \
                            widget->abs_area.w, \
                            widget->abs_area.h );
    if(widget->srf->srf==NULL)
        SDLGUITK_ERROR("No srf created in Widget DrawBlit()\n");
}

void PROT__widget_reset_req_area( SDLGuiTK_Widget *widget )
{
    widget->req_area.w = 0;
    widget->req_area.h = 0;
}

void PROT__widget_set_req_area( SDLGuiTK_Widget *widget, int req_w, int req_h )
{
    if(req_w>widget->req_area.w)
        widget->req_area.w = req_w;
    if(req_h>widget->req_area.h)
        widget->req_area.h = req_h;
}

void PROT__widget_destroypending()
{
    SDLGuiTK_Widget * widget;

    SDLGuiTK_list_lock( destroy_list );
    widget = (SDLGuiTK_Widget *) SDLGuiTK_list_pop_head(destroy_list);
    while( widget!=NULL ) {
        (*widget->Free)( widget );
        widget = (SDLGuiTK_Widget *) SDLGuiTK_list_pop_head(destroy_list);
    }
    SDLGuiTK_list_unlock( destroy_list );

}

void SDLGuiTK_widget_set_size_request( SDLGuiTK_Widget * widget, \
                                       int width, int height )
{
    widget->width_request = width;
    widget->height_request = height;

    if( widget->parent && widget->parent!=widget ) {
        PROT__signal_push( widget->parent->object, SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    }
}


SDLGuiTK_Widget * PROT__widget_is_entering( SDLGuiTK_Widget * widget,
                                            int x, int y )
{
    SDLGuiTK_Widget * current=NULL;

    if(!widget->can_focus) return NULL;
    if( ( x>=widget->act_area.x &&
          x<=(widget->act_area.x + widget->act_area.w) ) &&
        ( y>=widget->act_area.y &&
          y<=(widget->act_area.y + widget->act_area.h) ) )
    {
        if( !widget->has_focus ) {
            widget->has_focus = SDL_TRUE;
            PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_ENTER );
        }
        current = (*widget->RecursiveEntering) (widget, x, y );
        if( current==NULL ) {
            return widget;
        } else {
            return current;
        }
    } else {
        if( widget->has_focus ) {
            widget->has_focus = SDL_FALSE;
            PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_LEAVE );
        }
        return NULL;
    }

    return NULL;
}


void SDLGuiTK_widget_set_has_tooltip( SDLGuiTK_Widget *widget,
                                      SDL_bool has_tooltip )
{
    if(!has_tooltip && widget->has_tooltip) {
        free(widget->tooltip_text);
        widget->tooltip_text = NULL;
    }
    widget->has_tooltip = has_tooltip;
}

SDL_bool  SDLGuiTK_widget_get_has_tooltip( SDLGuiTK_Widget *widget )
{
    return widget->has_tooltip;
}


void SDLGuiTK_widget_set_tooltip_text( SDLGuiTK_Widget *widget,
                                       const char * text )
{
    if(widget->has_tooltip)
        free(widget->tooltip_text);
    widget->tooltip_text = calloc (strlen (text)+1, sizeof (char));
    strcpy(widget->tooltip_text, text);
    widget->has_tooltip = SDL_TRUE;
}

char *  SDLGuiTK_widget_get_tooltip_text( SDLGuiTK_Widget *widget )
{
    if(!widget->has_tooltip) return "";
    char *text = calloc(strlen (widget->tooltip_text), sizeof(char));
    strcpy(text, widget->tooltip_text);
    return text;
}


