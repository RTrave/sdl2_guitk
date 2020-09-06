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
#include "../render/surface2d.h"
#include "../render/render.h"
#include "../theme_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"
#include "../wmwidget.h"
#include "../context_prot.h"
#include "../render/mywm.h"
#include "frame_prot.h"

SDL_bool SDLGuiTK_IS_FRAME( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->frame!=NULL ) {
                    return SDL_TRUE;
                }
            }
        }
    }
    return SDL_FALSE;
}

SDLGuiTK_Frame * SDLGuiTK_FRAME( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_FRAME(widget) ) {
        SDLGUITK_ERROR( "SDLGuiTK_FRAME(): widget is not a frame\n" );
        return NULL;
    }
    return widget->container->bin->frame;
}


static int current_id=0;


static SDLGuiTK_Frame * Frame_create()
{
    SDLGuiTK_Frame * new_frame;

    new_frame = malloc( sizeof( struct SDLGuiTK_Frame ) );
    new_frame->bin = PROT__bin_new();
    new_frame->object = new_frame->bin->object;
    new_frame->bin->frame = new_frame;
    sprintf( new_frame->object->name, "frame%d", ++current_id );
    new_frame->object->widget->top = NULL;
    new_frame->object->widget->parent = NULL;

    new_frame->shadow_type = 0;

    new_frame->srf = MySDL_Surface_new("Frame_srf");
    new_frame->area.x = 0;
    new_frame->area.y = 0;
    new_frame->area.w = 0;
    new_frame->area.h = 0;
    new_frame->label_widget = NULL;
    new_frame->label_area.x = 0;
    new_frame->label_area.y = 0;
    new_frame->label_area.w = 0;
    new_frame->label_area.h = 0;

    return new_frame;
}

static void Frame_destroy( SDLGuiTK_Frame * frame )
{
    MySDL_Surface_free( frame->srf );
    PROT__bin_destroy( frame->bin );
    free( frame );
}

static void * Frame_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Frame * frame=widget->container->bin->frame;

    PROT__widget_reset_req_area( widget );

    PROT__bin_DrawUpdate( frame->bin );

    if(frame->label_widget)
        (*frame->label_widget->DrawUpdate)( frame->label_widget );
    widget->req_area.w+=14;
    widget->req_area.h+=28;
    PROT__widget_set_req_area(widget, 14, 30);

    return (void *) NULL;
}

static void * Frame_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Frame * frame=widget->container->bin->frame;
    Uint32 bgcolor, bdcolor;
    SDLGuiTK_Theme * theme;

    widget->container->children_area.x = 14;
    widget->container->children_area.y = 19;
    widget->container->children_area.w -= 28;
    widget->container->children_area.h -= 28;

    PROT__bin_DrawBlit( frame->bin );

    theme = PROT__theme_get_and_lock();
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->wgcolor.r, \
                           theme->wgcolor.g, \
                           theme->wgcolor.b, \
                           255 );
    bdcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           255 );
    PROT__theme_unlock( theme );

    //MySDL_CreateRGBSurface( widget->srf, widget->abs_area.w, widget->abs_area.h );
    //MySDL_FillRect( frame->srf, NULL, bgcolor );
    frame->area.x = 7;
    frame->area.y = 7;
    frame->area.w = widget->abs_area.w - 14;
    frame->area.h = widget->abs_area.h - 14;
    MySDL_FillRect( widget->srf, &frame->area, bdcolor );
    frame->area.x += 1;
    frame->area.y += 1;
    frame->area.w -= 2;
    frame->area.h -= 2;
    MySDL_FillRect( widget->srf, &frame->area, bgcolor );

    if(frame->label_widget) {
        (*frame->label_widget->DrawBlit)( frame->label_widget );
        frame->label_area.x = 10;
        frame->label_area.y = 0;
        frame->label_area.w = frame->label_widget->srf->srf->w;
        frame->label_area.h = frame->label_widget->srf->srf->h;
        MySDL_BlitSurface( frame->label_widget->srf, NULL, \
                           widget->srf, &frame->label_area );
    }

    if( frame->bin->child ) {
        if( frame->bin->child->visible ) {
            MySDL_BlitSurface( frame->bin->child->srf, NULL, \
                               widget->srf, &frame->bin->child->rel_area );
        }
    }

    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    return (void *) NULL;
}


static SDLGuiTK_Widget * Frame_RecursiveEntering( SDLGuiTK_Widget * widget,
                                                  int x, int y )
{
    SDLGuiTK_Widget * child;
    SDLGuiTK_Widget * active;

    child = SDLGuiTK_bin_get_child( widget->container->bin );
    if( !child ) {
        SDLGUITK_ERROR( "Frame_RecursiveEntering(): child==NULL\n" );
        return NULL;
    }
    if( !child->visible ) {
        SDLGUITK_ERROR( "Frame_RecursiveEntering(): child not visible\n" );
        return NULL;
    }
    active = PROT__widget_is_entering( child, x, y );

    return active;
}

static void * Frame_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Frame * frame=widget->container->bin->frame;
    SDLGuiTK_Widget * child;
    if( frame->label_widget )
        SDLGuiTK_widget_destroy( frame->label_widget );
    child = SDLGuiTK_bin_get_child( widget->container->bin );
    if( child ) {
        SDLGuiTK_widget_destroy( child );
    }

    return (void *) NULL;
}

static void * Frame_Free( SDLGuiTK_Widget * widget )
{
    Frame_destroy( widget->container->bin->frame );

    return (void *) NULL;
}

static void * Frame_ChildNotify( SDLGuiTK_Signal * signal, void * data )
{
    Frame_DrawUpdate( signal->object->widget );
    Frame_DrawBlit( signal->object->widget );

    return (void *) NULL;
}

static void Frame_setsignals( SDLGuiTK_Frame * frame )
{
    PROT_signal_connect(frame->object, SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY,
                        Frame_ChildNotify, SDLGUITK_SIGNAL_LEVEL2);

}

SDLGuiTK_Widget * SDLGuiTK_frame_new()
{
    SDLGuiTK_Frame * frame;

    frame = Frame_create();
    SDLGuiTK_Widget * widget=frame->object->widget;

    widget->RecursiveEntering = Frame_RecursiveEntering;
    widget->RecursiveDestroy = Frame_RecursiveDestroy;
    widget->Free = Frame_Free;

    widget->DrawUpdate = Frame_DrawUpdate;
    widget->DrawBlit = Frame_DrawBlit;

    Frame_setsignals( frame );

    PROT__signal_push( frame->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return frame->object->widget;
}

void 	SDLGuiTK_frame_set_label (SDLGuiTK_Frame * frame,
                                  const char * label)
{

}

void 	SDLGuiTK_frame_set_label_widget (SDLGuiTK_Frame * frame,
                                         SDLGuiTK_Widget * label_widget)
{
    frame->label_widget = label_widget;
    label_widget->parent = frame->object->widget;
    label_widget->top = frame->object->widget;
}

const char * 	SDLGuiTK_frame_get_label (SDLGuiTK_Frame * frame )
{
    return "";
}

SDLGuiTK_Widget * SDLGuiTK_frame_get_label_widget (SDLGuiTK_Frame * frame )
{
    return frame->label_widget;
}


