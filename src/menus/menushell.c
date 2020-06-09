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
#include "../list.h"
#include "../context_prot.h"
#include "../theme_prot.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"
#include "../signal.h"
#include "../mywm.h"
#include "../mycursor.h"

#include "menushell_prot.h"
#include "menu_prot.h"
#include "menuitem_prot.h"


static int current_id=0;


static struct SDLGuiTK_MenuShell * MenuShell_Create() {
    struct SDLGuiTK_MenuShell * new_menushell;

    new_menushell = malloc( sizeof( struct SDLGuiTK_MenuShell ) );
    new_menushell->widget = PROT__widget_new();
    new_menushell->widget->menushell = new_menushell;
    new_menushell->object = new_menushell->widget->object;
    sprintf( new_menushell->object->name, "menushell%d", ++current_id );

    new_menushell->wm_widget = NULL;
    new_menushell->children = SDLGuiTK_list_new();

    return new_menushell;
}

static void MenuShell_Destroy( struct SDLGuiTK_MenuShell * menushell ) {
    SDLGuiTK_list_destroy( menushell->children );
    PROT__widget_destroy( menushell->widget );
    free( menushell );
}

static void MenuShell_UpdatePosition( SDLGuiTK_MenuShell * menushell ) {
    SDLGuiTK_Widget * widget=menushell->object->widget;
    widget->abs_area.x = menushell->menu->object->widget->abs_area.x+3;
    widget->abs_area.y = menushell->menu->object->widget->abs_area.y+3;

}


static void * MenuShell_DrawUpdate( SDLGuiTK_Widget * widget )
{
    struct SDLGuiTK_MenuShell * menushell=widget->menushell;
    SDLGuiTK_Widget   * current;
    //SDLGuiTK_MenuItem * current_item;

    /* UPDATE ASCENDENTS */
    widget->parent = widget;
    widget->top = widget;
    /* First placement for widget */
    widget->req_area.w = 0;
    widget->req_area.h = 0;

    SDLGuiTK_list_lock( menushell->children );
    current = (SDLGuiTK_Widget *) SDLGuiTK_list_ref_init( menushell->children );
    while( current!=NULL ) {
        //current_item = SDLGuiTK_MENUITEM( current );
        current->top = widget->top;
        current->parent = widget;
        (*current->DrawUpdate)( current );
        if( widget->req_area.w<current->req_area.w )
            widget->req_area.w = current->req_area.w;
        widget->req_area.h += current->req_area.h+2;
        current = (SDLGuiTK_Widget *) SDLGuiTK_list_ref_next( menushell->children );
    }

    //TODO: why reset with Menu w -6 ???
    widget->req_area.w = menushell->menu->container->widget->req_area.w - 6;

    PROT__widget_DrawUpdate( widget );

    /* Prepare WM sizes */
    menushell->wm_widget->child_area.w = widget->req_area.w; //TODO: abs_area ???
    menushell->wm_widget->child_area.h = widget->req_area.h;
    menushell->wm_widget->surface2D_flag = 1;
    MyWM_WMWidget_DrawUpdate( menushell->wm_widget );

    return (void *) NULL;
}

