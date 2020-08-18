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

#include "../debug.h"
#include "../mysdl.h"
#include "../theme_prot.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"
#include "../signal.h"
#include "../render/surface2d.h"
#include "../wmwidget.h"
#include "../render/mywm.h"

#include "menushell_prot.h"
#include "menu_prot.h"
#include "menuitem_prot.h"


static SDL_bool SDLGuiTK_IS_MENU( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->menu!=NULL ) {
                return SDL_TRUE;
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_MENU(): widget is not a menu\n" );
    return SDL_FALSE;
}

SDLGuiTK_Menu * SDLGuiTK_MENU( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_MENU(widget) ) return NULL;
    return widget->container->menu;
}



static int current_id=0;

static SDLGuiTK_Menu * Menu_create()
{
    SDLGuiTK_Menu * new_menu;

    new_menu = malloc( sizeof( struct SDLGuiTK_Menu ) );
    new_menu->container = PROT__container_new();
    new_menu->object = new_menu->container->widget->object;
    new_menu->container->menu = new_menu;
    sprintf( new_menu->object->name, "menu%d", ++current_id );

    strcpy( new_menu->title, "" );
    new_menu->selected = NULL;

    PROT__menushell_create( new_menu );

    return new_menu;
}

static void Menu_destroy( SDLGuiTK_Menu * menu )
{
    PROT__menushell_destroy( menu );

    PROT__container_destroy( menu->container );
    free( menu );
}


static void * Menu_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Menu * menu=widget->container->menu;
    SDLGuiTK_MenuItem * current_child=menu->selected;
    SDLGuiTK_Widget * current=NULL;

    if( current_child==NULL ) {
        return (void *) NULL;
    }
    current = current_child->child;

    MenuItem_DrawUpdate_Menu( current_child );

    /* Suggested values for container */
    widget->container->children_area.w = current->req_area.w+6; //TODO: why 6?
    widget->container->children_area.h = current->req_area.h+10;

    PROT__container_DrawUpdate( widget->container );

    return (void *) NULL;
}

static void * Menu_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Menu * menu=widget->container->menu;
    SDLGuiTK_MenuItem * current_child=menu->selected;
    //SDLGuiTK_Widget * current=NULL;
    SDL_Rect          tmp_area;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;

    PROT__container_DrawBlit( widget->container );

    if( current_child==NULL ) {
        return (void *) NULL;
    }

    // draw menu background
    theme = PROT__theme_get_and_lock();
    tmp_area.x = 1;
    tmp_area.y = 1;
    tmp_area.w = widget->abs_area.w - 2;
    tmp_area.h = widget->abs_area.h - 2;
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           255 );
    MySDL_FillRect( widget->srf, &tmp_area, bgcolor );
    tmp_area.x = 2;
    tmp_area.y = 2;
    tmp_area.w = widget->abs_area.w - 14;
    tmp_area.h = widget->abs_area.h - 4;
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->bgcolor.r, \
                           theme->bgcolor.g, \
                           theme->bgcolor.b, \
                           255 );
    MySDL_FillRect( widget->srf, &tmp_area, bgcolor );
    PROT__theme_unlock( theme );

    current_child->parent_area.x = widget->container->border_width + 3;
    current_child->parent_area.y = widget->container->border_width + 6;

    MenuItem_DrawBlit_Menu( current_child );

    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    return (void *) NULL;
}


static SDLGuiTK_Widget * Menu_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{
    /*   SDLGuiTK_Menu * menu=widget->container->menu; */
    /*   SDLGuiTK_MenuItem * current_child; */
    /*   SDLGuiTK_Widget * child; */
    /*   SDLGuiTK_Widget * active; */


    return NULL;
}


static void * Menu_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Menu * menu=widget->container->menu;
    SDLGuiTK_MenuItem * child;

    SDLGuiTK_list_lock( menu->menushell->children );

    child = (SDLGuiTK_MenuItem *) SDLGuiTK_list_pop_head( menu->menushell->children );
    while( child ) {
        SDLGuiTK_widget_destroy( child->object->widget );
        child = (SDLGuiTK_MenuItem *) SDLGuiTK_list_pop_head( menu->menushell->children );
    }

    SDLGuiTK_list_unlock( menu->menushell->children );

    return (void *) NULL;
}

static void * Menu_Free( SDLGuiTK_Widget * widget )
{
    Menu_destroy( widget->container->menu );

    return (void *) NULL;
}

static void * Menu_Realize( SDLGuiTK_Widget * widget, \
                            void * data, void * event )
{
    return (void *) NULL;
}

static void * Menu_Show( SDLGuiTK_Widget * widget, \
                         void * data, void * event )
{

    return (void *) NULL;
}

static void * Menu_Hide( SDLGuiTK_Widget * widget, \
                         void * data, void * event )
{

    return (void *) NULL;
}


static void * Menu_MousePressed( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{
    SDLGuiTK_Menu * menu=widget->container->menu;
    PROT__menushell_start( menu );

    return (void *) NULL;
}

static void Menu_Init_functions( SDLGuiTK_Menu * menu )
{
    SDLGuiTK_SignalHandler * handler;

    handler = (SDLGuiTK_SignalHandler *) menu->object->signalhandler;

    menu->object->widget->RecursiveEntering = Menu_RecursiveEntering;
    menu->object->widget->RecursiveDestroy = Menu_RecursiveDestroy;
    menu->object->widget->Free = Menu_Free;

    menu->object->widget->DrawUpdate = Menu_DrawUpdate;
    menu->object->widget->DrawBlit = Menu_DrawBlit;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
            Menu_Realize;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
            Menu_Show;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
            Menu_Hide;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_PRESSED]->function = \
            Menu_MousePressed;
}


SDLGuiTK_Widget * SDLGuiTK_menu_new()
{
    SDLGuiTK_Menu * menu;

    menu = Menu_create();
    Menu_Init_functions( menu );
    PROT__signal_push( menu->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return menu->object->widget;
}


SDLGuiTK_Widget *SDLGuiTK_menu_get_active( SDLGuiTK_Menu *menu ) {
    if( menu->selected->child!=NULL ) {
        return menu->selected->object->widget;
    }
    return NULL;
}

