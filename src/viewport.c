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

#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "mysdl.h"
#include "myttf.h"
#include "mycursor.h"
#include "signal.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "theme_prot.h"
#include "container_prot.h"
#include "bin_prot.h"
#include "adjustment_prot.h"
//#include "mywm.h"
//#include "render/surface2d.h"
#include "wmwidget.h"
#include "context_prot.h"
#include "viewport_prot.h"


static SDL_bool SDLGuiTK_IS_VIEWPORT( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->viewport!=NULL ) {
                    return SDL_TRUE;
                }
            }
        }
    }
  SDLGUITK_ERROR( "SDLGuiTK_IS_VIEWPORT(): widget is not a viewport\n" );
  return SDL_FALSE;
}

SDLGuiTK_Viewport * SDLGuiTK_VIEWPORT( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_VIEWPORT(widget) ) return NULL;
    return widget->container->bin->viewport;
}


static int current_id=0;


static SDLGuiTK_Viewport * Viewport_create()
{
    SDLGuiTK_Viewport * new_viewport;

    new_viewport = malloc( sizeof( struct SDLGuiTK_Viewport ) );
    new_viewport->bin = PROT__bin_new();
    new_viewport->object = new_viewport->bin->object;
    new_viewport->bin->viewport = new_viewport;
    sprintf( new_viewport->object->name, "viewport%d", ++current_id );
    new_viewport->object->widget->top = new_viewport->object->widget;
    new_viewport->object->widget->parent = new_viewport->object->widget;

    new_viewport->object->widget->focus_on_click = SDL_TRUE;

    //new_viewport->srf = MySDL_Surface_new("Viewport_srf");
    new_viewport->area.x = 0;
    new_viewport->area.y = 0;
    new_viewport->area.w = 0;
    new_viewport->area.h = 0;
    new_viewport->force_width = 0;
    new_viewport->force_height = 0;
    new_viewport->is_active = 0;
    new_viewport->is_moving = SDL_FALSE;

    //new_viewport->wm_widget = WMWidget_New( new_viewport->object->widget );
    //new_viewport->wm_widget->border_width = 0;

    return new_viewport;
}

static void Viewport_destroy( SDLGuiTK_Viewport * viewport )
{
    //MySDL_Surface_free( viewport->srf );

    PROT__bin_destroy( viewport->bin );
    //WMWidget_Delete( viewport->wm_widget );
    free( viewport );
}


static void * Viewport_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Viewport * viewport=widget->container->bin->viewport;

    PROT__widget_reset_req_area( widget );

    PROT__bin_DrawUpdate( viewport->bin );

    widget->req_area.w = 0;
    widget->req_area.h = 0;

     /* Prepare WM coords */
    //widget->abs_area.x = viewport->object->widget->abs_area.x+3;
    //widget->abs_area.y = viewport->object->widget->abs_area.y+3;
    /* Prepare WM sizes */
    //viewport->wm_widget->child_area.w = widget->req_area.w; //TODO: abs_area ???
    //viewport->wm_widget->child_area.h = widget->req_area.h;
    //viewport->wm_widget->surface2D_flag = 1;
    //WMWidget_DrawUpdate( viewport->wm_widget );
    return (void *) NULL;
}

static double Adjustment_ratio(SDLGuiTK_Adjustment *adjustment)
{
    return ((adjustment->value-adjustment->lower)/(adjustment->upper-adjustment->lower));
}

static void * Viewport_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Viewport   * viewport=widget->container->bin->viewport;
    SDLGuiTK_Widget * child = viewport->bin->child;

    PROT__bin_DrawBlit( viewport->bin );

    viewport->area.w = widget->container->children_area.w;
    int diff_w = viewport->bin->child->rel_area.w - viewport->area.w;
    viewport->area.x = (int) diff_w*Adjustment_ratio(viewport->hadjustment);

    viewport->area.h = widget->container->children_area.h;
    int diff_h = viewport->bin->child->rel_area.h - viewport->area.h;
    viewport->area.y = (int) diff_h*Adjustment_ratio(viewport->vadjustment);

    MySDL_BlitSurface( viewport->bin->child->srf, &viewport->area, \
                        widget->srf, &viewport->bin->child->rel_area );

    //MySDL_CreateRGBSurface (viewport->srf,
    //                        viewport->area.w, viewport->area.h);
    //MySDL_BlitSurface( viewport->bin->child->srf, &viewport->area,
    //                    viewport->srf, &viewport->bin->child->rel_area );

    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    if(viewport->force_width)
        widget->act_area.w = viewport->force_width;
    else
        widget->act_area.w = widget->abs_area.w;
    if(viewport->force_height)
        widget->act_area.h = viewport->force_height;
    else
        widget->act_area.h = widget->abs_area.h;

    if(child->act_area.w>widget->act_area.w)
        child->act_area.w = widget->act_area.w;
    if(child->act_area.h>widget->act_area.h)
        child->act_area.h = widget->act_area.h;

    //WMWidget_DrawUpdate (viewport->wm_widget);
    //WMWidget_DrawBlit( viewport->wm_widget, viewport->srf );

    return (void *) NULL;
}

