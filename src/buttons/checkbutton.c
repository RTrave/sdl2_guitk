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

#include "../debug.h"
#include "../theme_prot.h"
#include "../signal.h"
#include "../myttf.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"

#include "button_prot.h"
#include "togglebutton_prot.h"
#include "checkbutton_prot.h"
#include "radiobutton_prot.h"


SDL_bool SDLGuiTK_IS_CHECK_BUTTON( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->button!=NULL ) {
                    if( widget->container->bin->button->togglebutton!=NULL ) {
                        if( widget->container->bin->button->togglebutton->checkbutton!=NULL ) {
                            return SDL_TRUE;
                        }
                    }
                }
            }
        }
    }
    return SDL_FALSE;
}

SDLGuiTK_CheckButton * SDLGuiTK_CHECK_BUTTON( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_CHECK_BUTTON(widget) ) {
        SDLGUITK_ERROR( "SDLGuiTK_IS_CHECK_BUTTON(): widget is not a check button\n" );
        return NULL;
    }
    return widget->container->bin->button->togglebutton->checkbutton;
}


static int current_id=0;

static SDLGuiTK_CheckButton * CheckButton_create()
{
    SDLGuiTK_CheckButton * new_checkbutton;

    new_checkbutton = malloc( sizeof( struct SDLGuiTK_CheckButton ) );
    new_checkbutton->togglebutton = PROT__toggle_button_new_from_checkbutton (new_checkbutton);
    new_checkbutton->object = new_checkbutton->togglebutton->object;
    sprintf( new_checkbutton->object->name, "checkbutton%d", ++current_id );
    new_checkbutton->radiobutton = NULL;

    new_checkbutton->indicator_srf = MySDL_Surface_new ("CheckButtonIndic_srf");

    new_checkbutton->togglebutton->button->bin->margin_left = 28;

    return new_checkbutton;
}

static void CheckButton_destroy( SDLGuiTK_CheckButton * checkbutton )
{
    MySDL_Surface_free( checkbutton->indicator_srf );

    // Button destroy himself and call ToggleButton_destroy
    //PROT__button_destroy( togglebutton->button );
    if(checkbutton->radiobutton)
        PROT__radiobutton_destroy (checkbutton->radiobutton);
    free( checkbutton );
}


static void CheckButton_Indicator(SDLGuiTK_CheckButton * checkbutton)
{
   SDLGuiTK_Theme * theme;

    theme = PROT__theme_get_and_lock();
    if(checkbutton->togglebutton->toggled)
        checkbutton->indicator_srf->srf =
            MyTTF_Render_Blended (checkbutton->indicator_srf->srf, "◼", 28, 0,
                                  theme->ftcolor);
    else
        checkbutton->indicator_srf->srf =
            MyTTF_Render_Blended (checkbutton->indicator_srf->srf, "◻", 28, 0,
                                  theme->ftcolor);
    PROT__theme_unlock( theme );
}

void PROT__checkbutton_DrawUpdate(SDLGuiTK_CheckButton * checkbutton)
{
    SDLGuiTK_Widget * widget = checkbutton->object->widget;

    if(checkbutton->radiobutton) {
        PROT__radiobutton_DrawUpdate (checkbutton->radiobutton);
        return;
    }
    widget->req_area.w+=28;
    CheckButton_Indicator( checkbutton );
    PROT__widget_set_req_area(widget, 28, checkbutton->indicator_srf->srf->h);
}

void PROT__checkbutton_DrawBlit(SDLGuiTK_CheckButton * checkbutton)
{
    SDLGuiTK_Widget * widget = checkbutton->object->widget;
    SDLGuiTK_Button * button=checkbutton->togglebutton->button;
    //Uint32 bgcolor;
    //SDLGuiTK_Theme * theme;

    if(checkbutton->radiobutton) {
        PROT__radiobutton_DrawBlit (checkbutton->radiobutton);
        return;
    }
    button->text_area.x = 0;
    button->text_area.y = 0;
    button->text_area.w = widget->abs_area.w;
    button->text_area.h = widget->abs_area.h;
/*
    theme = PROT__theme_get_and_lock();
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           150 );
    PROT__theme_unlock( theme );
*/
    checkbutton->indicator_area.x = 0;
    checkbutton->indicator_area.y = 0;
    checkbutton->indicator_area.w = 28;
    checkbutton->indicator_area.h = 28;
    MySDL_BlitSurface(  checkbutton->indicator_srf, NULL, \
                        widget->srf, &checkbutton->indicator_area );

    checkbutton->child_area.x = 28;
    checkbutton->child_area.y = 0;
    //checkbutton->child_area.w = widget->container->children_area.w - 28;
    //checkbutton->child_area.h = widget->container->children_area.h;
    checkbutton->child_area.w = button->bin->child->rel_area.w;
    checkbutton->child_area.h = button->bin->child->rel_area.h;
    if( button->bin->child!=NULL ) {
        MySDL_BlitSurface(  button->bin->child->srf, NULL, \
                            widget->srf, &checkbutton->child_area );
    }
}

void PROT__checkbutton_toggled(SDLGuiTK_CheckButton * checkbutton)
{
    if(checkbutton->radiobutton)
        PROT__radiobutton_toggled (checkbutton->radiobutton);
}

void PROT__checkbutton_destroy(SDLGuiTK_CheckButton * checkbutton)
{
    CheckButton_destroy (checkbutton);
}

static void CheckButton_set_functions( SDLGuiTK_CheckButton * button )
{

}


SDLGuiTK_Widget * SDLGuiTK_check_button_new()
{
    SDLGuiTK_CheckButton * checkbutton;

    checkbutton = CheckButton_create ();
    CheckButton_set_functions( checkbutton );
    PROT__signal_push( checkbutton->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return checkbutton->object->widget;
}

SDLGuiTK_Widget * SDLGuiTK_check_button_new_with_label( char * text )
{
    SDLGuiTK_CheckButton * checkbutton;
    SDLGuiTK_Widget * label;

    checkbutton = CheckButton_create ();
    CheckButton_set_functions( checkbutton );

    label = SDLGuiTK_label_new( text );
    SDLGuiTK_misc_set_padding( label->misc, 3, 5);
    SDLGuiTK_container_add( SDLGuiTK_CONTAINER(checkbutton->object->widget), label );
    SDLGuiTK_widget_show( label );
    PROT__signal_push( checkbutton->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return checkbutton->object->widget;

}

SDLGuiTK_CheckButton * PROT__check_button_new_from_radiobutton(
                                    SDLGuiTK_RadioButton * radiobutton)
{
    SDLGuiTK_CheckButton * checkbutton;

    checkbutton = CheckButton_create ();
    checkbutton->radiobutton = radiobutton;
    CheckButton_set_functions (checkbutton);

    return checkbutton;
}


