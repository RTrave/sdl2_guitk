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


static SDL_bool SDLGuiTK_IS_TOGGLE_BUTTON( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->button!=NULL ) {
                    if( widget->container->bin->button->togglebutton!=NULL ) {
                        return SDL_TRUE;
                    }
                }
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_TOGGLE_BUTTON(): widget is not a toggle button\n" );
    return SDL_FALSE;
}

SDLGuiTK_ToggleButton * SDLGuiTK_TOGGLE_BUTTON( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_TOGGLE_BUTTON(widget) ) return NULL;
    return widget->container->bin->button->togglebutton;
}


static int current_id=0;

static SDLGuiTK_ToggleButton * ToggleButton_create()
{
    SDLGuiTK_ToggleButton * new_togglebutton;

    new_togglebutton = malloc( sizeof( struct SDLGuiTK_ToggleButton ) );
    new_togglebutton->button = PROT__button_new_from_togglebutton(new_togglebutton);
    new_togglebutton->object = new_togglebutton->button->bin->object;
    sprintf( new_togglebutton->object->name, "togglebutton%d", ++current_id );
    new_togglebutton->checkbutton = NULL;

    new_togglebutton->toggled_srf = MySDL_Surface_new ("ToggleButton_srf");

    new_togglebutton->toggled = 0;

    return new_togglebutton;
}

static void ToggleButton_destroy( SDLGuiTK_ToggleButton * togglebutton )
{
    MySDL_Surface_free( togglebutton->toggled_srf );

    // Button destroy himself and call ToggleButton_destroy
    //PROT__button_destroy( togglebutton->button );
    if(togglebutton->checkbutton)
        PROT__checkbutton_destroy (togglebutton->checkbutton);
    free( togglebutton );
}


void PROT__togglebutton_DrawUpdate(SDLGuiTK_ToggleButton * togglebutton)
{
    if(togglebutton->checkbutton) {
        PROT__checkbutton_DrawUpdate (togglebutton->checkbutton);
        return;
    }
}

void PROT__togglebutton_DrawBlit(SDLGuiTK_ToggleButton * togglebutton)
{
    SDLGuiTK_Widget * widget=togglebutton->object->widget;
    SDLGuiTK_Button * button=togglebutton->button;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;
    MySDL_Surface * srf=MySDL_Surface_new ("ToggleButton_DrawBlit_srf");

    if(togglebutton->checkbutton) {
        PROT__checkbutton_DrawBlit (togglebutton->checkbutton);
        return;
    }
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

    button->text_area.x = 2;
    button->text_area.y = 2;
    button->text_area.w = widget->abs_area.w-4;
    button->text_area.h = widget->abs_area.h-4;
    bgcolor = SDL_MapRGBA( srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           50 );
    MySDL_FillRect( srf, &button->text_area, bgcolor );

    button->text_area.x = SDLGUITK_BUTTONBORDER;
    button->text_area.y = SDLGUITK_BUTTONBORDER;
    button->text_area.w = widget->abs_area.w-(2*SDLGUITK_BUTTONBORDER);
    button->text_area.h = widget->abs_area.h-(2*SDLGUITK_BUTTONBORDER);
    bgcolor = SDL_MapRGBA( srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           0 );
    MySDL_FillRect(srf, &button->text_area,
                       SDL_MapRGBA(srf->srf->format, 0, 0, 0, 0));
    PROT__theme_unlock( theme );

    if( button->bin->child!=NULL ) {
        if( button->bin->child->shown==1 ) {
            MySDL_BlitSurface(  button->bin->child->srf, NULL, \
                                widget->srf, &button->bin->child->rel_area );
            MySDL_BlitSurface(  srf, NULL, \
                                widget->srf, &button->bin->child->rel_area );
        }
    }
    if(togglebutton->toggled) {
        MySDL_FillRect(srf, &button->text_area,
                       SDL_MapRGBA(srf->srf->format, 255, 255, 255, 70));
        MySDL_BlitSurface(srf, NULL, \
                          widget->srf, &button->bin->child->rel_area );
    }
    MySDL_Surface_free( srf );
}

void PROT__togglebutton_clicked(SDLGuiTK_ToggleButton * togglebutton)
{
    if(togglebutton->toggled==0)
        togglebutton->toggled = 1;
    else
        togglebutton->toggled = 0;
    PROT__signal_push( togglebutton->object, SDLGUITK_SIGNAL_TYPE_TOGGLED );
}

void PROT__togglebutton_destroy(SDLGuiTK_ToggleButton * togglebutton)
{
    ToggleButton_destroy (togglebutton);
}


static void * ToggleButton_Toggled( SDLGuiTK_Widget * widget, \
                                    void * data, void * event )
{
    SDLGuiTK_ToggleButton * togglebutton=widget->container->bin->button->togglebutton;

     if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }

    return (void *) NULL;
}

static void ToggleButton_set_functions( SDLGuiTK_ToggleButton * button )
{
    SDLGuiTK_SignalHandler * handler;
    handler = (SDLGuiTK_SignalHandler *) button->object->signalhandler;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_TOGGLED]->function = \
            ToggleButton_Toggled;
}


SDLGuiTK_Widget * SDLGuiTK_toggle_button_new()
{
    SDLGuiTK_ToggleButton * togglebutton;

    togglebutton = ToggleButton_create();
    ToggleButton_set_functions( togglebutton );

    return togglebutton->object->widget;
}

SDLGuiTK_Widget * SDLGuiTK_toggle_button_new_with_label( char * text )
{
    SDLGuiTK_ToggleButton * togglebutton;
    SDLGuiTK_Widget * label;

    togglebutton = ToggleButton_create();
    ToggleButton_set_functions( togglebutton );

    label = SDLGuiTK_label_new( text );
    SDLGuiTK_misc_set_padding( label->misc, 3, 5);
    SDLGuiTK_container_add( SDLGuiTK_CONTAINER(togglebutton->object->widget), label );
    SDLGuiTK_widget_show( label );
    PROT__signal_push( togglebutton->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return togglebutton->object->widget;
}

SDL_bool SDLGuiTK_toggle_button_get_active( SDLGuiTK_ToggleButton * togglebutton )
{
    if(togglebutton->toggled)
        return SDL_TRUE;
    return SDL_FALSE;
}

void SDLGuiTK_toggle_button_set_active( SDLGuiTK_ToggleButton * togglebutton,
                                        SDL_bool is_active )
{
    if(togglebutton->toggled) {
        togglebutton->toggled = 0;
        PROT__signal_push( togglebutton->object, SDLGUITK_SIGNAL_TYPE_TOGGLED );
        return;
    }
    togglebutton->toggled = 1;
    PROT__signal_push( togglebutton->object, SDLGUITK_SIGNAL_TYPE_TOGGLED );
}

SDLGuiTK_ToggleButton * PROT__toggle_button_new_from_checkbutton(
                                    SDLGuiTK_CheckButton * checkbutton)
{
    SDLGuiTK_ToggleButton * togglebutton;

    togglebutton = ToggleButton_create ();
    togglebutton->checkbutton = checkbutton;
    ToggleButton_set_functions (togglebutton);

    return togglebutton;
}


