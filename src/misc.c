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

#include "texts/label_prot.h"
#include "misc_prot.h"



SDL_bool SDLGuiTK_IS_MISC( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->misc!=NULL ) {
            return SDL_TRUE;
        }
    }
    return SDL_FALSE;
}

SDLGuiTK_Misc * SDLGuiTK_MISC( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_MISC(widget) ) {
        SDLGUITK_ERROR( "SDLGuiTK_IS_MISC(): widget is not a misc\n" );
        return NULL;
    }
    return widget->misc;
}


static SDLGuiTK_Misc * Misc_create()
{
    SDLGuiTK_Misc * new_misc;

    new_misc = malloc( sizeof( struct SDLGuiTK_Misc ) );
    new_misc->widget = PROT__widget_new();
    new_misc->object = new_misc->widget->object;
    new_misc->widget->misc = new_misc;

    new_misc->label = NULL;
    new_misc->image = NULL;

    new_misc->xalign = 0.5;
    new_misc->yalign = 0.5;
    new_misc->xpad = 0;
    new_misc->ypad = 0;

    new_misc->area.x = 0;
    new_misc->area.y = 0;
    new_misc->area.w = 0;
    new_misc->area.h = 0;

    return new_misc;
}

static void Misc_destroy( SDLGuiTK_Misc * misc )
{
    PROT__widget_destroy( misc->widget );
    free( misc );
}


SDLGuiTK_Misc * PROT__misc_new()
{
    SDLGuiTK_Misc * misc;

    misc = Misc_create();

    return misc;
}

void PROT__misc_destroy( SDLGuiTK_Misc * misc )
{
    Misc_destroy( misc );
}


int PROT__misc_DrawUpdate( SDLGuiTK_Misc * misc )
{
    misc->area.x = misc->xpad;
    misc->area.y = misc->ypad;

    misc->widget->req_area.w = misc->area.w + 2*misc->xpad;
    misc->widget->req_area.h = misc->area.h + 2*misc->ypad;

    PROT__widget_DrawUpdate(  misc->widget );

    return 1;
}

void PROT__misc_DrawBlit(   SDLGuiTK_Misc * misc )
{
    int wdiff=0, hdiff=0;
    int xdiff=0, ydiff=0;
    SDL_Rect bg_area;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;

#if DEBUG_LEVEL >= 3
    /* printf("*** %s PROT__misc_DrawBlit\n", misc->widget->object->name); */
    /* printf("*** misc req_area w:%d h:%d\n", misc->widget->req_area.w, misc->widget->req_area.h); */
    /* printf("*** misc abs_area w:%d h:%d\n", misc->widget->abs_area.w, misc->widget->abs_area.h); */
#endif

    PROT__widget_DrawBlit(  misc->widget );
    wdiff = misc->widget->req_area.w - misc->area.w - 2*misc->xpad;
    hdiff = misc->widget->req_area.h - misc->area.h - 2*misc->ypad;

    if( wdiff>0 ) {
        xdiff = (int)((wdiff)*(misc->xalign));
    }
    if( hdiff>0 ) {
        ydiff = (int)((hdiff)*(misc->yalign));
    }

    misc->area.x += xdiff;
    misc->area.y += ydiff;

    theme = PROT__theme_get_and_lock();
#if DEBUG_LEVEL >= 3
    bgcolor = SDL_MapRGBA( misc->widget->srf->srf->format, 0xff, 0x00, 0x00, 0xff );
#else
    bgcolor = SDL_MapRGBA( misc->widget->srf->srf->format, \
                           theme->wgcolor.r, \
                           theme->wgcolor.g, \
                           theme->wgcolor.b, \
                           255 );
#endif
    PROT__theme_unlock( theme );
    bg_area.x = misc->xpad;
    bg_area.y = misc->ypad;
    bg_area.w = misc->widget->abs_area.w - 2*misc->xpad;
    bg_area.h = misc->widget->abs_area.h - 2*misc->ypad;
    MySDL_FillRect( misc->widget->srf, &bg_area, bgcolor );
}


void SDLGuiTK_misc_set_alignment( SDLGuiTK_Misc * misc, \
                                  float xalign, \
                                  float yalign )
{
    misc->xalign = xalign;
    misc->yalign = yalign;

    if( misc->widget->parent ) {
        PROT__signal_push( misc->widget->parent->object,
                           SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    }
}

void SDLGuiTK_misc_set_padding(   SDLGuiTK_Misc * misc, \
                                  int xpad, \
                                  int ypad )
{
    misc->xpad = xpad;
    misc->ypad = ypad;

    if( misc->widget->parent ) {
        PROT__signal_push( misc->widget->parent->object,
                           SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    }
}

void SDLGuiTK_misc_get_alignment( SDLGuiTK_Misc * misc, \
                                  float * xalign, \
                                  float * yalign )
{
    *xalign = misc->xalign;
    *yalign = misc->yalign;
}

void SDLGuiTK_misc_get_padding(   SDLGuiTK_Misc * misc, \
                                  int * xpad, \
                                  int * ypad )
{
    *xpad = misc->xpad;
    *ypad = misc->ypad;
}

