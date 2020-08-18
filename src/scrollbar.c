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

#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "mysdl.h"
#include "signal.h"

#include "object_prot.h"
#include "widget_prot.h"
#include "theme_prot.h"

#include "adjustment_prot.h"
#include "scrollbar_prot.h"


static SDL_bool SDLGuiTK_IS_SCROLLBAR( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->scrollbar!=NULL ) {
            return SDL_TRUE;
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_SCROLLBAR(): widget is not a scrollbar\n" );
    return SDL_FALSE;
}

SDLGuiTK_Scrollbar * SDLGuiTK_SCROLLBAR( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_SCROLLBAR(widget) ) return NULL;
    return widget->scrollbar;
}

static int current_id=0;

static SDLGuiTK_Scrollbar * Scrollbar_create()
{
    SDLGuiTK_Scrollbar * new_scrollbar;

    new_scrollbar = malloc( sizeof( struct SDLGuiTK_Scrollbar ) );
    new_scrollbar->widget = PROT__widget_new();
    new_scrollbar->object = new_scrollbar->widget->object;
    sprintf( new_scrollbar->object->name, "scrollbar%d", ++current_id );
    new_scrollbar->widget->scrollbar = new_scrollbar;

    new_scrollbar->orientation = SDLGUITK_ORIENTATION_HORIZONTAL;
    new_scrollbar->adjustment = NULL;

    new_scrollbar->srf = MySDL_Surface_new ("Scrollbar_srf");

    return new_scrollbar;
}

static void Scrollbar_destroy( SDLGuiTK_Scrollbar * scrollbar )
{
    PROT__widget_destroy( scrollbar->widget );
    free( scrollbar );
}


static double Adjustment_ratio(SDLGuiTK_Adjustment *adjustment)
{
    return (adjustment->value/(adjustment->upper-adjustment->lower));
}

static void * Scrollbar_make_surface(SDLGuiTK_Scrollbar * scrollbar)
{
    SDLGuiTK_Widget * widget=scrollbar->widget;
    SDL_Rect area;
    Uint32 bgcolor, fgcolor;
    SDLGuiTK_Theme * theme;

    theme = PROT__theme_get_and_lock();
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->bgcolor.r, \
                           theme->bgcolor.g, \
                           theme->bgcolor.b, \
                           255 );
    fgcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->ftcolor.r, \
                           theme->ftcolor.g, \
                           theme->ftcolor.b, \
                           255 );
    PROT__theme_unlock( theme );

    MySDL_FillRect( widget->srf, NULL, fgcolor );
    area.x = 1; area.y = 1;
    area.w = widget->srf->srf->w-2; area.h = widget->srf->srf->h-2;
    MySDL_FillRect( widget->srf, &area, bgcolor );

    //draw button
    if(scrollbar->orientation==SDLGUITK_ORIENTATION_HORIZONTAL) {
        int w_diff = area.w-SCROLLBAR_SIZE;
        scrollbar->button_area.x =
            (int) (w_diff*Adjustment_ratio (scrollbar->adjustment));
        scrollbar->button_area.y = 1;
        scrollbar->button_area.w = SCROLLBAR_SIZE;
        scrollbar->button_area.h = area.h;
    } else {
        int h_diff = area.h-SCROLLBAR_SIZE;
        scrollbar->button_area.x = 1;
        scrollbar->button_area.y =
            (int) (h_diff*Adjustment_ratio (scrollbar->adjustment));
        scrollbar->button_area.w = area.w;
        scrollbar->button_area.h = SCROLLBAR_SIZE;
    }
    MySDL_FillRect( widget->srf, &scrollbar->button_area, fgcolor );
    //printf("TEST SB1\n");

    return NULL;
}

static void * Scrollbar_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Scrollbar * scrollbar=SDLGuiTK_SCROLLBAR( widget );

    PROT__widget_reset_req_area( widget );

    PROT__widget_set_req_area(widget, SCROLLBAR_SIZE, SCROLLBAR_SIZE);
    PROT__widget_DrawUpdate( scrollbar->widget );

    return (void *) NULL;
}

static void * Scrollbar_active_area( SDLGuiTK_Scrollbar * scrollbar )
{
    SDLGuiTK_Widget * widget=scrollbar->widget;
    widget->act_area.x = \
                         widget->abs_area.x;
    widget->act_area.y = \
                         widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    scrollbar->button_act_area.x =
        widget->abs_area.x + scrollbar->button_area.x;
    scrollbar->button_act_area.y =
        widget->abs_area.y + scrollbar->button_area.y;
    scrollbar->button_act_area.w = scrollbar->button_area.w;
    scrollbar->button_act_area.h = scrollbar->button_area.h;

    return (void *) NULL;
}

static void * Scrollbar_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Scrollbar * scrollbar=SDLGuiTK_SCROLLBAR(widget);

    PROT__widget_DrawBlit( scrollbar->widget );
    Scrollbar_make_surface(scrollbar);
    Scrollbar_active_area(scrollbar);
/*
    if( widget->top!=NULL ) {
        MySDL_BlitSurface(  scrollbar->srf, NULL, \
                            widget->srf, &scrollbar->area );
    }
*/
    return (void *) NULL;
}


