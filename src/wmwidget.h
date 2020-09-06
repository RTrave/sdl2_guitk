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

#pragma once

#include <SDL2/SDL_guitk.h>
#include "mysdl.h"
#include "render/render.h"
#include "render/surface2d.h"

typedef struct SDLGuiTK_WMWidget SDLGuiTK_WMWidget;

/* SDLGuiTK_WMWidget structure definition */
struct SDLGuiTK_WMWidget {
    SDLGuiTK_Object    * object;        /* referent widget */

    SDLGuiTK_Widget * widget;           /* herits from */

    SDLGuiTK_List     * children;
    SDL_bool            is_wmchild;     /* FALSE if top, TRUE if have a parent */
    SDLGuiTK_Widget   * parent;
    SDLGuiTK_WMWidget * wmparent;

    /* "public" data */
    SDL_bool            enter;
    SDL_bool            moving;
    int                 border_width;
    SDL_Rect            area;
    SDLGuiTK_Render   * render;         /* Render associated if MULTI mode */

    /* "private" data */
    MySDL_Surface       * srf;
    SDL_Rect              child_area;    /* (w,h) setted in DrawUpdate() window,menushell,..
                                            * (x,y) setted in self DrawUpdate*/
    SDL_bool              title_shown;
    char                  title[256];
    MySDL_Surface       * title_srf;
    SDL_Rect              title_area;
    SDLGuiTK_Surface2D  * surface2D;
    SDL_bool              surface2D_flag;
};


extern SDLGuiTK_WMWidget  * WMWidget_New( SDLGuiTK_Widget * widget );
extern void                 WMWidget_Delete( SDLGuiTK_WMWidget * wm_widget );

// Get the render associated with, in MULTIPLES mode
SDLGuiTK_Render * WMWidget_getrender( SDLGuiTK_WMWidget * wm_widget );
// Get the window associated with
SDL_Window * WMWidget_getwindow( SDLGuiTK_WMWidget * wm_widget );

// Fill surface with bgcolor
extern void                 WMWidget_Clean( SDLGuiTK_WMWidget * wm_widget );
// child_area size and area coords & size
extern void                 WMWidget_DrawUpdate( SDLGuiTK_WMWidget * wm_widget );
// Blit widget srf in child_area
extern void                 WMWidget_DrawBlit( SDLGuiTK_WMWidget * wm_widget,
        MySDL_Surface * surface);

// Set title
void WMWidget_set_title( SDLGuiTK_WMWidget * wm_widget,\
                         const char *title );

// Is mouse entered in WMWidget or its childs
SDLGuiTK_WMWidget * WMWidget_is_entered( SDLGuiTK_WMWidget * wm_widget,
                                         int x, int y);
