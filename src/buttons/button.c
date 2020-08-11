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

#include "togglebutton_prot.h"
#include "button_prot.h"


static SDL_bool SDLGuiTK_IS_BUTTON( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->button!=NULL ) {
                    return SDL_TRUE;
                }
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_BUTTON(): widget is not a button\n" );
    return SDL_FALSE;
}

SDLGuiTK_Button * SDLGuiTK_BUTTON( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_BUTTON(widget) ) return NULL;
    return widget->container->bin->button;
}


static int current_id=0;

static SDLGuiTK_Button * Button_create()
{
    SDLGuiTK_Button * new_button;

    new_button = malloc( sizeof( struct SDLGuiTK_Button ) );
    new_button->bin = PROT__bin_new();
    new_button->bin->button = new_button;
    new_button->object = new_button->bin->object;
    sprintf( new_button->object->name, "button%d", ++current_id );

    new_button->text = calloc( 256, sizeof( char ) );
    strcpy( new_button->text, new_button->object->name );

    new_button->srf = MySDL_Surface_new ("Button_srf");

    new_button->text_flag = 1;
    new_button->pressed_flag = 0;
    new_button->text_srf = MySDL_Surface_new ("Button_text_srf");
    new_button->active_alpha_mod = -0.005;
    new_button->active_srf = MySDL_Surface_new ("Button_active_srf");
    new_button->active_cflag = 0;
    new_button->togglebutton = NULL;

    return new_button;
}

static void Button_destroy( SDLGuiTK_Button * button )
{
    MySDL_Surface_free( button->active_srf );
    MySDL_Surface_free( button->text_srf );
    MySDL_Surface_free( button->srf );

    PROT__bin_destroy( button->bin );
    if(button->togglebutton)
        PROT__togglebutton_destroy (button->togglebutton);
    free(button->text);
    free( button );
}


static void Button_make_surface( SDLGuiTK_Button * button )
{
    SDLGuiTK_Theme * theme;
    SDLGuiTK_Widget * widget=button->object->widget;

    theme = PROT__theme_get_and_lock();
    button->text_srf->srf = MyTTF_Render_Solid_Block(  button->text_srf->srf, \
                                                       button->text, 16, \
                                                       MyTTF_STYLE_NORMAL, \
                                                       theme->ftcolor, \
                                                       40 );
    button->text_area.x = SDLGUITK_BUTTONBORDER;
    button->text_area.y = SDLGUITK_BUTTONBORDER;
    button->text_area.w = button->text_srf->srf->w;
    button->text_area.h = button->text_srf->srf->h;

    widget->rel_area.w = button->text_srf->srf->w + 2*SDLGUITK_BUTTONBORDER;
    widget->rel_area.h = button->text_srf->srf->h + 2*SDLGUITK_BUTTONBORDER;

    PROT__theme_unlock( theme );

    button->text_flag = 0;
}

static void * Button_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Button * button=widget->container->bin->button;

    PROT__widget_reset_req_area( widget );

    PROT__bin_DrawUpdate( button->bin );

    if(button->togglebutton)
        PROT__togglebutton_DrawUpdate (button->togglebutton);

    return (void *) NULL;
}

static void Button_active_area( SDLGuiTK_Button * button )
{
    SDLGuiTK_Widget * widget=button->bin->container->widget;

    widget->act_area.x = widget->abs_area.x /* + button->bin->area.x */;
    widget->act_area.y = widget->abs_area.y /* + button->bin->area.y */;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;
}
static void ButtonCache( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Button * button=widget->container->bin->button;
    Uint32 bdcolor2;

    Button_active_area( button );

    if( button->active_srf->srf==NULL ) {
        MySDL_CreateRGBSurface(  button->active_srf,\
                                 widget->act_area.w, \
                                 widget->act_area.h );
    }

    if( button->active_srf->srf->w!=widget->act_area.w ||
        button->active_srf->srf->h!=widget->act_area.h )
    {
        MySDL_CreateRGBSurface(  button->active_srf,\
                                 widget->act_area.w, \
                                 widget->act_area.h );
    }
    bdcolor2 = SDL_MapRGBA( button->active_srf->srf->format, \
                            255, 255, 255, 200 );
    MySDL_FillRect( button->active_srf, NULL, bdcolor2 );
    button->active_cflag = 1;
}


static void * Button_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Button * button=widget->container->bin->button;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;
    MySDL_Surface * srf=MySDL_Surface_new ("Button_DrawBlit_srf");

    PROT__bin_DrawBlit( button->bin );

    if(button->togglebutton) {
        PROT__togglebutton_DrawBlit (button->togglebutton);
        ButtonCache( widget );
        return 0;
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
    MySDL_Surface_free( srf );

    ButtonCache( widget );

    return (void *) NULL;
}


static void * Button_Realize( SDLGuiTK_Widget * widget, \
                              void * data, void * event )
{
    SDLGuiTK_Button * button=widget->container->bin->button;

    if( button->text_flag!=0 ) {
        Button_make_surface( button );
    }

    return (void *) NULL;
}

