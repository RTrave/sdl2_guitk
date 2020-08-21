 /*
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

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
#include "../mysdl.h"
#include "../myttf.h"
#include "../mycursor.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../signal.h"
//#include "../mywm.h"
#include "../render/surface2d.h"
#include "../wmwidget.h"
#include "../render/mywm.h"
#include "../theme_prot.h"
#include "../context_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"
#include "../scrollbar_prot.h"
#include "../viewport_prot.h"
#include "scrolledwindow_prot.h"

static SDL_bool SDLGuiTK_IS_SCROLLEDWINDOW( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->scrolledwindow!=NULL ) {
                    return SDL_TRUE;
                }
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_SCROLLEDWINDOW(): widget is not a scrolledwindow\n" );
    return SDL_FALSE;
}

SDLGuiTK_ScrolledWindow * SDLGuiTK_SCROLLEDWINDOW( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_SCROLLEDWINDOW(widget) ) return NULL;
    return widget->container->bin->scrolledwindow;
}


static int current_id=0;

static SDLGuiTK_ScrolledWindow * ScrolledWindow_create()
{
    SDLGuiTK_ScrolledWindow * new_scrolledwindow;

    new_scrolledwindow = malloc( sizeof( struct SDLGuiTK_ScrolledWindow ) );
    new_scrolledwindow->bin = PROT__bin_new();
    new_scrolledwindow->object = new_scrolledwindow->bin->object;
    new_scrolledwindow->bin->scrolledwindow = new_scrolledwindow;
    sprintf( new_scrolledwindow->object->name, "scrolledwindow%d", ++current_id );
    new_scrolledwindow->object->widget->top = NULL;
    new_scrolledwindow->object->widget->parent = NULL;

    new_scrolledwindow->hscrollbar = NULL;
    new_scrolledwindow->vscrollbar = NULL;

    new_scrolledwindow->srf = MySDL_Surface_new("ScrolledWindow_srf");
    new_scrolledwindow->area.x = 0;
    new_scrolledwindow->area.y = 0;
    new_scrolledwindow->area.w = 0;
    new_scrolledwindow->area.h = 0;

    return new_scrolledwindow;
}

static void ScrolledWindow_destroy( SDLGuiTK_ScrolledWindow * scrolledwindow )
{
    MySDL_Surface_free( scrolledwindow->srf );

    PROT__bin_destroy( scrolledwindow->bin );
    //MyWM_WMWidget_Delete( scrolledwindow->wm_widget );
    free( scrolledwindow );
}

static void ScrolledWindow_Scrollbars_DrawUpdate(SDLGuiTK_ScrolledWindow * scrolledwindow)
{
    scrolledwindow->hscrollbar->parent = scrolledwindow->object->widget;
    scrolledwindow->hscrollbar->top = scrolledwindow->object->widget->top;
    (*(scrolledwindow->hscrollbar)->DrawUpdate)(scrolledwindow->hscrollbar);
    scrolledwindow->vscrollbar->parent = scrolledwindow->object->widget;
    scrolledwindow->vscrollbar->top = scrolledwindow->object->widget->top;
    (*scrolledwindow->vscrollbar->DrawUpdate)(scrolledwindow->vscrollbar);
}

static void ScrolledWindow_Scrollbars_DrawBlit(SDLGuiTK_ScrolledWindow * scrolledwindow)
{
    SDLGuiTK_Widget *widget = scrolledwindow->object->widget;
    SDLGuiTK_Widget *hscrollbar = scrolledwindow->hscrollbar;
    SDLGuiTK_Widget *vscrollbar = scrolledwindow->vscrollbar;
    SDL_Rect *harea = &scrolledwindow->hsb_area;
    SDL_Rect *varea = &scrolledwindow->vsb_area;

    hscrollbar->abs_area.x = harea->x + widget->abs_area.x;
    hscrollbar->abs_area.y = harea->y + widget->abs_area.y;
    hscrollbar->rel_area.x = harea->x;
    hscrollbar->rel_area.y = harea->y;
    PROT__widget_set_req_area(hscrollbar,
                              harea->w,
                              harea->h);
    (*hscrollbar->DrawBlit)(hscrollbar);

    vscrollbar->abs_area.x = varea->x + widget->abs_area.x;
    vscrollbar->abs_area.y = varea->y + widget->abs_area.y;
    vscrollbar->rel_area.x = varea->x;
    vscrollbar->rel_area.y = varea->y;
    PROT__widget_set_req_area(vscrollbar,
                              varea->w,
                              varea->h);
    (*vscrollbar->DrawBlit)(vscrollbar);
}

static void * ScrolledWindow_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;

    PROT__widget_reset_req_area( widget );

    PROT__bin_DrawUpdate( scrolledwindow->bin );
    widget->req_area.w+=SCROLLBAR_SIZE;
    widget->req_area.h+=SCROLLBAR_SIZE;
    if(scrolledwindow->hscrollbar && scrolledwindow->vscrollbar)
        ScrolledWindow_Scrollbars_DrawUpdate(scrolledwindow);

    return (void *) NULL;
}

static void ScrolledWindow_SetAreas(SDLGuiTK_ScrolledWindow * scrolledwindow)
{
    SDLGuiTK_Container * container=scrolledwindow->bin->container;
/*
 * child->rel_area.w-=SCROLLBAR_SIZE; child->rel_area.h-=SCROLLBAR_SIZE;
    child->abs_area.w-=SCROLLBAR_SIZE; child->abs_area.h-=SCROLLBAR_SIZE;
    child->act_area.w-=SCROLLBAR_SIZE; child->act_area.h-=SCROLLBAR_SIZE;
    printf("SCROLLEDWINDOW: relw=%d relh=%d sbsize=%d\n",
           child->rel_area.w, child->rel_area.h, SCROLLBAR_SIZE);
*/
    scrolledwindow->area.x = container->children_area.x;
    scrolledwindow->area.y = container->children_area.y;
    scrolledwindow->area.w = container->children_area.w - SCROLLBAR_SIZE;
    scrolledwindow->area.h = container->children_area.h - SCROLLBAR_SIZE;
    scrolledwindow->hsb_area.x = container->children_area.x;
    scrolledwindow->hsb_area.y = container->children_area.h-SCROLLBAR_SIZE;
    scrolledwindow->hsb_area.w = container->children_area.w-SCROLLBAR_SIZE;
    scrolledwindow->hsb_area.h = SCROLLBAR_SIZE;
    scrolledwindow->vsb_area.x = container->children_area.w-SCROLLBAR_SIZE;
    scrolledwindow->vsb_area.y = container->children_area.y;
    scrolledwindow->vsb_area.w = SCROLLBAR_SIZE;
    scrolledwindow->vsb_area.h = container->children_area.h-SCROLLBAR_SIZE;

}