static void Viewport_Move(SDLGuiTK_Viewport *viewport, int x, int y)
{
    SDLGuiTK_Widget * widget=viewport->object->widget;
    int mdiff = 0;
    if(viewport->bin->child->req_area.w>widget->container->children_area.w)
        mdiff = x-viewport->mbutton_x;
    if(mdiff!=0) {
        double fraction = Adjustment_ratio (viewport->hadjustment) -
                (((double)mdiff) / ((double)viewport->bin->child->req_area.w-(double)widget->container->children_area.w));
        PROT__adjustment_set_fraction(viewport->hadjustment, fraction);
        //scrollbar->button_area.x += mdiff;
        //scrollbar->button_act_area.x += mdiff;
        viewport->mbutton_x += mdiff;
    }
    mdiff = 0;
    if(viewport->bin->child->req_area.h>widget->container->children_area.h)
        mdiff = y-viewport->mbutton_y;
    if(mdiff!=0) {
        double fraction = Adjustment_ratio (viewport->vadjustment) -
                (((double)mdiff) / ((double)viewport->bin->child->req_area.h-(double)widget->container->children_area.h));
        PROT__adjustment_set_fraction(viewport->vadjustment, fraction);
        //scrollbar->button_area.x += mdiff;
        //scrollbar->button_act_area.x += mdiff;
        viewport->mbutton_y += mdiff;
    }
}

static SDLGuiTK_Widget * Viewport_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{
    SDLGuiTK_Viewport *viewport = widget->container->bin->viewport;
    SDLGuiTK_Widget * child;
    SDLGuiTK_Widget * active;

    child = SDLGuiTK_bin_get_child( widget->container->bin );
    if( !child ) {
        SDLGUITK_ERROR( "Viewport_RecursiveEntering(): child==NULL\n" );
        return NULL;
    }
    if( !child->visible ) {
        return NULL;
    }
    if(viewport->is_moving) {
        Viewport_Move(viewport, x, y);
        return NULL;
    }
    if((x>=widget->act_area.x && x<(widget->act_area.x+widget->act_area.w)) &&
       (y>=widget->act_area.y && y<(widget->act_area.y+widget->act_area.h)))
    {
        viewport->is_active = 1;
        return active = PROT__widget_is_entering( child, x, y );
    } else if(viewport->is_active==1) {
        viewport->is_active = 0;
        //PROT__signal_push( child->object, SDLGUITK_SIGNAL_TYPE_LEAVE );
        //PROT_MyWM_leaveall ();
    }
    return NULL;
}

static void * Viewport_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * child;

    child = SDLGuiTK_bin_get_child( widget->container->bin );
    if( child!=NULL ) {
        SDLGuiTK_widget_destroy( child );
    }

    return (void *) NULL;
}

static void * Viewport_Free( SDLGuiTK_Widget * widget )
{
    Viewport_destroy( widget->container->bin->viewport );

    return (void *) NULL;
}


static void * Viewport_Realize( SDLGuiTK_Signal * signal, void * data )
{
    Viewport_DrawUpdate( signal->object->widget );
    Viewport_DrawBlit( signal->object->widget );

    return (void *) NULL;
}

static void * Viewport_Show( SDLGuiTK_Signal * signal, void * data )
{
    //SDLGuiTK_Viewport * viewport=widget->container->bin->viewport;

    //if( widget->shown==1 ) return (void *) NULL;
    //widget->shown = 1;
    //viewport->wm_widget->is_wmchild = 1;
    //viewport->wm_widget->parent = viewport->object->widget->top;
    //PROT__context_ref_wmwidget( viewport->wm_widget );

    return (void *) NULL;
}

static void * Viewport_Hide( SDLGuiTK_Signal * signal, void * data )
{
    //SDLGuiTK_Viewport * viewport=widget->container->bin->viewport;

    //if( widget->shown==0 ) return (void *) NULL;
    //widget->shown = 0;
    //PROT__context_unref_wmwidget( viewport->wm_widget );

    return (void *) NULL;
}

static void * Viewport_ChildNotify( SDLGuiTK_Signal * signal, void * data )
{
    Viewport_DrawUpdate( signal->object->widget );
    Viewport_DrawBlit( signal->object->widget );

    return (void *) NULL;
}


static void * Viewport_MouseEnter( SDLGuiTK_Signal * signal, void * data )
{

    return (void *) NULL;
}

