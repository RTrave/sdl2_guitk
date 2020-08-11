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


static SDL_bool SDLGuiTK_IS_RADIO_BUTTON( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->button!=NULL ) {
                    if( widget->container->bin->button->togglebutton!=NULL ) {
                        if( widget->container->bin->button->togglebutton->checkbutton!=NULL ) {
                            if( widget->container->bin->button->togglebutton->checkbutton->radiobutton!=NULL ) {
                                return SDL_TRUE;
                            }
                        }
                    }
                }
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_RADIO_BUTTON(): widget is not a radio button\n" );
    return SDL_FALSE;
}

SDLGuiTK_RadioButton * SDLGuiTK_RADIO_BUTTON( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_RADIO_BUTTON(widget) ) return NULL;
    return widget->container->bin->button->togglebutton->checkbutton->radiobutton;
}


static int current_id=0;

static SDLGuiTK_RadioButton * RadioButton_create()
{
    SDLGuiTK_RadioButton * new_radiobutton;

    new_radiobutton = malloc( sizeof( struct SDLGuiTK_RadioButton ) );
    new_radiobutton->checkbutton = PROT__check_button_new_from_radiobutton (new_radiobutton);
    new_radiobutton->object = new_radiobutton->checkbutton->object;
    sprintf( new_radiobutton->object->name, "radiobutton%d", ++current_id );
    //new_radiobutton->radiobutton = NULL;

    new_radiobutton->indicator_srf = MySDL_Surface_new ("RadioButtonIndic_srf");

    //new_checkbutton->toggled = 0;
    new_radiobutton->group = NULL;
    new_radiobutton->checkbutton->togglebutton->button->bin->margin_left = 28;

    return new_radiobutton;
}

static void RadioButton_destroy( SDLGuiTK_RadioButton * radiobutton )
{
    MySDL_Surface_free( radiobutton->indicator_srf );

    // Button destroy himself and call ToggleButton_destroy
    //PROT__button_destroy( togglebutton->button );
    //if(checkbutton->checkbutton)
    //    PROT__checkbutton_destroy (checkbutton->checkbutton);
    free( radiobutton );
}


static void RadioButton_Indicator(SDLGuiTK_RadioButton * radiobutton)
{
   SDLGuiTK_Theme * theme;
    theme = PROT__theme_get_and_lock();
    if(radiobutton->checkbutton->togglebutton->toggled)
        radiobutton->indicator_srf->srf =
            MyTTF_Render_Blended (radiobutton->indicator_srf->srf, "◆", 28, 0,
                                  theme->ftcolor);
    else
        radiobutton->indicator_srf->srf =
            MyTTF_Render_Blended (radiobutton->indicator_srf->srf, "◇", 28, 0,
                                  theme->ftcolor);
    PROT__theme_unlock( theme );
}

void PROT__radiobutton_DrawUpdate(SDLGuiTK_RadioButton * radiobutton)
{
    SDLGuiTK_Widget * widget = radiobutton->object->widget;

    widget->req_area.w+=28;
    RadioButton_Indicator( radiobutton );
    PROT__widget_set_req_area(widget, 28, radiobutton->indicator_srf->srf->h);
    //widget->container->bin->container->children_area.w -= 28;
    //widget->req_area.h+=SCROLLBAR_SIZE;
}

void PROT__radiobutton_DrawBlit(SDLGuiTK_RadioButton * radiobutton)
{
    SDLGuiTK_Widget * widget = radiobutton->object->widget;
    SDLGuiTK_Button * button=radiobutton->checkbutton->togglebutton->button;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;
    MySDL_Surface * srf=MySDL_Surface_new ("CheckButton_DrawBlit_srf");


    MySDL_CreateRGBSurface( srf, widget->abs_area.w, widget->abs_area.h );
    button->text_area.x = 0;
    button->text_area.y = 0;
    button->text_area.w = widget->abs_area.w;
    button->text_area.h = widget->abs_area.h;

    theme = PROT__theme_get_and_lock();
    bgcolor = SDL_MapRGBA( srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           150 );
    MySDL_FillRect( srf, NULL, bgcolor );
    PROT__theme_unlock( theme );

    radiobutton->indicator_area.x = 0;
    radiobutton->indicator_area.y = 0;
    radiobutton->indicator_area.w = 28;
    radiobutton->indicator_area.h = 28;
    MySDL_BlitSurface(  radiobutton->indicator_srf, NULL, \
                        widget->srf, &radiobutton->indicator_area );

    radiobutton->child_area.x = 28;
    radiobutton->child_area.y = 0;
    //radiobutton->child_area.w = widget->container->children_area.w - 28;
    //radiobutton->child_area.h = widget->container->children_area.h;
    radiobutton->child_area.w = button->bin->child->rel_area.w;
    radiobutton->child_area.h = button->bin->child->rel_area.h;
    if( button->bin->child!=NULL ) {
        MySDL_BlitSurface(  button->bin->child->srf, NULL, \
                            widget->srf, &radiobutton->child_area );
    }
}

void PROT__radiobutton_toggled(SDLGuiTK_RadioButton * radiobutton)
{
    if(!radiobutton->group || !radiobutton->checkbutton->togglebutton->toggled)
        return;
    SDLGuiTK_RadioButton * current = (SDLGuiTK_RadioButton *)
        SDLGuiTK_list_ref_init (radiobutton->group);
    while(current) {
        if(current!=radiobutton)
            if(current->checkbutton->togglebutton->toggled)
                SDLGuiTK_toggle_button_set_active (SDLGuiTK_TOGGLE_BUTTON (current->object->widget), SDL_FALSE);
        current = (SDLGuiTK_RadioButton *)
            SDLGuiTK_list_ref_next (radiobutton->group);
    }
}

void PROT__radiobutton_destroy(SDLGuiTK_RadioButton * radiobutton)
{
    RadioButton_destroy (radiobutton);
}

static void RadioButton_set_functions( SDLGuiTK_RadioButton * button )
{
    SDLGuiTK_SignalHandler * handler;
    handler = (SDLGuiTK_SignalHandler *) button->object->signalhandler;

    //handler->fdefault[SDLGUITK_SIGNAL_TYPE_TOGGLED]->function =
    //        ToggleButton_Toggled;
}


SDLGuiTK_Widget * SDLGuiTK_radio_button_new()
{
    SDLGuiTK_RadioButton * radiobutton;

    radiobutton = RadioButton_create ();
    RadioButton_set_functions( radiobutton );

    return radiobutton->object->widget;
}

SDLGuiTK_Widget * SDLGuiTK_radio_button_new_with_label( char * text )
{
    SDLGuiTK_RadioButton * radiobutton;
    SDLGuiTK_Widget * label;

    radiobutton = RadioButton_create ();
    RadioButton_set_functions( radiobutton );

    label = SDLGuiTK_label_new( text );
    SDLGuiTK_misc_set_padding( label->misc, 3, 5);
    SDLGuiTK_container_add( SDLGuiTK_CONTAINER(radiobutton->object->widget), label );
    SDLGuiTK_widget_show( label );
    PROT__signal_push( radiobutton->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return radiobutton->object->widget;

}

void SDLGuiTK_radio_button_join_group(SDLGuiTK_RadioButton * radiobutton,
                                      SDLGuiTK_RadioButton * group_source)
{
    if(!group_source->group) {
        group_source->group = SDLGuiTK_list_new ();
        SDLGuiTK_list_append (group_source->group, (SDLGuiTK_Object *)group_source);
    }
    radiobutton->group = group_source->group;
    SDLGuiTK_list_append (group_source->group, (SDLGuiTK_Object *)radiobutton);
}

