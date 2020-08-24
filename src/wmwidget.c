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

#include "debug.h"
#include "mysdl.h"
#include "signal.h"
#include "list.h"
#include "theme_prot.h"
#include "myttf.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "render/surface2d.h"
#include "wmwidget.h"
#include "context_prot.h"


SDLGuiTK_WMWidget * WMWidget_New( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_WMWidget * wm_widget;

    wm_widget = malloc( sizeof( struct SDLGuiTK_WMWidget ) );
    wm_widget->widget = widget;
    wm_widget->object = widget->object;

    wm_widget->children = SDLGuiTK_list_new ();
    wm_widget->render = NULL;
    wm_widget->is_wmchild = 0;
    wm_widget->parent = NULL;
    wm_widget->wmparent = NULL;

    wm_widget->border_width = 3;
    wm_widget->title_shown = 0;
    wm_widget->title_srf = MySDL_Surface_new("Window_title_srf");
    wm_widget->title_area.x = 0;
    wm_widget->title_area.y = 0;
    wm_widget->title_area.w = 0;
    wm_widget->title_area.h = 0;

    wm_widget->enter = 0;
    wm_widget->moving = 0;
    wm_widget->srf = MySDL_Surface_new ("WMWidget_srf");
    wm_widget->area.x = 0;
    wm_widget->area.y = 0;
    wm_widget->area.w = 0;
    wm_widget->area.h = 0;
    wm_widget->child_area.x = 0;
    wm_widget->child_area.y = 0;
    wm_widget->child_area.w = 0;
    wm_widget->child_area.h = 0;

    wm_widget->surface2D = NULL;
    wm_widget->surface2D_flag = 0;

    return wm_widget;
}

void              WMWidget_Delete( SDLGuiTK_WMWidget * wm_widget )
{
    SDLGuiTK_list_destroy (wm_widget->children);
    if( wm_widget->title_srf->srf!=NULL ) {
        SDL_FreeSurface (wm_widget->title_srf->srf);
        wm_widget->title_srf->srf = NULL;
    }
    MySDL_Surface_free( wm_widget->srf );
    free( wm_widget );
}


static void WMWidget_MakeTitleSurface( SDLGuiTK_WMWidget * wm_widget )
{
    SDLGuiTK_Theme * theme;

    if(wm_widget->title_srf->srf!=NULL) return;
    theme = PROT__theme_get_and_lock();
    wm_widget->title_srf->srf = MyTTF_Render_Blended( wm_widget->title_srf->srf, \
                                                   wm_widget->title, 18, \
                                                   MyTTF_STYLE_BOLD, \
                                                   theme->bgcolor );
    PROT__theme_unlock( theme );
}

static void WMWidget_DrawTitleSurface( SDLGuiTK_WMWidget * wm_widget )
{
    Uint32 bdcolor;
    SDLGuiTK_Theme * theme;

    theme = PROT__theme_get_and_lock();
    bdcolor = SDL_MapRGBA( wm_widget->title_srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           255 );
    PROT__theme_unlock( theme );

    wm_widget->title_area.x = 0;
    wm_widget->title_area.y = 0;
    wm_widget->title_area.w = wm_widget->srf->srf->w;
    wm_widget->title_area.h = wm_widget->title_srf->srf->h;
    MySDL_FillRect( wm_widget->srf, &wm_widget->title_area, bdcolor );

    wm_widget->title_area.x = ( wm_widget->srf->srf->w - wm_widget->title_srf->srf->w )/2;
    wm_widget->title_area.y = 0;
    wm_widget->title_area.w = wm_widget->title_srf->srf->w;
    wm_widget->title_area.h = wm_widget->title_srf->srf->h;


    MySDL_BlitSurface( wm_widget->title_srf, NULL, \
                       wm_widget->srf, &wm_widget->title_area );
}


SDLGuiTK_Render * WMWidget_getrender( SDLGuiTK_WMWidget * wm_widget )
{
    if(!wm_widget)
        return NULL;
    if(wm_widget->render)
        return wm_widget->render;
    if(wm_widget->is_wmchild && wm_widget->wmparent && wm_widget->wmparent->render)
        return wm_widget->wmparent->render;
    //printf("No render founded !!!\n");
    return NULL;
}

