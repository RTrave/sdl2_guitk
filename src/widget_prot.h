/*
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

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

#pragma once

#include <SDL2/SDL_guitk.h>
#include "mysdl.h"

/* SDLGuiTK_Widget structure definition */
struct SDLGuiTK_Widget {
    SDLGuiTK_Object * object;              /* referent object and herits from */

    /* herited */
    SDLGuiTK_Misc         * misc;
    SDLGuiTK_Container    * container;
    SDLGuiTK_Entry        * entry;
    SDLGuiTK_MenuShell    * menushell;
    SDLGuiTK_Scrollbar    * scrollbar;
    SDLGuiTK_ProgressBar  * progressbar;

    /* "public" data */
    SDL_bool              visible;
    SDL_bool              can_focus;
    SDL_bool              has_focus;
    //SDL_bool              can_default;
    SDL_bool              has_default;
    SDL_bool              has_tooltip;
    char                * tooltip_text;
    SDL_bool              focus_on_click;
    int activable_child;

    /* "private" data */
    int    id;
    SDLGuiTK_Widget * parent;
    SDLGuiTK_Widget * top;
    //SDLGuiTK_TooltipsData * tooltipsdata;
    int  width_request;
    int  height_request;

    /* "public" methods */
    SDLGuiTK_Widget * (*RecursiveEntering)( SDLGuiTK_Widget *, int x, int y );
    void * (*RecursiveDestroy)( SDLGuiTK_Widget * );
    void * (*Free)( SDLGuiTK_Widget * );
    void * (*DrawUpdate)( SDLGuiTK_Widget * );
    void * (*DrawBlit  )( SDLGuiTK_Widget * );
    int    (*UpdateActive  )( SDLGuiTK_Widget * );

    /* "private" data */
    SDL_Rect        req_area;        /* (w,h) set in DrawUpdate() of herited */
    SDL_Rect        abs_area;        /* (w,h) set in self DrawUpdate() */
    SDL_Rect        rel_area;        /* (w,h) set in self DrawUpdate() */
    SDL_Rect        act_area;
    MySDL_Surface * srf;
    // Active surface elements
    SDL_Rect        active_area;
    MySDL_Surface * active_srf;
    float           active_alpha;
};


void PROT__widget_init();
void PROT__widget_uninit();


/* Create and destroy structure */
SDLGuiTK_Widget *PROT__widget_new();
void PROT__widget_destroy( SDLGuiTK_Widget *widget );
void PROT__widget_set_top( SDLGuiTK_Widget *widget, SDLGuiTK_Widget *top);

/* Draw the Widget on his top surface */
void PROT__widget_DrawUpdate( SDLGuiTK_Widget *widget );
void PROT__widget_DrawBlit(   SDLGuiTK_Widget *widget );

/* Reset and Update req_area dimensions, only if larger values */
void PROT__widget_reset_req_area( SDLGuiTK_Widget *widget );
void PROT__widget_set_req_area( SDLGuiTK_Widget *widget,
                                int req_w, int req_h );

/* Entering/Leaving recursive procedure
 * returns the new focused widget, or NULL if none*/
SDLGuiTK_Widget * PROT__widget_is_entering( SDLGuiTK_Widget * widget,
                                            int x, int y );

/* Destroy REALLY structure */
void PROT__widget_destroypending();

