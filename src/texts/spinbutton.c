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
#include "../myttf.h"
#include "../mycursor.h"
#include "../signal.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../theme_prot.h"
#include "../render/mywm.h"
#include "../context_prot.h"
#include "../adjustment_prot.h"

#include "editable_prot.h"
#include "entry_prot.h"
#include "spinbutton_prot.h"


SDL_bool SDLGuiTK_IS_SPINBUTTON( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->entry!=NULL ) {
            if( widget->entry->spinbutton!=NULL ) {
                return SDL_TRUE;
            }
        }
    }
    return SDL_FALSE;
}

SDLGuiTK_SpinButton * SDLGuiTK_SPINBUTTON( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_SPINBUTTON(widget) ) {
        SDLGUITK_ERROR( "SDLGuiTK_IS_SPINBUTTON(): widget is not an spinbutton\n" );
        return NULL;
    }
    return widget->entry->spinbutton;
}


static int current_id=0;

static SDLGuiTK_SpinButton * SpinButton_create()
{
    SDLGuiTK_SpinButton * new_spinbutton;

    new_spinbutton = malloc( sizeof( struct SDLGuiTK_SpinButton ) );
    new_spinbutton->entry = PROT__entry_new_from_spinbutton (new_spinbutton);
    new_spinbutton->object = new_spinbutton->entry->object;
    sprintf( new_spinbutton->object->name, "spinbutton%d", ++current_id );

    new_spinbutton->value = 0;
    new_spinbutton->climb_rate = 0;
    new_spinbutton->digits = 0;
    new_spinbutton->adjustment = NULL;

    new_spinbutton->mouseOn = SDL_FALSE;
    new_spinbutton->controlD_srf = MySDL_Surface_new ("SpinButtonDOWN_srf");
    new_spinbutton->controlD_flag = SDL_FALSE;
    new_spinbutton->controlU_srf = MySDL_Surface_new ("SpinButtonUP_srf");
    new_spinbutton->controlU_flag = SDL_FALSE;

    new_spinbutton->active_srf = MySDL_Surface_new ("Button_active_srf");
    new_spinbutton->active_cflag = 0;
    new_spinbutton->active_alpha_mod = -0.005;

    return new_spinbutton;
}

static void SpinButton_destroy( SDLGuiTK_SpinButton * spinbutton )
{
    PROT__adjustment_detach (spinbutton->adjustment, spinbutton->object->widget);
    MySDL_Surface_free( spinbutton->active_srf );
    MySDL_Surface_free( spinbutton->controlD_srf );
    MySDL_Surface_free( spinbutton->controlU_srf );

    // Button destroy himself and call ToggleButton_destroy
    //PROT__button_destroy( spinbutton->button );
    free( spinbutton );
}


static void SpinButton_Controls(SDLGuiTK_SpinButton * spinbutton)
{
    SDLGuiTK_Theme * theme;

    theme = PROT__theme_get_and_lock();
    spinbutton->controlD_srf->srf =
            MyTTF_Render_Blended (spinbutton->controlD_srf->srf, "▽", 20, 0,
                                  theme->ftcolor);
    spinbutton->controlD_area.w = spinbutton->controlD_srf->srf->w;
    spinbutton->controlD_area.h = spinbutton->controlD_srf->srf->h;
    spinbutton->controlU_srf->srf =
            MyTTF_Render_Blended (spinbutton->controlU_srf->srf, "△", 20, 0,
                                  theme->ftcolor);
    spinbutton->controlU_area.w = spinbutton->controlU_srf->srf->w;
    spinbutton->controlU_area.h = spinbutton->controlU_srf->srf->h;
    PROT__theme_unlock( theme );

}

static void SpinButton_MakeActiveSrf(SDLGuiTK_SpinButton * spinbutton)
{
    Uint32 bdcolor2;
    MySDL_CreateRGBSurface(  spinbutton->active_srf,
                             spinbutton->controlD_area.w+4,
                             24 );
    bdcolor2 = SDL_MapRGBA( spinbutton->active_srf->srf->format,
                            255, 255, 255, 255 );
    MySDL_FillRect( spinbutton->active_srf, NULL, bdcolor2 );
}

