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
#include "context_prot.h"

#include "progressbar_prot.h"


static SDL_bool SDLGuiTK_IS_PROGRESSBAR( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->progressbar!=NULL ) {
            return SDL_TRUE;
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_PROGRESSBAR(): widget is not a progressbar\n" );
    return SDL_FALSE;
}

SDLGuiTK_ProgressBar * SDLGuiTK_PROGRESSBAR( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_PROGRESSBAR(widget) ) return NULL;
    return widget->progressbar;
}

static int current_id=0;

static SDLGuiTK_ProgressBar * ProgressBar_create()
{
    SDLGuiTK_ProgressBar * new_progressbar;

    new_progressbar = malloc( sizeof( struct SDLGuiTK_ProgressBar ) );
    new_progressbar->widget = PROT__widget_new();
    new_progressbar->object = new_progressbar->widget->object;
    sprintf( new_progressbar->object->name, "progressbar%d", ++current_id );
    new_progressbar->widget->progressbar = new_progressbar;

    new_progressbar->orientation = SDLGUITK_ORIENTATION_HORIZONTAL;
    new_progressbar->fraction = 0;

    new_progressbar->srf = MySDL_Surface_new ("ProgressBar_srf");

    return new_progressbar;
}

static void ProgressBar_destroy( SDLGuiTK_ProgressBar * progressbar )
{
    MySDL_Surface_free (progressbar->srf);
    PROT__widget_destroy( progressbar->widget );
    free( progressbar );
}


static void * ProgressBar_make_surface(SDLGuiTK_ProgressBar * progressbar)
{
    SDLGuiTK_Widget * widget=progressbar->widget;
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

    if(progressbar->orientation==SDLGUITK_ORIENTATION_HORIZONTAL) {
        area.x = 0; area.y = 0;
        area.w = widget->srf->srf->w; area.h = PROGRESSBAR_SIZE;
    } else {
        area.x = 0; area.y = 0;
        area.w = PROGRESSBAR_SIZE; area.h = widget->srf->srf->h;
    }
    MySDL_FillRect( widget->srf, &area, fgcolor );
    if(progressbar->orientation==SDLGUITK_ORIENTATION_HORIZONTAL) {
        area.x = 1; area.y = 1;
        area.w = widget->srf->srf->w-2; area.h = PROGRESSBAR_SIZE-2;
    } else {
        area.x = 1; area.y = 1;
        area.w = PROGRESSBAR_SIZE-2; area.h = widget->srf->srf->h-2;
    }
    MySDL_FillRect( widget->srf, &area, bgcolor );

    //draw bar
    if(progressbar->orientation==SDLGUITK_ORIENTATION_HORIZONTAL) {
        int w_diff = (int)((double)area.w*progressbar->fraction);
        if(w_diff<=0) return NULL;
        progressbar->area.x = 1;
        progressbar->area.y = 1;
        progressbar->area.w = w_diff;
        progressbar->area.h = PROGRESSBAR_SIZE-2;
    } else {
        int h_diff = (int)((double)area.h*progressbar->fraction);
        if(h_diff<=0) return NULL;
        progressbar->area.x = 1;
        progressbar->area.y = 1;
        progressbar->area.w = PROGRESSBAR_SIZE-2;
        progressbar->area.h = h_diff;
    }
    MySDL_FillRect( widget->srf, &progressbar->area, fgcolor );
    return NULL;
}

static void * ProgressBar_active_area( SDLGuiTK_ProgressBar * progressbar )
{
    SDLGuiTK_Widget * widget=progressbar->widget;
    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    return (void *) NULL;
}


static void * ProgressBar_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_ProgressBar * progressbar=SDLGuiTK_PROGRESSBAR( widget );

    PROT__widget_reset_req_area( widget );

    PROT__widget_set_req_area(widget, PROGRESSBAR_SIZE, PROGRESSBAR_SIZE);
    PROT__widget_DrawUpdate( progressbar->widget );

    return (void *) NULL;
}

static void * ProgressBar_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_ProgressBar * progressbar=SDLGuiTK_PROGRESSBAR(widget);
    PROT__widget_DrawBlit( progressbar->widget );
    ProgressBar_make_surface(progressbar);
    ProgressBar_active_area(progressbar);
    return (void *) NULL;
}

static SDLGuiTK_Widget * ProgressBar_RecursiveEntering( SDLGuiTK_Widget * widget, \
                                                        int x, int y )
{
    //SDLGuiTK_ProgressBar * progressbar = widget->progressbar;
    return NULL;
}


static void * ProgressBar_RecursiveDestroy( SDLGuiTK_Widget * widget )
{

    return (void *) NULL;
}

static void * ProgressBar_Free( SDLGuiTK_Widget * widget )
{
    ProgressBar_destroy( widget->progressbar );

    return (void *) NULL;
}


static void ProgressBar_set_functions( SDLGuiTK_ProgressBar * progressbar )
{
    SDLGuiTK_Widget * widget=progressbar->object->widget;

    widget->RecursiveEntering = ProgressBar_RecursiveEntering;
    widget->RecursiveDestroy = ProgressBar_RecursiveDestroy;
    widget->Free = ProgressBar_Free;

    widget->DrawUpdate = ProgressBar_DrawUpdate;
    widget->DrawBlit = ProgressBar_DrawBlit;

}

SDLGuiTK_Widget *SDLGuiTK_progress_bar_new()
{
    SDLGuiTK_ProgressBar * progressbar;

    progressbar = ProgressBar_create ();
    ProgressBar_set_functions( progressbar );
    PROT__signal_push( progressbar->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return progressbar->widget;
}

void SDLGuiTK_progress_bar_set_orientation (SDLGuiTK_ProgressBar *pbar,
                                            int orientation)
{
    pbar->orientation = orientation;
    if(pbar->widget->parent)
        PROT__signal_push( pbar->widget->parent->object, SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
}

void SDLGuiTK_progress_bar_set_fraction (SDLGuiTK_ProgressBar *pbar,
                                         double fraction)
{
    if(fraction<0 || fraction>1)
        return;
    pbar->fraction = fraction;
    if(pbar->widget->parent)
        PROT__signal_push( pbar->widget->parent->object, SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
}

double SDLGuiTK_progress_bar_get_fraction (SDLGuiTK_ProgressBar *pbar)
{
    return pbar->fraction;
}

