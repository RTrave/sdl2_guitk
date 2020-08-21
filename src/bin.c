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
#include "container/scrolledwindow_prot.h"
#include "menus/menuitem_prot.h"
#include "bin_prot.h"


static SDL_bool SDLGuiTK_IS_BIN( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                return SDL_TRUE;
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_BIN(): widget is not a bin\n" );
    return SDL_FALSE;
}

SDLGuiTK_Bin * SDLGuiTK_BIN( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_BIN(widget) ) return NULL;
    return widget->container->bin;
}


static SDLGuiTK_Bin * Bin_create()
{
    SDLGuiTK_Bin * new_bin;

    new_bin = malloc( sizeof( struct SDLGuiTK_Bin ) );
    new_bin->container = PROT__container_new();
    new_bin->object = new_bin->container->object;
    new_bin->container->bin = new_bin;

    new_bin->window = NULL;
    new_bin->panel = NULL;
    new_bin->alignment = NULL;
    new_bin->button = NULL;
    new_bin->menuitem = NULL;
    new_bin->scrolledwindow = NULL;
    new_bin->tooltip = NULL;

    new_bin->child = NULL;
    new_bin->margin_left = 0;
    new_bin->margin_right = 0;

    return new_bin;
}

static void Bin_destroy( SDLGuiTK_Bin * bin )
{
    PROT__container_destroy( bin->container );
    free( bin );
}


SDLGuiTK_Bin * PROT__bin_new()
{
    SDLGuiTK_Bin * bin;

    bin = Bin_create();

    return bin;
}

void PROT__bin_destroy( SDLGuiTK_Bin * bin )
{
    Bin_destroy( bin );
}

void PROT__bin_set_top( SDLGuiTK_Bin *bin, SDLGuiTK_Widget *top)
{
    if(bin->child)
        PROT__widget_set_top(bin->child, top);
}


SDLGuiTK_Widget * SDLGuiTK_bin_get_child( SDLGuiTK_Bin * bin )
{
    return bin->child;
}


void PROT__bin_add( SDLGuiTK_Bin * bin, SDLGuiTK_Widget * widget )
{
    if( bin->child )
        SDLGUITK_ERROR( "__bin_add(): yet have a child! LOOSING IT!\n" );

    widget->parent = bin->object->widget;
    PROT__widget_set_top (widget, bin->object->widget->top);
    if( bin->scrolledwindow )
        bin->child = PROT__scrolledwindow_add(bin->scrolledwindow, widget);
    else if( bin->menuitem )
        bin->child = PROT__menuitem_add(bin->menuitem, widget);
    else
        bin->child = widget;

#if DEBUG_LEVEL >= 2
    char tmpstr[512];
    sprintf( tmpstr, "__bin_add(): %s added to %s\n", \
             widget->object->name, widget->parent->object->name );
    SDLGUITK_LOG( tmpstr );
#endif

    if(bin->object->widget->parent)
        PROT__signal_push( bin->object->widget->parent->object,
                           SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
}

void PROT__bin_remove( SDLGuiTK_Bin * bin, SDLGuiTK_Widget * widget )
{
#if DEBUG_LEVEL > 1
    char tmplog[256];
    sprintf( tmplog, "__bin_remove():  %s from %s\n", widget->object->name, bin->object->name );
    SDLGUITK_LOG( tmplog );
#endif

    if( bin->child!=widget ) {
        SDLGUITK_ERROR( "__bin_remove(): Bin child is not the one you think!\n" );
        return;
    }

    if( bin->object->widget->parent ) {
        PROT__signal_push( bin->object->widget->parent->object, \
                           SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    }
    bin->child = NULL;
    widget->parent = NULL;
    PROT__widget_set_top (widget, NULL);

}


void PROT__bin_DrawUpdate( SDLGuiTK_Bin * bin )
{
    SDLGuiTK_Widget *widget=bin->object->widget;

    if(!bin->child || !bin->child->visible) {
        bin->container->children_area.w = 1;
        bin->container->children_area.h = 1;
        PROT__container_DrawUpdate( bin->container );
        return;
    }

    (*bin->child->DrawUpdate)( bin->child );
    bin->container->children_area.w = bin->child->req_area.w;
    bin->container->children_area.h = bin->child->req_area.h;

#if DEBUG_LEVEL >= 3
    /* printf("*** %s PROT__bin_DrawUpdate\n", widget->object->name); */
    /* printf("*** bin children_area1 w:%d h:%d\n", bin->container->children_area.w, bin->container->children_area.h); */
#endif

    PROT__container_DrawUpdate( bin->container );

    widget->req_area.w += ( bin->margin_left + bin->margin_right );
}

void PROT__bin_DrawBlit(   SDLGuiTK_Bin * bin )
{
    SDLGuiTK_Widget *widget=bin->object->widget;

    if( !bin->child || !bin->child->visible ) {
        SDLGUITK_LOG( "PROT__bin_DrawBlit(): 'child==NULL'\n" );
        PROT__container_DrawBlit( bin->container );
        return;
    }

    PROT__container_DrawBlit( bin->container );

    /* CHILD POSITION SUGGESTION  */
    bin->child->abs_area.x =
        bin->container->children_area.x + widget->abs_area.x + bin->margin_left;
    bin->child->abs_area.y =
        bin->container->children_area.y + widget->abs_area.y;

    bin->child->rel_area.x =
        bin->container->children_area.x + bin->margin_left;
    bin->child->rel_area.y =
        bin->container->children_area.y;

    PROT__widget_set_req_area(
        bin->child,
        bin->container->children_area.w - ( bin->margin_left + bin->margin_right ),
        bin->container->children_area.h);
#if DEBUG_LEVEL >= 3
    /* printf("*** %s PROT__bin_DrawBlit\n", widget->object->name); */
    /* printf("*** bin req_area w:%d h:%d\n", widget->req_area.w, widget->req_area.h); */
    /* printf("*** bin abs_area w:%d h:%d\n", widget->abs_area.w, widget->abs_area.h); */
#endif
    (*bin->child->DrawBlit)( bin->child );

    // then add margin_right if needed
    bin->container->children_area.w += bin->margin_right;
}