static int SpinButton_UpdateActive( SDLGuiTK_Widget * widget)
{
    SDLGuiTK_SpinButton * spinbutton=widget->entry->spinbutton;

    if( widget->active_alpha>=0.15 ) {
        spinbutton->active_alpha_mod = -0.005;
    };
    if( widget->active_alpha<=0.10 ) {
        spinbutton->active_alpha_mod = 0.005;
    };
    widget->active_alpha+=spinbutton->active_alpha_mod;
    if( spinbutton->active_cflag==1 ) {
        SpinButton_MakeActiveSrf(spinbutton);
        spinbutton->active_cflag = 0;
        return 1;
    }
    return 0;
}

void PROT__spinbutton_DrawUpdate(SDLGuiTK_SpinButton * spinbutton)
{
    SDLGuiTK_Widget * widget = spinbutton->object->widget;

    SpinButton_Controls( spinbutton );
    PROT__widget_set_req_area(widget, 120, spinbutton->controlD_srf->srf->h+4);
}

void PROT__spinbutton_DrawBlit(SDLGuiTK_SpinButton * spinbutton)
{
    SDLGuiTK_Widget * widget = spinbutton->object->widget;
    SDLGuiTK_Entry * entry=spinbutton->entry;
    int wdiff=0, hdiff=0;
    float cursor_ratio=1;
    //int xdiff=0, ydiff=0;
    SDL_Rect fg_area;
    SDLGuiTK_Editable * editable=entry->editable;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;

    fg_area.x = 2;
    fg_area.y = 2;
    fg_area.w = widget->abs_area.w-4;
    fg_area.h = widget->abs_area.h-4;


    theme = PROT__theme_get_and_lock();
#if DEBUG_LEVEL >= 3
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, 0x00, 0xff, 0x00, 0xff );
#else
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->bgcolor.r, \
                           theme->bgcolor.g, \
                           theme->bgcolor.b, \
                           255 );
#endif
    PROT__theme_unlock( theme );
    MySDL_FillRect( widget->srf, &fg_area, bgcolor );

    // Draw controls
    int hdiffc = (widget->abs_area.h/2)-10;
    spinbutton->controlD_area.x = widget->abs_area.w-42;
    spinbutton->controlD_area.y = hdiffc;
    MySDL_BlitSurface(  spinbutton->controlD_srf, NULL,
                        widget->srf, &spinbutton->controlD_area );
    spinbutton->controlU_area.x = widget->abs_area.w-21;
    spinbutton->controlU_area.y = hdiffc;
    MySDL_BlitSurface(  spinbutton->controlU_srf, NULL,
                        widget->srf, &spinbutton->controlU_area );


    wdiff = entry->srf->srf->w - widget->abs_area.w +4 +56;
    hdiff = entry->srf->srf->h - widget->abs_area.h +4;
    if(hdiff<0)
        fg_area.y -= hdiff/2;
    fg_area.h = entry->srf->srf->h;
    if(wdiff>0) {
        cursor_ratio = ((float)editable->cursor_position /
                        (float)unicode_strlen(editable->text,-1));
        entry->view_area.x = wdiff*cursor_ratio;
        entry->view_area.y = 0;
        entry->view_area.w = widget->abs_area.w-4-56;
        entry->view_area.h = entry->srf->srf->h;
        MySDL_BlitSurface(  entry->srf, &entry->view_area, \
                            widget->srf, &fg_area );
    } else {
        MySDL_BlitSurface(  entry->srf, NULL, \
                            widget->srf, &fg_area );
    }

}