static void * MenuShell_DrawBlit( SDLGuiTK_Widget * widget )
{
    struct SDLGuiTK_MenuShell * menushell=widget->menushell;
    SDLGuiTK_Widget   * current;
    //SDLGuiTK_MenuItem * current_item;
    int current_x=1, current_y=1;
    SDL_Rect          tmp_area;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;

    PROT__widget_DrawBlit( menushell->widget );

    theme = PROT__theme_get_and_lock();
    tmp_area.x = 1;
    tmp_area.y = 1;
    tmp_area.w = widget->abs_area.w - 2;
    tmp_area.h = widget->abs_area.h - 2;
    bgcolor = SDL_MapRGBA( widget->srf->format, \
                           theme->bgcolor.r, \
                           theme->bgcolor.g, \
                           theme->bgcolor.b, \
                           255 );
    SDL_FillRect( widget->srf, &tmp_area, bgcolor );
    //SDL_UpdateRect( widget->srf, 0, 0, 0, 0 );
    PROT__theme_unlock( theme );

    current = (SDLGuiTK_Widget *) SDLGuiTK_list_ref_init( menushell->children );
    while( current!=NULL ) {
        //current_item = SDLGuiTK_MENUITEM( current );
        /* CHILD SIZE SUGGESTION */
        current->abs_area.x = widget->abs_area.x + current_x;
        current->abs_area.y = widget->abs_area.y + current_y;
        current->rel_area.x = current_x;
        current->rel_area.y = current_y;
        //current->abs_area.w = current->rel_area.w;
        //current->abs_area.h = current->rel_area.h;
        (*current->DrawBlit)( current );
        current_y += current->rel_area.h;
        // Workaround bad active values
        /* current->act_area.x = current->abs_area.x; */
        /* current->act_area.y = current->abs_area.y; */
        current->act_area.w = widget->abs_area.w;
        /* current->act_area.h = current->abs_area.h; */
        SDL_BlitSurface( current->srf, NULL,			\
                         widget->srf, &current->rel_area );
        //2SDL_UpdateRect( widget->srf, 0, 0, 0, 0 );
        //SDL_UpdateWindowSurface( current->srf );
        /*     SDL_UpdateRects( widget->srf, 1, &widget->rel_area ); */
        current_y+=2;
        current = (SDLGuiTK_Widget *) SDLGuiTK_list_ref_next( menushell->children );
    }

    //widget->rel_area.w = widget->abs_area.w;
    //widget->rel_area.h = widget->abs_area.h;

    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    SDLGuiTK_list_unlock( menushell->children );

    MyWM_WMWidget_DrawBlit( menushell->wm_widget );

    SDL_BlitSurface( widget->srf, NULL, \
                     menushell->wm_widget->srf, &menushell->wm_widget->child_area );
    //SDL_UpdateRect( menushell->wm_widget->srf, 0, 0, 0, 0 );

    return (void *) NULL;
}

static SDLGuiTK_Widget * MenuShell_RecursiveEntering( SDLGuiTK_Widget *widget, \
        int x, int y )
{
    struct SDLGuiTK_MenuShell * menushell=widget->menushell;
    SDLGuiTK_Widget   * current, *active;
    //SDLGuiTK_MenuItem * current_item;
    /*   int current_x=1, current_y=1; */

    SDLGuiTK_list_lock( menushell->children );

    current = (SDLGuiTK_Widget *) SDLGuiTK_list_ref_init( menushell->children );
    while( current!=NULL ) {
        //current_item = SDLGuiTK_MENUITEM( current );
        /*     current->act_area.x = widget->abs_area.x + current_x; */
        /*     current->act_area.y = widget->abs_area.y + current_y; */
        /*     current_y+=current->abs_area.h; */
        active = PROT__widget_is_entering( current, x, y );
        if( active!=NULL ) {
            PROT_List_ref_reinit( menushell->children );
            SDLGuiTK_list_unlock( menushell->children );
            return active;
        }
        current = (SDLGuiTK_Widget *) SDLGuiTK_list_ref_next( menushell->children );
    }

    SDLGuiTK_list_unlock( menushell->children );

    return NULL;
}

static void * MenuShell_RecursiveDestroy( SDLGuiTK_Widget * widget )
{

    return (void *) NULL;
}

static void * MenuShell_Free( SDLGuiTK_Widget * widget )
{

    return (void *) NULL;
}

static void * MenuShell_Show( SDLGuiTK_Widget * widget, \
                              void * data, void * event )
{
    struct SDLGuiTK_MenuShell * menushell=widget->menushell;

    MenuShell_DrawUpdate( widget );
    MenuShell_DrawBlit( widget );
    widget->shown = 1;
    PROT__context_ref_wmwidget( menushell->wm_widget );

    return (void *) NULL;
}

static void * MenuShell_Hide( SDLGuiTK_Widget * widget, \
                              void * data, void * event )
{
    struct SDLGuiTK_MenuShell * menushell=widget->menushell;

    widget->shown = 0;
    PROT__context_unref_wmwidget( menushell->wm_widget );
    MyWM_WMWidget_Delete( menushell->wm_widget );
    menushell->wm_widget = NULL;

    return (void *) NULL;
}

static void * MenuShell_MouseEnter( SDLGuiTK_Widget * widget, \
                                    void * data, void * event )
{
    MyCursor_Set( SDLGUITK_CURSOR_DEFAULT );
    return (void *) NULL;
}