static void * Scrollbar_Realize( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{

    return (void *) NULL;
}

static void * Scrollbar_Show( SDLGuiTK_Widget * widget, \
                              void * data, void * event )
{
/*
    widget->shown = 1;
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }
*/
    return (void *) NULL;
}

static void * Scrollbar_Hide( SDLGuiTK_Widget * widget, \
                              void * data, void * event )
{
/*
    widget->shown = 0;
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }
*/
    return (void *) NULL;
}

static void * Scrollbar_MouseEnter(SDLGuiTK_Widget * widget, \
                                   void * data, void * event )
{
    widget->scrollbar->mousein = 1;

    return NULL;
}

static void * Scrollbar_MouseLeave(SDLGuiTK_Widget * widget,
                                   void * data, void * event )
{
    widget->scrollbar->mousein = 0;
    widget->scrollbar->buttonOn = 0;

    return NULL;
}

static void * Scrollbar_MousePressed(SDLGuiTK_Widget * widget, \
                                     void * data, void * event )
{
    SDLGuiTK_Scrollbar * scrollbar = widget->scrollbar;
    int x, y;
    SDL_GetMouseState (&x, &y);
    //printf("TEST mouse: %d/%d\n", x, y);
    if(x>=scrollbar->button_act_area.x &&
       x<(scrollbar->button_act_area.x+scrollbar->button_act_area.w) &&
       y>=scrollbar->button_act_area.y &&
       y<(scrollbar->button_act_area.y+scrollbar->button_act_area.h)) {
           printf("OK SBButton pressed\n");
           scrollbar->buttonOn = 1;
           scrollbar->mbutton_x = x;
           scrollbar->mbutton_y = y;
       }

    return NULL;
}

static void * Scrollbar_MouseReleased(SDLGuiTK_Widget * widget,
                                      void * data, void * event )
{
    SDLGuiTK_Scrollbar * scrollbar = widget->scrollbar;
    scrollbar->buttonOn = 0;

    return NULL;
}

static void * Scrollbar_MouseClicked(SDLGuiTK_Widget * widget,
                                      void * data, void * event )
{
    return NULL;
}


static SDLGuiTK_Widget * Scrollbar_RecursiveEntering( SDLGuiTK_Widget * widget, \
                                                  int x, int y )
{
    SDLGuiTK_Scrollbar * scrollbar = widget->scrollbar;
    int mx, my;
    SDL_GetMouseState (&mx, &my);
    //printf("TEST mouse_recurseenter: %d/%d --- %d/%d\n", mx, my, x, y);
    int mdiff = 0;
    //int wdiff = 0;
    if(scrollbar->orientation==SDLGUITK_ORIENTATION_HORIZONTAL) {
        mdiff = mx-scrollbar->mbutton_x;
        if(scrollbar->buttonOn==1 && mdiff!=0){
            //wdiff = scrollbar->button_area.x;
            //printf("Mx:%d / AbsX:%d, AbsW:%d\n ", mx, widget->abs_area.x, widget->abs_area.w);
            double value =
                (((double)mx - (double)widget->abs_area.x) / ((double)widget->abs_area.w));
            SDLGuiTK_adjustment_set_value(scrollbar->adjustment,
                                          value);
            scrollbar->button_area.x += mdiff;
            scrollbar->button_act_area.x += mdiff;
            scrollbar->mbutton_x += mdiff;
        }
    } else {
        mdiff = my-scrollbar->mbutton_y;
        if(scrollbar->buttonOn==1 && mdiff!=0){
            //wdiff = scrollbar->button_area.y;
            //printf("Mx:%d / AbsX:%d, AbsW:%d\n ", mx, widget->abs_area.x, widget->abs_area.w);
            double value =
                (((double)my - (double)widget->abs_area.y) /
                 ((double)widget->abs_area.h-SCROLLBAR_SIZE));
            SDLGuiTK_adjustment_set_value(scrollbar->adjustment,
                                          value);
            scrollbar->button_area.y += mdiff;
            scrollbar->button_act_area.y += mdiff;
            scrollbar->mbutton_y += mdiff;
        }
    }
    return NULL;
}


static void * Scrollbar_RecursiveDestroy( SDLGuiTK_Widget * widget )
{

    return (void *) NULL;
}

static void * Scrollbar_Free( SDLGuiTK_Widget * widget )
{
    Scrollbar_destroy( widget->scrollbar );

    return (void *) NULL;
}

static void Scrollbar_set_functions( SDLGuiTK_Scrollbar * scrollbar )
{
    SDLGuiTK_Widget * widget=scrollbar->object->widget;
    SDLGuiTK_SignalHandler * handler;

    handler = (SDLGuiTK_SignalHandler *) scrollbar->object->signalhandler;

    widget->RecursiveEntering = Scrollbar_RecursiveEntering;
    widget->RecursiveDestroy = Scrollbar_RecursiveDestroy;
    widget->Free = Scrollbar_Free;

    widget->DrawUpdate = Scrollbar_DrawUpdate;
    widget->DrawBlit = Scrollbar_DrawBlit;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
            Scrollbar_Realize;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
            Scrollbar_Show;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
            Scrollbar_Hide;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_ENTER]->function = \
            Scrollbar_MouseEnter;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_LEAVE]->function = \
            Scrollbar_MouseLeave;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_PRESSED]->function = \
            Scrollbar_MousePressed;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_RELEASED]->function = \
            Scrollbar_MouseReleased;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_CLICKED]->function = \
            Scrollbar_MouseClicked;

}

SDLGuiTK_Widget *SDLGuiTK_scrollbar_new(int orientation,
                                        SDLGuiTK_Adjustment *adjustment)
{
    SDLGuiTK_Scrollbar * scrollbar;

    scrollbar = Scrollbar_create ();
    scrollbar->orientation = orientation;
    if(adjustment!=NULL)
        scrollbar->adjustment = adjustment;
    else
        scrollbar->adjustment = SDLGuiTK_adjustment_new(0,0,100,1);
    PROT__adjustment_attach(scrollbar->adjustment,scrollbar->widget);

    Scrollbar_set_functions( scrollbar );
    PROT__signal_push( scrollbar->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return scrollbar->widget;
}


