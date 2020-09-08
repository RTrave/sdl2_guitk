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

#include <SDL2/SDL_guitk.h>
#include "../debug.h"
#include "../mysdl.h"
#include "../signal.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../theme_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"

#include "alignment_prot.h"


SDL_bool SDLGuiTK_IS_ALIGNMENT( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->alignment!=NULL ) {
                    return SDL_TRUE;
                }
            }
        }
    }
    return SDL_FALSE;
}

SDLGuiTK_Alignment * SDLGuiTK_ALIGNMENT( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_ALIGNMENT(widget) ) {
        SDLGUITK_ERROR( "SDLGuiTK_IS_ALIGNMENT(): widget is not an alignment\n" );
        return NULL;
    }
    return widget->container->bin->alignment;
}


static int current_id=0;


static SDLGuiTK_Alignment * Alignment_create()
{
    SDLGuiTK_Alignment * new_alignment;

    new_alignment = malloc( sizeof( struct SDLGuiTK_Alignment ) );
    new_alignment->bin = PROT__bin_new();
    new_alignment->object = new_alignment->bin->object;
    new_alignment->bin->alignment = new_alignment;
    sprintf( new_alignment->object->name, "alignment%d", ++current_id );

    new_alignment->xalign = 0.5;
    new_alignment->yalign = 0.5;
    new_alignment->xscale = 1.0;
    new_alignment->yscale = 1.0;
    new_alignment->top_padding = 0;
    new_alignment->bottom_padding = 0;
    new_alignment->left_padding = 0;
    new_alignment->right_padding = 0;

    return new_alignment;
}

static void Alignment_destroy( SDLGuiTK_Alignment * alignment )
{
    PROT__bin_destroy( alignment->bin );
    free( alignment );
}


static void * Alignment_DrawUpdate( SDLGuiTK_Widget * widget )
{
    PROT__widget_reset_req_area( widget );

    PROT__bin_DrawUpdate( widget->container->bin );

    return (void *) NULL;
}

static void * Alignment_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Alignment * alignment=widget->container->bin->alignment;
    int wdiff=0, hdiff=0;

    if( alignment->bin->child!=NULL ) {

        wdiff = \
                widget->abs_area.w - (2*widget->container->border_width) \
                - alignment->bin->child->rel_area.w;
        hdiff = \
                widget->abs_area.h - (2*widget->container->border_width) \
                - alignment->bin->child->rel_area.h;

        if( wdiff>0 ) {
            widget->container->children_area.w = \
                                                 alignment->bin->child->rel_area.w + (int)(wdiff*alignment->xscale);
            widget->container->children_area.x = \
                                                 (int)((widget->abs_area.w-widget->container->children_area.w - 2*widget->container->border_width)*alignment->xalign) + widget->container->border_width;
        }
        if( hdiff>0 ) {
            widget->container->children_area.h = \
                                                 alignment->bin->child->rel_area.h + (int)(hdiff*alignment->yscale);
            widget->container->children_area.y = \
                                                 (int)((widget->abs_area.h-widget->container->children_area.h - 2*widget->container->border_width)*alignment->yalign) + widget->container->border_width;
        }

    }

    PROT__bin_DrawBlit( widget->container->bin );

    if( alignment->bin->child!=NULL ) {
        /*     SDL_mutexP( alignment->bin->child->object->mutex ); */
        if( alignment->bin->child->visible ) {
            MySDL_BlitSurface(    alignment->bin->child->srf, NULL, \
                                  widget->srf, &alignment->bin->child->rel_area );
            //2SDL_UpdateRects( widget->srf, 1, &alignment->bin->child->rel_area );
            //SDL_UpdateWindowSurface( widget->srf );
        }
        /*     SDL_mutexV( alignment->bin->child->object->mutex ); */
    }

    widget->rel_area.w = widget->abs_area.w;
    widget->rel_area.h = widget->abs_area.h;

    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    return (void *) NULL;
}

static void * Alignment_Free( SDLGuiTK_Widget * data )
{
    Alignment_destroy( data->container->bin->alignment );

    return (void *) NULL;
}


static SDLGuiTK_Widget * Alignment_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{
    SDLGuiTK_Widget * child;
    SDLGuiTK_Widget * active;
    child = SDLGuiTK_bin_get_child( widget->container->bin );
    if( child==NULL ) {
        SDLGUITK_ERROR( "Alignment_RecursiveEntering(): child==NULL\n" );
        return NULL;
    }

    active = PROT__widget_is_entering( child, x, y );

    return active;
}

static void * Alignment_RecursiveDestroy( SDLGuiTK_Widget * data )
{
    /*   SDLGuiTK_Widget * widget; */

    /*   widget = SDLGuiTK_bin_get_child( data->bin ); */
    /*   PROT__bin_remove( data->bin, widget ); */

    /*   SDLGuiTK_widget_destroy( widget ); */

    return (void *) NULL;
}

static void Alignment_Init_functions( SDLGuiTK_Alignment * alignment )
{
    alignment->object->widget->RecursiveEntering = Alignment_RecursiveEntering;
    alignment->object->widget->RecursiveDestroy = Alignment_RecursiveDestroy;
    alignment->object->widget->Free = Alignment_Free;

    alignment->object->widget->DrawUpdate = Alignment_DrawUpdate;
    alignment->object->widget->DrawBlit = Alignment_DrawBlit;

}


SDLGuiTK_Widget * SDLGuiTK_alignment_new( float xalign, float yalign, \
        float xscale, float yscale )
{
    SDLGuiTK_Alignment * alignment;

    alignment = Alignment_create();
    Alignment_Init_functions( alignment );
    alignment->xalign = xalign;
    alignment->yalign = yalign;
    alignment->xscale = xscale;
    alignment->yscale = yscale;
    PROT__signal_push( alignment->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return alignment->object->widget;
}

void SDLGuiTK_alignment_set( SDLGuiTK_Alignment *alignment, \
                             float xalign, float yalign, \
                             float xscale, float yscale )
{
    alignment->xalign = xalign;
    alignment->yalign = yalign;
    alignment->xscale = xscale;
    alignment->yscale = yscale;
    if( alignment->object->widget->parent ) {
        PROT__signal_push( alignment->object->widget->parent->object,
                           SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    }
}


void PROT__alignment_destroylight( SDLGuiTK_Widget * alignment )
{
    /*   Alignment_destroy( alignment->alignment ); */
}