static void * MenuShell_MouseLeave( SDLGuiTK_Widget * widget, \
                                    void * data, void * event )
{
    struct SDLGuiTK_MenuShell * menushell=widget->menushell;
    if( widget->shown==0) {
        SDLGUITK_ERROR("Workaround: mywm re-call LEAVE event!\n");
        return (void *) NULL;
    }
    MyCursor_Unset();
    PROT__signal_push( menushell->object, SDLGUITK_SIGNAL_TYPE_DEACTIVATE );

    return (void *) NULL;
}

static void * MenuShell_Realize( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{
    struct SDLGuiTK_MenuShell * menushell=widget->menushell;
    /*   MenuShell_Show( widget, NULL ); */
    MenuShell_DrawUpdate( widget );
    MenuShell_DrawBlit( widget );
    MenuShell_UpdatePosition( menushell );
    return (void *) NULL;
}

static void * MenuShell_Activate( SDLGuiTK_Widget * widget, \
                                  void * data, void * event )
{
    PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_REALIZE );
    PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_SHOW );

    return (void *) NULL;
}

static void * MenuShell_DeActivate( SDLGuiTK_Widget * widget, \
                                    void * data, void * event )
{
    PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_HIDE );
    /*   PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_DESTROY ); */

    return (void *) NULL;
}

static void * MenuShell_MousePressed( SDLGuiTK_Widget * widget, \
                                      void * data, void * event )
{

    return (void *) NULL;
}

static void * MenuShell_SelectionDone( SDLGuiTK_Widget * widget, \
                                       void * data, void * event )
{
    /*   SDLGuiTK_MenuShell * menushell=widget->menushell; */
    SDLGuiTK_Menu * menu=widget->menushell->menu;
    PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_LEAVE );
    /*   PROT__signal_push( menu->container->widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT ); */
    PROT__signal_push( menu->container->widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );

    return (void *) NULL;
}

static void MenuShell_Init_functions( struct SDLGuiTK_MenuShell * menushell )
{
    SDLGuiTK_SignalHandler * handler;

    handler = (SDLGuiTK_SignalHandler *) menushell->object->signalhandler;

    menushell->object->widget->RecursiveEntering = MenuShell_RecursiveEntering;
    menushell->object->widget->RecursiveDestroy = MenuShell_RecursiveDestroy;
    menushell->object->widget->Free = MenuShell_Free;

    menushell->object->widget->DrawUpdate = MenuShell_DrawUpdate;
    menushell->object->widget->DrawBlit = MenuShell_DrawBlit;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
            MenuShell_Realize;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
            MenuShell_Show;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
            MenuShell_Hide;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_ENTER]->function = \
            MenuShell_MouseEnter;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_LEAVE]->function = \
            MenuShell_MouseLeave;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_ACTIVATE]->function = \
            MenuShell_Activate;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_DEACTIVATE]->function = \
            MenuShell_DeActivate;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_PRESSED]->function = \
            MenuShell_MousePressed;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_SELECTIONDONE]->function = \
            MenuShell_SelectionDone;
}

void
PROT__menushell_create( SDLGuiTK_Menu * menu ) {
    struct SDLGuiTK_MenuShell * menushell;

    menushell = MenuShell_Create();
    MenuShell_Init_functions( menushell );
    menushell->menu = menu;
    menu->menushell = menushell;
}

void
PROT__menushell_destroy( SDLGuiTK_Menu * menu ) {
    MenuShell_Destroy( menu->menushell );
}


void SDLGuiTK_menu_shell_append( SDLGuiTK_Menu *menu, \
                                 SDLGuiTK_Widget *child ) {
    SDLGuiTK_MenuItem * menuitem=child->container->bin->menuitem;

    child->parent = menu->object->widget;
    menu->selected = menuitem;
    menuitem->menu = menu;
    SDLGuiTK_list_lock( menu->menushell->children );
    SDLGuiTK_list_append( menu->menushell->children, (SDLGuiTK_Object *) child );
    SDLGuiTK_list_unlock( menu->menushell->children );
}

void
PROT__menushell_start( SDLGuiTK_Menu * menu ) {
    menu->menushell->wm_widget = MyWM_WMWidget_New( menu->menushell->object->widget );
    menu->menushell->wm_widget->border_width = 1;
    PROT__signal_push( menu->menushell->object, SDLGUITK_SIGNAL_TYPE_ACTIVATE );

}

