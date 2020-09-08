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

#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "mysdl.h"
#include "signal.h"

#include "object_prot.h"
#include "widget_prot.h"
#include "theme_prot.h"

#include "container_prot.h"

#include "bin_prot.h"
#include "container/box_prot.h"
#include "container/grid_prot.h"


SDL_bool SDLGuiTK_IS_CONTAINER( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            return SDL_TRUE;
        }
    }
    return SDL_FALSE;
}

SDLGuiTK_Container * SDLGuiTK_CONTAINER( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_CONTAINER(widget) ) {
        SDLGUITK_ERROR( "SDLGuiTK_IS_CONTAINER(): widget is not a container\n" );
        return NULL;
    }
    return widget->container;
}


static SDLGuiTK_Container * Container_create()
{
    SDLGuiTK_Container * new_container;

    new_container = malloc( sizeof( struct SDLGuiTK_Container ) );
    new_container->widget = PROT__widget_new();
    new_container->object = new_container->widget->object;
    new_container->widget->container = new_container;

    new_container->bin = NULL;
    new_container->box = NULL;
    new_container->grid = NULL;
    new_container->menu = NULL;

    new_container->border_width = 0;

    new_container->children_area.x = 0;
    new_container->children_area.y = 0;
    new_container->children_area.w = 0;
    new_container->children_area.h = 0;

    return new_container;
}

static void Container_destroy( SDLGuiTK_Container * container )
{
    PROT__widget_destroy( container->widget );
    free( container );
}


void SDLGuiTK_container_add( SDLGuiTK_Container * container,\
                             SDLGuiTK_Widget * widget )
{
    if( container->bin ) {
        PROT__bin_add( container->bin, widget );
        return;
    }
    if( container->box ) {
        SDLGUITK_ERROR( "SDLGuiTK_container_add(): use _box_pack_start for boxes!\n" );
        return;
    }
    if( container->grid ) {
        SDLGUITK_ERROR( "SDLGuiTK_container_add(): use _grid_attach for grids!\n" );
        return;
    }
    if( container->menu ) {
        SDLGUITK_ERROR( "SDLGuiTK_container_add(): use _menu_shell_append for menus!\n" );
        return;
    }
}


void SDLGuiTK_container_remove( SDLGuiTK_Container * container, \
                                SDLGuiTK_Widget * widget )
{
    if( container->bin ) {
        PROT__bin_remove( container->bin, widget );
        return;
    }

    if( container->box ) {
        PROT__box_remove( container->box, widget );
        return;
    }

    if( container->grid ) {
        PROT__grid_remove( container->grid, widget );
        return;
    }

    if( container->menu ) {
        //PROT__menu_remove( container->menu, widget );
        //return; //TODO
    }

#if DEBUG_LEVEL >= 1
    char tmpstr[512];
    sprintf( tmpstr, "container_remove(): %s not found in %s\n", \
             widget->object->name, \
             container->object->name );
    SDLGUITK_ERROR( tmpstr );
#endif
}



SDLGuiTK_Container * PROT__container_new()
{
    SDLGuiTK_Container * container;

    container = Container_create();

    return container;
}

void PROT__container_destroy( SDLGuiTK_Container * container )
{
    Container_destroy( container );
}

void PROT__container_set_top( SDLGuiTK_Container *container, SDLGuiTK_Widget *top)
{
    if(container->bin)
        PROT__bin_set_top (container->bin, top);
    else if(container->box)
        PROT__box_set_top (container->box, top);
    else if(container->grid)
        PROT__grid_set_top (container->grid, top);
    // Menu ?
}


SDLGuiTK_List *SDLGuiTK_container_get_children( SDLGuiTK_Container *container )
{
    SDLGuiTK_List * children=NULL;

    if( container->bin ) {
        if( container->bin->child ) {
            children = SDLGuiTK_list_new();
            SDLGuiTK_list_append( children, container->bin->child->object );
        }
    } else if( container->box ) {
        children = SDLGuiTK_list_copy( container->box->children );
    } else if( container->grid ) {
        children = SDLGuiTK_list_copy( container->grid->children );
    }

    return children;
}


void PROT__container_DrawUpdate( SDLGuiTK_Container * container )
{
    container->children_area.x = container->border_width;
    container->children_area.y = container->border_width;

    PROT__widget_set_req_area(container->widget,
                              container->children_area.w + 2*(container->border_width),
                              container->children_area.h + 2*(container->border_width));

    PROT__widget_DrawUpdate( container->widget );


    container->children_area.w =
        container->widget->req_area.w - 2*(container->border_width);
    container->children_area.h =
        container->widget->req_area.h - 2*(container->border_width);
}

static void Container_DrawSurface( SDLGuiTK_Container * container )
{
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;

    container->bg_area.x = container->border_width;
    container->bg_area.y = container->border_width;
    container->bg_area.w = \
                           container->widget->abs_area.w - (2*container->border_width);
    container->bg_area.h = \
                           container->widget->abs_area.h - (2*container->border_width);

    theme = PROT__theme_get_and_lock();
#if DEBUG_LEVEL >= 3
    bgcolor = SDL_MapRGBA( container->widget->srf->srf->format, \
                           0xff, 0xff, 0x00, 128 );
#else
    bgcolor = SDL_MapRGBA( container->widget->srf->srf->format, \
                           theme->wgcolor.r, \
                           theme->wgcolor.g, \
                           theme->wgcolor.b, \
                           255 );
#endif
    PROT__theme_unlock( theme );

    MySDL_FillRect( container->widget->srf, &container->bg_area, bgcolor );
}

void PROT__container_DrawBlit(   SDLGuiTK_Container * container )
{
    if(!SDLGuiTK_IS_FRAME (container->widget)) {
        container->children_area.w =
            container->widget->req_area.w - 2*(container->border_width);
        container->children_area.h =
            container->widget->req_area.h - 2*(container->border_width);
    }

    PROT__widget_DrawBlit( container->widget );

    Container_DrawSurface( container );
}


void SDLGuiTK_container_set_border_width( SDLGuiTK_Container * container,\
        int border_width )
{
    container->border_width = border_width;

    if( container->widget->parent )
        PROT__signal_push( container->widget->parent->object, \
                           SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
}

