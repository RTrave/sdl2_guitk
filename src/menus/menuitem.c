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
#include "../signal.h"
#include "../theme_prot.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"
#include "../render/surface2d.h"
#include "../wmwidget.h"
#include "../render/mywm.h"

#include "menushell_prot.h"
#include "menu_prot.h"
#include "menuitem_prot.h"



static SDL_bool SDLGuiTK_IS_MENUITEM( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->menuitem!=NULL ) {
                    return SDL_TRUE;
                }
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_MENUITEM(): widget is not a menuitem\n" );
    return SDL_FALSE;
}

SDLGuiTK_MenuItem * SDLGuiTK_MENUITEM( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_MENUITEM(widget) ) return NULL;
    return widget->container->bin->menuitem;
}


static int current_id=0;

static SDLGuiTK_MenuItem * MenuItem_create()
{
    SDLGuiTK_MenuItem * new_menuitem;

    new_menuitem = malloc( sizeof( struct SDLGuiTK_MenuItem ) );
    new_menuitem->bin = PROT__bin_new();
    new_menuitem->object = new_menuitem->bin->object;
    new_menuitem->bin->menuitem = new_menuitem;
    sprintf( new_menuitem->object->name, "menuitem%d", ++current_id );

    new_menuitem->child = NULL;
    new_menuitem->active_flag = 0;
    new_menuitem->active_alpha_mod = -0.01;
    new_menuitem->active_srf = MySDL_Surface_new ("Menuitem_active_srf");

    return new_menuitem;
}

static void MenuItem_destroy( SDLGuiTK_MenuItem * menuitem )
{
     MySDL_Surface_free( menuitem->active_srf );
   free( menuitem );
}

SDLGuiTK_Widget * PROT__menuitem_add( SDLGuiTK_MenuItem * menuitem,
                                      SDLGuiTK_Widget * widget)
{
    menuitem->child = widget;
    return widget;
}


static void Make_activesrf( SDLGuiTK_Widget * widget ) {
    SDLGuiTK_MenuItem * menuitem=widget->container->bin->menuitem;
    SDL_Rect          tmp_area;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;

    MySDL_CreateRGBSurface(   menuitem->active_srf,   \
                              widget->act_area.w,		\
                              widget->act_area.h );
    theme = PROT__theme_get_and_lock();
    tmp_area.x = 0;
    tmp_area.y = 0;
    tmp_area.w = widget->act_area.w;
    tmp_area.h = widget->act_area.h;
    bgcolor = SDL_MapRGBA( menuitem->active_srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           255 );
    MySDL_FillRect( menuitem->active_srf, &tmp_area, bgcolor );
    //SDL_UpdateRect( widget->act_srf, 0, 0, 0, 0 );
    //SDL_SetAlpha( widget->act_srf, SDL_SRCALPHA|SDL_RLEACCEL, 255 ); /*  */
    PROT__theme_unlock( theme );
}

static void * MenuItem_DrawUpdate( SDLGuiTK_Widget * widget )
{
    /*   SDLGuiTK_MenuItem * menuitem=widget->container->bin->menuitem; */
    /*   SDLGuiTK_Widget * child=menuitem->child; */

    PROT__bin_DrawUpdate( widget->container->bin );
    widget->container->children_area.x = 0; //TODO: why 0 ? handled by Bin
    widget->container->children_area.y = 0;
    /*   widget->rel_area.w += (2*SDLGUITK_BUTTONBORDER); */
    /*   widget->rel_area.h += (2*SDLGUITK_BUTTONBORDER); */

    return (void *) NULL;
}

