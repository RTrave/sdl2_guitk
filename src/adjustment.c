/*
   SDL_guitk - GUI toolkit designed for SDL environnements.

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

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "mysdl.h"
#include "myttf.h"
#include "signal.h"
#include "object_prot.h"
#include "widget_prot.h"

#include "adjustment_prot.h"


static int current_id=0;

void PROT__adjustment_attach(SDLGuiTK_Adjustment *adjustment,
                             SDLGuiTK_Widget *parent)
{
    SDLGuiTK_list_append (adjustment->parents,
                          (SDLGuiTK_Object *)parent);
#if DEBUG_LEVEL >= 2
    SDLGUITK_LOG ("__adjustment_attach(): ");
    printf("%s attached to %s\n",
           adjustment->object->name, parent->object->name);
#endif

}

void PROT__adjustment_detach(SDLGuiTK_Adjustment *adjustment,
                             SDLGuiTK_Widget *parent)
{
    SDLGuiTK_Widget *current;
    current =   (SDLGuiTK_Widget *)
                SDLGuiTK_list_remove(adjustment->parents,
                                   (SDLGuiTK_Object *)parent);
    if(current) {
#if DEBUG_LEVEL >= 2
        SDLGUITK_LOG ("__adjustment_detach(): ");
        printf("%s detached from %s\n",
               adjustment->object->name, parent->object->name);
#endif
    } else
        SDLGUITK_ERROR("__adjustment_detach(): widget not found!\n");
    if(SDLGuiTK_list_length (adjustment->parents)==0) {
#if DEBUG_LEVEL >= 2
        SDLGUITK_LOG ("__adjustment_detach(): last parent .. free adjustment\n");
#endif
        SDLGuiTK_list_destroy (adjustment->parents);
        PROT__object_destroy (adjustment->object);
        free(adjustment);
    }
}


SDLGuiTK_Object *SDLGuiTK_ADJUSTMENT( SDLGuiTK_Adjustment *adjustment )
{
    return adjustment->object;
}

SDLGuiTK_Adjustment * SDLGuiTK_adjustment_new(double value,
                                              double lower,
                                              double upper,
                                              double step_increment)
{
    SDLGuiTK_Adjustment * new_adjustment;

    new_adjustment = malloc( sizeof( struct SDLGuiTK_Adjustment ) );
    new_adjustment->object = PROT__object_new();
    new_adjustment->object->adjustment = new_adjustment;
    sprintf( new_adjustment->object->name, "adjustment%d", ++current_id );

    new_adjustment->parents = SDLGuiTK_list_new ();
    new_adjustment->value = value;
    new_adjustment->lower = lower;
    new_adjustment->upper = upper;
    new_adjustment->step_increment = step_increment;

    return new_adjustment;
}

void SDLGuiTK_adjustment_set_value(SDLGuiTK_Adjustment *adjustment,
                                   double               value)
{
    if(value<adjustment->lower)
        value = adjustment->lower;
    if(value>adjustment->upper)
        value = adjustment->upper;
    adjustment->value = value;
    PROT__signal_push(adjustment->object,
                      SDLGUITK_SIGNAL_TYPE_VALUECHANGED );

    SDLGuiTK_Widget * current;
    current = (SDLGuiTK_Widget*)
        SDLGuiTK_list_ref_init (adjustment->parents);
    while(current) {
        PROT__signal_push ( current->object,
                            SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY);
        current = (SDLGuiTK_Widget*)
            SDLGuiTK_list_ref_next (adjustment->parents);
    }
}

double SDLGuiTK_adjustment_get_value(SDLGuiTK_Adjustment *adjustment)
{
    return adjustment->value;
}

double SDLGuiTK_adjustment_get_fraction(SDLGuiTK_Adjustment *adjustment)
{
    return (adjustment->value-adjustment->lower) /
        (adjustment->upper-adjustment->lower);
}

void PROT__adjustment_set_fraction(SDLGuiTK_Adjustment *adjustment,
                                   double               fraction)
{
    double value;
    value = (adjustment->lower + fraction*(adjustment->upper-adjustment->lower));
    if(value<adjustment->lower)
        value = adjustment->lower;
    if(value>adjustment->upper)
        value = adjustment->upper;
    adjustment->value = value;
    PROT__signal_push(adjustment->object,
                      SDLGUITK_SIGNAL_TYPE_VALUECHANGED );

    SDLGuiTK_Widget * current;
    current = (SDLGuiTK_Widget*)
        SDLGuiTK_list_ref_init (adjustment->parents);
    while(current) {
        PROT__signal_push ( current->object,
                            SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY);
        current = (SDLGuiTK_Widget*)
            SDLGuiTK_list_ref_next (adjustment->parents);
    }
}