static void SpinButton_ControlDOWN(SDLGuiTK_SpinButton * spinbutton)
{
    SDLGuiTK_Widget * widget=spinbutton->object->widget;

    widget->active_alpha = 0.15;
    spinbutton->active_alpha_mod = -0.005;
    widget->active_srf = spinbutton->active_srf;
    int hdiffc = (widget->abs_area.h/2)-10;
    widget->active_area.x = widget->abs_area.x + spinbutton->controlD_area.x -2;
    widget->active_area.y = widget->abs_area.y+hdiffc-3;
    widget->active_area.w = 24; //spinbutton->controlD_area.w;
    widget->active_area.h = 24; //spinbutton->controlD_area.h;
    spinbutton->active_cflag = 1;
}

static void SpinButton_ControlUP(SDLGuiTK_SpinButton * spinbutton)
{
    SDLGuiTK_Widget * widget=spinbutton->object->widget;

    widget->active_alpha = 0.15;
    spinbutton->active_alpha_mod = -0.005;
    widget->active_srf = spinbutton->active_srf;
    int hdiffc = (widget->abs_area.h/2)-10;
    widget->active_area.x = widget->abs_area.x + spinbutton->controlU_area.x-2;
    widget->active_area.y = widget->abs_area.y + hdiffc-3;
    widget->active_area.w = 24;//spinbutton->controlU_area.w;
    widget->active_area.h = 24;//spinbutton->controlU_area.h;
    spinbutton->active_cflag = 1;
}

static void SpinButton_ControlLeave(SDLGuiTK_SpinButton * spinbutton)
{
    SDLGuiTK_Widget * widget=spinbutton->object->widget;

    widget->active_srf = NULL;
    widget->active_alpha = 0.15;
    spinbutton->active_alpha_mod = -0.005;
}

void PROT__spinbutton_RecursiveEntering(SDLGuiTK_SpinButton * spinbutton,
                                        int x, int y)
{
    SDLGuiTK_Widget * widget = spinbutton->object->widget;

    if(!spinbutton->mouseOn) return;
    int hdiffc = (widget->abs_area.h/2)-10;
    if( x>(widget->abs_area.x+spinbutton->controlD_area.x) &&
        x<(widget->abs_area.x+spinbutton->controlD_area.x+spinbutton->controlD_area.w) &&
        y>(widget->abs_area.y+hdiffc) &&
        y<(widget->abs_area.y+hdiffc+spinbutton->controlD_area.h) )
        {
            if(!spinbutton->controlD_flag) {
                spinbutton->controlD_flag = SDL_TRUE;
                MyCursor_Set( SDLGUITK_CURSOR_DEFAULT );
                SpinButton_ControlDOWN (spinbutton);
            }
        } else {
            if(spinbutton->controlD_flag) {
                spinbutton->controlD_flag = SDL_FALSE;
                MyCursor_Set( SDLGUITK_CURSOR_TEXT );
                SpinButton_ControlLeave(spinbutton);
            }
        }
    if( x>(widget->abs_area.x+spinbutton->controlU_area.x) &&
        x<(widget->abs_area.x+spinbutton->controlU_area.x+spinbutton->controlU_area.w) &&
        y>(widget->abs_area.y+hdiffc) &&
        y<(widget->abs_area.y+hdiffc+spinbutton->controlU_area.h) )
        {
            if(!spinbutton->controlU_flag) {
                spinbutton->controlU_flag = SDL_TRUE;
                MyCursor_Set( SDLGUITK_CURSOR_DEFAULT );
                SpinButton_ControlUP (spinbutton);
            }
        } else {
            if(spinbutton->controlU_flag) {
                spinbutton->controlU_flag = SDL_FALSE;
                MyCursor_Set( SDLGUITK_CURSOR_TEXT );
                SpinButton_ControlLeave(spinbutton);
            }
        }
}

void PROT__spinbutton_enter(SDLGuiTK_SpinButton * spinbutton)
{
    if(!spinbutton->mouseOn) {
        spinbutton->mouseOn = SDL_TRUE;
    }
}