static void * MenuItem_DrawBlit( SDLGuiTK_Widget * widget )
{
    //SDLGuiTK_MenuItem * menuitem=widget->container->bin->menuitem;
    SDLGuiTK_Container * container=widget->container;
    SDLGuiTK_Widget * child=widget->container->bin->child;

    if( container->children_area.w<widget->req_area.w)
        container->children_area.w = widget->req_area.w;
    container->children_area.h = widget->req_area.h;

    PROT__bin_DrawBlit( widget->container->bin );

    /*   printf( "ITEM2 ( child->srf.w=%d , child->srf.h=%d )\n", child->srf->w, child->srf->h ); */
    /*   printf( "ITEM2 ( widget->srf.w=%d , widget->srf.h=%d )\n", widget->srf->w, widget->srf->h ); */

    MySDL_BlitSurface(  child->srf, NULL, \
                        widget->srf, &child->rel_area );
    //SDL_UpdateRect( widget->srf, 0, 0, 0, 0 );

    widget->act_area.x = widget->abs_area.x /* + button->bin->area.x */;
    widget->act_area.y = widget->abs_area.y /* + button->bin->area.y */;
    /*   widget->act_area.w = widget->abs_area.w; */
    //widget->act_area.w = menuitem->menu->container->widget->rel_area.w - 6;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    Make_activesrf( widget );

    return (void *) NULL;
}

void MenuItem_DrawUpdate_Menu( SDLGuiTK_MenuItem * menuitem )
{
    SDLGuiTK_Widget * child=menuitem->child;
    /*   SDLGuiTK_Menu * menu=menuitem->child->parent->container->menu; */

    (*child->DrawUpdate)( child );
    menuitem->parent_area.w = child->req_area.w;
    menuitem->parent_area.h = child->req_area.h;
    /*   printf( "ITEM ( parent_area.w=%d , parent_area.h=%d )\n", menuitem->parent_area.w, menuitem->parent_area.h ); */
    return;
}

void MenuItem_DrawBlit_Menu( SDLGuiTK_MenuItem * menuitem )
{
    SDLGuiTK_Widget * child=menuitem->child;
    /*   SDLGuiTK_Menu * menu=menuitem->child->parent->container->menu; */

    /* CHILD SIZE SUGGESTION */
    /*   widget->abs_area.x = menuitem->child->parent->abs_area.x; */
    /*   widget->abs_area.y = menuitem->child->parent->abs_area.y; */
    /*   widget->rel_area.x = 0; */
    /*   widget->rel_area.y = 0; */
    child->req_area.w = menuitem->parent_area.w;
    child->req_area.h = menuitem->parent_area.h;

    (*child->DrawBlit)( child );

    //child->act_area.w = menuitem->menu->container->widget->rel_area.w - 6;
    /*   widget->act_area.h = widget->abs_area.h; */

    /*   printf( "ITEM ( parent_area.x=%d , parent_area.y=%d )\n", menuitem->parent_area.x, menuitem->parent_area.y ); */
    /*   printf( "ITEM ( parent_area.w=%d , parent_area.h=%d )\n", menuitem->parent_area.w, menuitem->parent_area.h ); */
    /*   printf( "ITEM ( widget->srf.w=%d , widget->srf.h=%d )\n", widget->srf->w, widget->srf->h ); */
    /*   printf( "ITEM ( parent->srf.w=%d , parent->srf.h=%d )\n", menuitem->child->parent->srf->w, menuitem->child->parent->srf->h ); */
    MySDL_BlitSurface(  child->srf, NULL, \
                        menuitem->child->parent->srf, &menuitem->parent_area );
    //2SDL_UpdateRect( menuitem->child->parent->srf, 0, 0, 0, 0 );
    //SDL_UpdateWindowSurface( widget->srf );
    /*   SDL_UpdateRects( menuitem->child->parent->srf, 1, &menuitem->parent_area ); */

    return;
}

static void * MenuItem_Free( SDLGuiTK_Widget * widget )
{
    MenuItem_destroy( widget->container->bin->menuitem );
    return (void *) NULL;
}

static SDLGuiTK_Widget * MenuItem_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{

    return NULL;
}


static void * MenuItem_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    return (void *) NULL;
}


static void * MenuItem_Realize( SDLGuiTK_Widget * widget, \
                                void * data, void * event )
{
    return (void *) NULL;
}

static void * MenuItem_Show( SDLGuiTK_Widget * widget, \
                             void * data, void * event )
{

    return (void *) NULL;
}

static void * MenuItem_Hide( SDLGuiTK_Widget * widget, \
                             void * data, void * event )
{

    return (void *) NULL;
}