static void * Viewport_MouseLeave( SDLGuiTK_Signal * signal, void * data )
{
    SDLGuiTK_Widget * widget=signal->object->widget;
    SDLGuiTK_Viewport * viewport=widget->container->bin->viewport;
    viewport->is_moving = SDL_FALSE;

    return (void *) NULL;
}

static void * Viewport_MousePressed( SDLGuiTK_Signal * signal, void * data )
{
    SDLGuiTK_Widget * widget=signal->object->widget;
    SDLGuiTK_Viewport * viewport=widget->container->bin->viewport;
    int x, y;
    if( SDL_GetMouseState( &x, &y) ) {
        printf( "Mouse pressed on viewport: x=%d y=%d\n", x, y );
        viewport->mbutton_x = x;
        viewport->mbutton_y = y;
        viewport->is_moving = SDL_TRUE;
        //window->moving = 1;
    }
    return (void *) NULL;
}

static void * Viewport_MouseReleased( SDLGuiTK_Signal * signal, void * data )
{
    SDLGuiTK_Widget * widget=signal->object->widget;
    SDLGuiTK_Viewport * viewport=widget->container->bin->viewport;
   int x, y;
    if( SDL_GetMouseState( &x, &y) ) {
        printf( "Mouse released on viewport: x=%d y=%d\n", x, y );
    }
    viewport->is_moving = SDL_FALSE;
    //Viewport_DrawUpdate( widget );
    //Viewport_DrawBlit( widget );

    return (void *) NULL;
}

static void Viewport_setsignals( SDLGuiTK_Viewport * viewport )
{
    viewport->object->widget->RecursiveEntering = Viewport_RecursiveEntering;
    viewport->object->widget->RecursiveDestroy = Viewport_RecursiveDestroy;
    viewport->object->widget->Free = Viewport_Free;

    viewport->object->widget->DrawUpdate = Viewport_DrawUpdate;
    viewport->object->widget->DrawBlit = Viewport_DrawBlit;

    PROT_signal_connect(viewport->object, SDLGUITK_SIGNAL_TYPE_REALIZE,
                        Viewport_Realize, SDLGUITK_SIGNAL_LEVEL2);

    PROT_signal_connect(viewport->object, SDLGUITK_SIGNAL_TYPE_SHOW,
                        Viewport_Show, SDLGUITK_SIGNAL_LEVEL2);

    PROT_signal_connect(viewport->object, SDLGUITK_SIGNAL_TYPE_HIDE,
                        Viewport_Hide, SDLGUITK_SIGNAL_LEVEL2);

    PROT_signal_connect(viewport->object, SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY,
                        Viewport_ChildNotify, SDLGUITK_SIGNAL_LEVEL2);

    PROT_signal_connect(viewport->object, SDLGUITK_SIGNAL_TYPE_ENTER,
                        Viewport_MouseEnter, SDLGUITK_SIGNAL_LEVEL2);

    PROT_signal_connect(viewport->object, SDLGUITK_SIGNAL_TYPE_LEAVE,
                        Viewport_MouseLeave, SDLGUITK_SIGNAL_LEVEL2);

    PROT_signal_connect(viewport->object, SDLGUITK_SIGNAL_TYPE_PRESSED,
                        Viewport_MousePressed, SDLGUITK_SIGNAL_LEVEL2);

    PROT_signal_connect(viewport->object, SDLGUITK_SIGNAL_TYPE_RELEASED,
                        Viewport_MouseReleased, SDLGUITK_SIGNAL_LEVEL2);
}

SDLGuiTK_Widget * SDLGuiTK_viewport_new(SDLGuiTK_Adjustment *hadjustment,
                                        SDLGuiTK_Adjustment *vadjustment)
{
    SDLGuiTK_Viewport * viewport;

    viewport = Viewport_create();
    if(hadjustment==NULL) {
        hadjustment = SDLGuiTK_adjustment_new(0, 0, 1, 0);
    }
    if(vadjustment==NULL) {
        vadjustment = SDLGuiTK_adjustment_new(0, 0, 1, 0);
    }
    viewport->hadjustment = hadjustment;
    PROT__adjustment_attach(hadjustment, viewport->object->widget);
    viewport->vadjustment = vadjustment;
    PROT__adjustment_attach(vadjustment, viewport->object->widget);

    Viewport_setsignals( viewport );

    PROT__signal_push( viewport->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return viewport->object->widget;
}

SDLGuiTK_Adjustment * SDLGuiTK_viewport_get_hadjustment(SDLGuiTK_Viewport *viewport)
{
    return viewport->hadjustment;
}

SDLGuiTK_Adjustment * SDLGuiTK_viewport_get_vadjustment(SDLGuiTK_Viewport *viewport)
{
    return viewport->vadjustment;
}

void PROT_viewport_setsize(SDLGuiTK_Viewport * viewport,
                           int width, int height)
{
    viewport->force_width = width;
    viewport->force_height = height;
}