SDL_Window * WMWidget_getwindow( SDLGuiTK_WMWidget * wm_widget )
{
    SDLGuiTK_Render * render;
    if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE)
        render = WMWidget_getrender(wm_widget);
    else
        render = current_context->main_render;
    return Render_GetVideoWindow (render);
}

void WMWidget_DrawUpdate( SDLGuiTK_WMWidget * wm_widget )
{
    /*   SDLGuiTK_Widget * widget=wm_widget->widget; */

    wm_widget->child_area.x = wm_widget->border_width;
    wm_widget->child_area.y = wm_widget->border_width;

    wm_widget->area.x = wm_widget->widget->abs_area.x - wm_widget->border_width - wm_widget->widget->rel_area.x;
    wm_widget->area.y = wm_widget->widget->abs_area.y - wm_widget->border_width - wm_widget->widget->rel_area.y;
    wm_widget->area.w = wm_widget->child_area.w + 2*wm_widget->border_width;
    wm_widget->area.h = wm_widget->child_area.h + 2*wm_widget->border_width;

    if(wm_widget->title_shown && current_context->type!=SDLGUITK_CONTEXT_MODE_MULTIPLE) {
        WMWidget_MakeTitleSurface (wm_widget);
        wm_widget->child_area.y += wm_widget->title_srf->srf->h;
        wm_widget->area.y -= wm_widget->title_srf->srf->h;
        wm_widget->area.h += wm_widget->title_srf->srf->h;
    }
    SDLGuiTK_Render * render;
    if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE)
        render = WMWidget_getrender(wm_widget);
    else
        render = current_context->main_render;
    if( render && wm_widget->is_wmchild )
    {
        SDL_Window * mwindow = Render_GetVideoWindow(render);
        int w,h;
        SDL_GetWindowSize(mwindow,&w,&h);
        //printf("render: %d %d\n", w, h);
        if(wm_widget->area.x+wm_widget->area.w>w) {
            int xdiff1 = wm_widget->area.x + wm_widget->area.w - w;
            wm_widget->area.x -= xdiff1;
            wm_widget->widget->abs_area.x -= xdiff1;
        }
        if(wm_widget->area.x<0) {
            wm_widget->widget->abs_area.x -= wm_widget->area.x;
            wm_widget->area.x  = 0;
        }
        if(wm_widget->area.y+wm_widget->area.h>h) {
            int ydiff1 = wm_widget->area.y + wm_widget->area.h - h;
            wm_widget->area.y -= ydiff1;
            wm_widget->widget->abs_area.y -= ydiff1;
        }

    }
}

void WMWidget_Clean( SDLGuiTK_WMWidget * wm_widget )
{
    Uint32 bdcolor;
    SDLGuiTK_Theme * theme;

    MySDL_CreateRGBSurface(   wm_widget->srf,\
                              wm_widget->area.w, wm_widget->area.h );

    /* Load theme values */
    theme = PROT__theme_get_and_lock();
    bdcolor = SDL_MapRGBA( wm_widget->srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           255 );
    PROT__theme_unlock( theme );

    /* Blit border, backgroud and title */
    MySDL_FillRect( wm_widget->srf, NULL, bdcolor );
}


void                 WMWidget_DrawBlit( SDLGuiTK_WMWidget * wm_widget,
                                        MySDL_Surface * surface)
{
    WMWidget_Clean( wm_widget );
    if(wm_widget->title_shown && current_context->type!=SDLGUITK_CONTEXT_MODE_MULTIPLE) {
        WMWidget_DrawTitleSurface (wm_widget);
    }

    MySDL_BlitSurface( surface, NULL, \
                       wm_widget->srf, &wm_widget->child_area );
}


void WMWidget_set_title( SDLGuiTK_WMWidget * wm_widget,\
                         const char *title )
{
    strcpy( wm_widget->title, title );
    wm_widget->title_shown = 1;
}
