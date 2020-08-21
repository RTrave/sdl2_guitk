/*
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

   Copyright (C) 2007 Trave Roman

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
#include "../theme_prot.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"

#include "tooltip_prot.h"


SDLGuiTK_Tooltip *SDLGuiTK_TOOLTIP( SDLGuiTK_Tooltip *tooltip ) {
  return tooltip; /* ??? */
}


static int current_id=0;

static SDLGuiTK_Tooltip * Tooltip_create()
{
    SDLGuiTK_Tooltip * new_tooltip;

    new_tooltip = malloc( sizeof( struct SDLGuiTK_Tooltip ) );
    new_tooltip->bin = PROT__bin_new();
    new_tooltip->bin->tooltip = new_tooltip;
    new_tooltip->object = new_tooltip->bin->object;
    sprintf( new_tooltip->object->name, "tooltip%d", ++current_id );
    new_tooltip->srf = MySDL_Surface_new ("Tooltip_srf");
    new_tooltip->surface2D = Surface2D_new ();

    new_tooltip->object->widget->top = new_tooltip->object->widget;
    new_tooltip->object->widget->parent = new_tooltip->object->widget;

    return new_tooltip;
}

static void Tooltip_destroy( SDLGuiTK_Tooltip * tooltip )
{
    Surface2D_destroy (tooltip->surface2D);
    MySDL_Surface_free( tooltip->srf );
    PROT__bin_destroy( tooltip->bin );
    free( tooltip );
}

static void * Tooltip_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Tooltip * tooltip=widget->container->bin->tooltip;

    PROT__widget_reset_req_area( widget );
    PROT__bin_DrawUpdate( tooltip->bin );

    return (void *) NULL;
}

static void * Tooltip_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Tooltip * tooltip=widget->container->bin->tooltip;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;

    //tooltip->srf=MySDL_Surface_new ("Button_DrawBlit_srf");

    PROT__bin_DrawBlit( tooltip->bin );

    MySDL_CreateRGBSurface( tooltip->srf, widget->abs_area.w, widget->abs_area.h );
    tooltip->area.x = 1;
    tooltip->area.y = 1;
    tooltip->area.w = widget->abs_area.w-2;
    tooltip->area.h = widget->abs_area.h-2;

    theme = PROT__theme_get_and_lock();
    bgcolor = SDL_MapRGBA( tooltip->srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           255 );
    MySDL_FillRect( tooltip->srf, NULL, bgcolor );
    bgcolor = SDL_MapRGBA( tooltip->srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           0 );
    MySDL_FillRect( tooltip->srf, &tooltip->area, bgcolor );
    PROT__theme_unlock( theme );

    if( tooltip->bin->child ) {
        if( tooltip->bin->child->visible ) {
            MySDL_BlitSurface(  tooltip->bin->child->srf, NULL, \
                                widget->srf, &tooltip->bin->child->rel_area );
            MySDL_BlitSurface(  tooltip->srf, NULL, \
                                widget->srf, &tooltip->bin->child->rel_area );

        }
    }

    return (void *) NULL;
}


SDLGuiTK_Tooltip * PROT__Tooltip_create()
{
    SDLGuiTK_Tooltip * tooltip;

    tooltip = Tooltip_create();

    tooltip->label = SDLGuiTK_label_new( "" );
    SDLGuiTK_misc_set_padding( tooltip->label->misc, 10, 5);
    SDLGuiTK_container_add( SDLGuiTK_CONTAINER(tooltip->object->widget), tooltip->label );
    SDLGuiTK_widget_show( tooltip->label );

    return tooltip;
}

void PROT__Tooltip_destroy(SDLGuiTK_Tooltip * tooltip)
{
    Tooltip_destroy( tooltip );
}


void PROT__Tooltip_update( SDLGuiTK_Tooltip * tooltip, char *text )
{
    SDLGuiTK_label_set_text (tooltip->label, text);
    Tooltip_DrawUpdate( tooltip->object->widget );
    Tooltip_DrawBlit( tooltip->object->widget );
    Surface2D_update (tooltip->surface2D, tooltip->object->widget->srf->srf);
}

void PROT__Tooltip_blit( SDLGuiTK_Tooltip * tooltip )
{
    int mouse_x, mouse_y;
    SDL_GetMouseState( &mouse_x, &mouse_y );
    Surface2D_blitsurface( tooltip->surface2D, mouse_x+8, mouse_y+4 );
}