static void * ScrolledWindow_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;

    PROT__bin_DrawBlit( scrolledwindow->bin );

    if( scrolledwindow->bin->child!=NULL ) {

        if( scrolledwindow->bin->child->visible ) {
            ScrolledWindow_SetAreas(scrolledwindow);
            PROT_viewport_setsize (SDLGuiTK_VIEWPORT(scrolledwindow->bin->child),
                                   scrolledwindow->area.w, scrolledwindow->area.h);
            MySDL_BlitSurface(  scrolledwindow->bin->child->srf, &scrolledwindow->area, \
                                widget->srf, &scrolledwindow->bin->child->rel_area );

            ScrolledWindow_Scrollbars_DrawBlit(scrolledwindow);
            MySDL_BlitSurface(  scrolledwindow->hscrollbar->srf, NULL, \
                                widget->srf, &scrolledwindow->hsb_area );
            MySDL_BlitSurface(  scrolledwindow->vscrollbar->srf, NULL, \
                                widget->srf, &scrolledwindow->vscrollbar->rel_area );
        }
    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;
    }

    return (void *) NULL;
}


static SDLGuiTK_Widget * ScrolledWindow_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{
    SDLGuiTK_ScrolledWindow * scrolledwindow=widget->container->bin->scrolledwindow;
    SDLGuiTK_Widget * active;
    if(widget->container->bin->child==NULL)
        return NULL;
    active = PROT__widget_is_entering( scrolledwindow->hscrollbar, x, y );
    if(active!=NULL)
        return active;
    active = PROT__widget_is_entering( scrolledwindow->vscrollbar, x, y );
    if(active!=NULL)
        return active;
    active = PROT__widget_is_entering( scrolledwindow->bin->child, x, y );
    if(active!=NULL)
        return active;
    return NULL;
}

static void * ScrolledWindow_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * child;

    child = SDLGuiTK_bin_get_child( widget->container->bin );
    PROT__bin_remove( widget->container->bin, child );

    SDLGuiTK_widget_destroy( child );

    return (void *) NULL;
}

static void * ScrolledWindow_Free( SDLGuiTK_Widget * widget )
{
    ScrolledWindow_destroy( widget->container->bin->scrolledwindow );

    return (void *) NULL;
}


static void ScrolledWindow_set_functions( SDLGuiTK_ScrolledWindow * scrolledwindow )
{
    scrolledwindow->object->widget->RecursiveEntering = ScrolledWindow_RecursiveEntering;
    scrolledwindow->object->widget->RecursiveDestroy = ScrolledWindow_RecursiveDestroy;
    scrolledwindow->object->widget->Free = ScrolledWindow_Free;

    scrolledwindow->object->widget->DrawUpdate = ScrolledWindow_DrawUpdate;
    scrolledwindow->object->widget->DrawBlit = ScrolledWindow_DrawBlit;
}

SDLGuiTK_Widget * SDLGuiTK_scrolled_window_new()
{
    SDLGuiTK_ScrolledWindow * scrolledwindow;

    scrolledwindow = ScrolledWindow_create();
    ScrolledWindow_set_functions( scrolledwindow );
    PROT__signal_push( scrolledwindow->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return scrolledwindow->object->widget;
}

SDLGuiTK_Widget * PROT__scrolledwindow_add(SDLGuiTK_ScrolledWindow *scrolledwindow,
                                           SDLGuiTK_Widget *widget )
{
    SDLGuiTK_Widget *viewport;
    if(SDLGuiTK_VIEWPORT (widget))
    {
        viewport = widget;
    }
    else
    {
        SDLGUITK_LOG ("Viewport created for ScrolledWindow\n");
        viewport = SDLGuiTK_viewport_new(NULL, NULL);
        SDLGuiTK_widget_show( viewport );
        SDLGuiTK_container_add(SDLGuiTK_CONTAINER(viewport), widget );
    }
    scrolledwindow->hscrollbar = SDLGuiTK_scrollbar_new (SDLGUITK_ORIENTATION_HORIZONTAL,
                                                         SDLGuiTK_viewport_get_hadjustment(SDLGuiTK_VIEWPORT(viewport)));
    scrolledwindow->vscrollbar = SDLGuiTK_scrollbar_new (SDLGUITK_ORIENTATION_VERTICAL,
                                                         SDLGuiTK_viewport_get_vadjustment(SDLGuiTK_VIEWPORT(viewport)));
    SDLGuiTK_widget_show (scrolledwindow->hscrollbar);
    SDLGuiTK_widget_show (scrolledwindow->vscrollbar);

    return viewport;
}