void PROT__spinbutton_leave(SDLGuiTK_SpinButton * spinbutton)
{
    if(spinbutton->mouseOn) {
        spinbutton->mouseOn = SDL_FALSE;
    }
    double value = atof(SDLGuiTK_entry_get_text( spinbutton->object->widget));
    SDLGuiTK_adjustment_set_value (spinbutton->adjustment, value);
}

void PROT__spinbutton_pressed(SDLGuiTK_SpinButton * spinbutton)
{
    double value = spinbutton->adjustment->value;
    if(spinbutton->controlD_flag) {
        value -= spinbutton->adjustment->step_increment;
    }
    else if(spinbutton->controlU_flag) {
        value += spinbutton->adjustment->step_increment;
    }
    else return;
    SDLGuiTK_adjustment_set_value (spinbutton->adjustment, value);
}

void PROT__spinbutton_released(SDLGuiTK_SpinButton * spinbutton)
{

}

void PROT__spinbutton_clicked(SDLGuiTK_SpinButton * spinbutton)
{
}

void PROT__spinbutton_destroy(SDLGuiTK_SpinButton * spinbutton)
{
    SpinButton_destroy( spinbutton );
}

SDL_bool PROT__spinbutton_inputtext_verif(SDLGuiTK_SpinButton * spinbutton,
                                          char *text)
{
    if( !strcmp(text,"0") && !strcmp(text,"1") && !strcmp(text,"2") &&
        !strcmp(text,"3") && !strcmp(text,"4") && !strcmp(text,"5") &&
        !strcmp(text,"6") && !strcmp(text,"7") && !strcmp(text,"8") &&
        !strcmp(text,"9") && !strcmp(text,".") && !strcmp(text,",") )
        return SDL_FALSE;
    return SDL_TRUE;
}

SDL_bool PROT__spinbutton_keyboard(SDLGuiTK_SpinButton * spinbutton, SDL_Keysym *keysym)
{
    if (strcmp( SDL_GetKeyName( keysym->sym ), "Return")==0) {
        double value = atof(SDLGuiTK_entry_get_text( spinbutton->object->widget));
        SDLGuiTK_adjustment_set_value (spinbutton->adjustment, value);
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

const char *  PROT__spinbutton_set_text(SDLGuiTK_SpinButton * spinbutton, const char *str)
{
    char *text;
    text = calloc(1028, sizeof (char));
    char format[8];
    sprintf (format, "%%.%df", spinbutton->digits);
    sprintf (text, format, atof(str));
    return text;
}

static void * SpinButton_ChildNotify( SDLGuiTK_Signal * signal, void * data )
{
    SDLGuiTK_SpinButton * spinbutton=signal->object->widget->entry->spinbutton;
    char text[256];
    char format[8];
    sprintf (format, "%%.%df", spinbutton->digits);
    sprintf (text, format, spinbutton->adjustment->value);
    SDLGuiTK_entry_set_text (spinbutton->object->widget, text);

    return (void *) NULL;
}


static void SpinButton_set_functions( SDLGuiTK_SpinButton * spinbutton )
{
    spinbutton->object->widget->UpdateActive = SpinButton_UpdateActive;

    PROT_signal_connect(spinbutton->object, SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY,
                        SpinButton_ChildNotify, SDLGUITK_SIGNAL_LEVEL2);
}

SDLGuiTK_Widget * SDLGuiTK_spin_button_new( SDLGuiTK_Adjustment *adjustment,
                                            double climb_rate,
                                            int digits )
{
    SDLGuiTK_SpinButton * spinbutton;

    spinbutton = SpinButton_create();
    SpinButton_set_functions( spinbutton );
    if(!adjustment)
        adjustment = SDLGuiTK_adjustment_new (0, 0, 100, 1);
    PROT__adjustment_attach (adjustment, spinbutton->object->widget);
    spinbutton->adjustment = adjustment;
    spinbutton->climb_rate = climb_rate;
    spinbutton->digits = digits;
    PROT__signal_push( spinbutton->object, SDLGUITK_SIGNAL_TYPE_REALIZE );
SpinButton_Controls(spinbutton);
    return spinbutton->object->widget;
}