static void * Button_Show( SDLGuiTK_Widget * widget, \
                           void * data, void * event )
{
    widget->shown = 1;
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }

    return (void *) NULL;
}

static void * Button_Hide( SDLGuiTK_Widget * widget, \
                           void * data, void * event )
{
    widget->shown = 0;
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }

    return (void *) NULL;
}

static SDLGuiTK_Widget * Button_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{

    return NULL;
}

static void * Button_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * child;

    child = SDLGuiTK_bin_get_child( widget->container->bin );
    PROT__bin_remove( widget->container->bin, child );

    SDLGuiTK_widget_destroy( child );

    return (void *) NULL;
}

static void * Button_Free( SDLGuiTK_Widget * widget )
{
    Button_destroy( widget->container->bin->button );

    return (void *) NULL;
}


static int Button_UpdateActive( SDLGuiTK_Widget * widget)
{
    SDLGuiTK_Button * button=widget->container->bin->button;

    if( widget->act_alpha>=0.15 ) {
        button->active_alpha_mod = -0.005;
    };
    if( widget->act_alpha<=0.10 ) {
        button->active_alpha_mod = 0.005;
    };
    widget->act_alpha+=button->active_alpha_mod;
    if( button->active_cflag==1 ) {
        button->active_cflag = 0;
        return 1;
    }
    return 0;
}


static void * Button_MouseEnter( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{
    SDLGuiTK_Button * button=widget->container->bin->button;

    widget->act_alpha = 0.15;
    button->active_alpha_mod = -0.005;
    ButtonCache( widget );
    widget->act_srf = button->active_srf;

    return (void *) NULL;
}

static void * Button_MouseLeave( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{
    SDLGuiTK_Button * button=widget->container->bin->button;

    widget->act_srf = NULL;
    widget->act_alpha = 0.15;
    button->active_alpha_mod = -0.005;
    button->pressed_flag = 0;

    return (void *) NULL;
}

static void * Button_MousePressed( SDLGuiTK_Widget * widget, \
                                   void * data, void * event )
{
    SDLGuiTK_Button * button=widget->container->bin->button;

    button->pressed_flag = 1;

    return (void *) NULL;
}

static void * Button_MouseReleased( SDLGuiTK_Widget * widget, \
                                    void * data, void * event )
{
    SDLGuiTK_Button * button=widget->container->bin->button;

    if( button->pressed_flag==1 ) {
        button->pressed_flag = 0;
        PROT__signal_push( widget->object, SDLGUITK_SIGNAL_TYPE_CLICKED );
    }
    if(button->togglebutton)
        PROT__togglebutton_clicked (button->togglebutton);
    return (void *) NULL;
}

static void Button_set_functions( SDLGuiTK_Button * button )
{
    SDLGuiTK_SignalHandler * handler;

    button->object->widget->RecursiveEntering = Button_RecursiveEntering;
    button->object->widget->RecursiveDestroy = Button_RecursiveDestroy;
    button->object->widget->UpdateActive = Button_UpdateActive;
    button->object->widget->Free = Button_Free;

    button->object->widget->DrawUpdate = Button_DrawUpdate;
    button->object->widget->DrawBlit = Button_DrawBlit;

    handler = (SDLGuiTK_SignalHandler *) button->object->signalhandler;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
            Button_Realize;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
            Button_Show;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
            Button_Hide;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_ENTER]->function = \
            Button_MouseEnter;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_LEAVE]->function = \
            Button_MouseLeave;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_PRESSED]->function = \
            Button_MousePressed;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_RELEASED]->function = \
            Button_MouseReleased;
}



SDLGuiTK_Widget * SDLGuiTK_button_new()
{
    SDLGuiTK_Button * button;

    button = Button_create();
    Button_set_functions( button );
    Button_make_surface( button );
    PROT__signal_push( button->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return button->object->widget;
}

SDLGuiTK_Widget * SDLGuiTK_button_new_with_label( char * text )
{
    SDLGuiTK_Button * button;
    SDLGuiTK_Widget * label;

    button = Button_create();
    strcpy( button->text, text );
    Button_set_functions( button );
    Button_make_surface( button );

    label = SDLGuiTK_label_new( text );
    SDLGuiTK_misc_set_padding( label->misc, 3, 5);
    SDLGuiTK_container_add( SDLGuiTK_CONTAINER(button->object->widget), label );
    SDLGuiTK_widget_show( label );
    PROT__signal_push( button->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return button->object->widget;
}


SDLGuiTK_Button * PROT__button_new_from_togglebutton(
                                    SDLGuiTK_ToggleButton * togglebutton)
{
    SDLGuiTK_Button * button;

    button = Button_create();
    button->togglebutton = togglebutton;
    Button_set_functions( button );
    Button_make_surface( button );

    return button;
}

void PROT__button_destroy(SDLGuiTK_Button * button)
{
    Button_destroy (button);
}