static void * MenuItem_MouseEnter( SDLGuiTK_Widget * widget, \
                                   void * data, void * event )
{
    SDLGuiTK_MenuItem * menuitem=widget->container->bin->menuitem;

    Make_activesrf( widget );
    widget->act_srf = menuitem->active_srf;
    widget->act_alpha = 0.5;
    menuitem->active_flag = 1;
    return (void *) NULL;
}

static void * MenuItem_MouseLeave( SDLGuiTK_Widget * widget, \
                                   void * data, void * event )
{
    SDLGuiTK_MenuItem * menuitem=widget->container->bin->menuitem;
    MySDL_FreeSurface( widget->act_srf );
    widget->act_srf = NULL;
    widget->act_alpha = 0.5;
    menuitem->active_alpha_mod = -0.01;
    return (void *) NULL;
}

static int MenuItem_UpdateActive( SDLGuiTK_Widget * widget)
{
    SDLGuiTK_MenuItem * menuitem=widget->container->bin->menuitem;
    if( widget->act_alpha>=0.5 ) {
        menuitem->active_alpha_mod = -0.01;
    };
    if( widget->act_alpha<=0.25 ) {
        menuitem->active_alpha_mod = 0.01;
    };
    widget->act_alpha+=menuitem->active_alpha_mod;
    if( menuitem->active_flag==1 ) {
        menuitem->active_flag = 0;
        return 1;
    }
    return 0;
}

static void * MenuItem_MousePressed( SDLGuiTK_Widget * widget, \
                                     void * data, void * event )
{
    SDLGuiTK_MenuItem * menuitem=widget->container->bin->menuitem;
    SDLGuiTK_Menu * menu=menuitem->menu;

    /*   menu->selected = menuitem; */
    menu->selected = menuitem;
    PROT__signal_push( menu->menushell->widget->object, SDLGUITK_SIGNAL_TYPE_SELECTIONDONE );
    /*   PROT__signal_push( menu->menushell->widget->object, SDLGUITK_SIGNAL_TYPE_LEAVE ); */

    return (void *) NULL;
}



static void MenuItem_Init_functions( SDLGuiTK_MenuItem * menuitem )
{
    SDLGuiTK_SignalHandler * handler;

    handler = (SDLGuiTK_SignalHandler *) menuitem->object->signalhandler;

    menuitem->object->widget->RecursiveEntering = MenuItem_RecursiveEntering;
    menuitem->object->widget->RecursiveDestroy = MenuItem_RecursiveDestroy;
    menuitem->object->widget->Free = MenuItem_Free;

    menuitem->object->widget->DrawUpdate = MenuItem_DrawUpdate;
    menuitem->object->widget->DrawBlit = MenuItem_DrawBlit;
    menuitem->object->widget->UpdateActive = MenuItem_UpdateActive;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
            MenuItem_Realize;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
            MenuItem_Show;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
            MenuItem_Hide;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_ENTER]->function = \
            MenuItem_MouseEnter;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_LEAVE]->function = \
            MenuItem_MouseLeave;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_PRESSED]->function = \
            MenuItem_MousePressed;
}

SDLGuiTK_Widget *SDLGuiTK_menuitem_new() {
    SDLGuiTK_MenuItem * menuitem;
    menuitem = MenuItem_create();
    MenuItem_Init_functions( menuitem );
    PROT__signal_push( menuitem->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return menuitem->object->widget;
}

SDLGuiTK_Widget *SDLGuiTK_menuitem_new_with_label( char *label ) {
    SDLGuiTK_MenuItem * menuitem;
    SDLGuiTK_Widget * w_label;

    menuitem = MenuItem_create();
    MenuItem_Init_functions( menuitem );
    w_label = SDLGuiTK_label_new( label );
    menuitem->child = w_label;
    SDLGuiTK_container_add( SDLGuiTK_CONTAINER(menuitem->object->widget), w_label );
    SDLGuiTK_widget_show( w_label );
    PROT__signal_push( menuitem->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return menuitem->object->widget;
}

void SDLGuiTK_menuitem_select( SDLGuiTK_MenuItem *menu_item ) {
    SDLGuiTK_Menu * menu=menu_item->menu;
    menu->selected = menu_item;
    PROT__signal_push( menu->container->widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
}

