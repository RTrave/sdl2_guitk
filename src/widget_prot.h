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

    SDLGuiTK_Misc       * misc;                      /* herited */
    SDLGuiTK_Container  * container;                 /* herited */
    SDLGuiTK_Entry      * entry;                     /* herited */
    SDLGuiTK_Editable   * editable;                  /* herited */
    SDLGuiTK_MenuShell  * menushell;                 /* herited */
    SDLGuiTK_Scrollbar  * scrollbar;                 /* herited */


    int  shown;                                      /* "public" data */
    int  activable, activable_child;                 /* "public" data */
    int  enter;                                      /* "public" data */
    SDLGuiTK_Widget * parent;                        /* "public" data */
    SDLGuiTK_Widget * top;                           /* "public" data */
    SDLGuiTK_TooltipsData * tooltipsdata;            /* "public" data */
    int  width_request;                              /* "public" data */
    int  height_request;                             /* "public" data */
    /*   SDLGuiTK_Requisition requisition;                /\* "public" data *\/ */


    int    id;                                       /* "private" data */
    SDLGuiTK_Widget * (*RecursiveEntering)( SDLGuiTK_Widget *, \
                                            int x, int y );  /* "private" data */
    void * (*RecursiveDestroy)( SDLGuiTK_Widget * ); /* "private" data */
    void * (*Free)( SDLGuiTK_Widget * );             /* "private" data */
    void * (*DrawUpdate)( SDLGuiTK_Widget * );       /* "private" data */
    void * (*DrawBlit  )( SDLGuiTK_Widget * );       /* "private" data */
    int    (*UpdateActive  )( SDLGuiTK_Widget * );   /* "private" data */

    /* "private" data */
    SDL_Rect      req_area;        /* (w,h) set in DrawUpdate() of herited */
    SDL_Rect      abs_area;        /* (w,h) set in self DrawUpdate() */
    SDL_Rect      rel_area;        /* (w,h) set in self DrawUpdate() */
    SDL_Rect      act_area;        /* "private" data */
    MySDL_Surface * srf;             /* "private" data */
    MySDL_Surface * act_srf;         /* "private" data */
    float         act_alpha;       /* "private" data */

    /*   int           srf_create;                        /\* "private" data *\/ */
    /*   SDL_Rect      WM_area;                           /\* "private" data *\/ */
    /*   SDL_Surface * WM_srf;                            /\* "private" data *\/ */
    /*   int  hided_parent;                               /\* "public" data *\/ */
    /*   int    changed;                                  /\* "private" data *\/ */
};


extern void PROT__widget_init();
extern void PROT__widget_uninit();


/* Create and destroy structure */
extern
SDLGuiTK_Widget *PROT__widget_new();
extern
void             PROT__widget_destroy( SDLGuiTK_Widget *widget );

/* Draw the Widget on his top surface */
extern
void PROT__widget_DrawUpdate( SDLGuiTK_Widget *widget );
extern
void PROT__widget_DrawBlit(   SDLGuiTK_Widget *widget );

/* Reset and Update req_area dimensions, only if larger values */
extern
void PROT__widget_reset_req_area( SDLGuiTK_Widget *widget );
extern
void PROT__widget_set_req_area( SDLGuiTK_Widget *widget, int req_w, int req_h );

/* Entering/Exiting recursive procedure */
extern
SDLGuiTK_Widget * PROT__widget_is_entering( SDLGuiTK_Widget * widget, \
        int x, int y );

/* Destroy REALLY structure */
extern
void PROT__widget_destroypending();

