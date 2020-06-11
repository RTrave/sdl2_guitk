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
#include "../mywm.h"
#include "../theme_prot.h"
#include "../context_prot.h"
#include "../container_prot.h"
#include "../bin_prot.h"
#include "window_prot.h"


#define WINDOW_BORDER_SIZE ((int) 2)


static SDL_bool SDLGuiTK_IS_WINDOW( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->bin!=NULL ) {
                if( widget->container->bin->window!=NULL ) {
                    return SDL_TRUE;
                }
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_WINDOW(): widget is not a window\n" );
    return SDL_FALSE;
}

SDLGuiTK_Window * SDLGuiTK_WINDOW( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_WINDOW(widget) ) return NULL;
    return widget->container->bin->window;
}


static int current_id=0;


static SDLGuiTK_Window * Window_create()
{
    SDLGuiTK_Window * new_window;

    new_window = malloc( sizeof( struct SDLGuiTK_Window ) );
    new_window->bin = PROT__bin_new();
    new_window->object = new_window->bin->object;
    new_window->bin->window = new_window;
    sprintf( new_window->object->name, "window%d", ++current_id );
    new_window->object->widget->top = NULL;
    new_window->object->widget->parent = NULL;

    strcpy( new_window->title, new_window->object->name );
    new_window->position = SDLGUITK_WINDOW_CENTER;
    new_window->show_title = 0;

    new_window->srf = MySDL_Surface_new("Window_srf");
    new_window->area.x = 0;
    new_window->area.y = 0;
    new_window->area.w = 0;
    new_window->area.h = 0;
    new_window->title_srf = MySDL_Surface_new("Window_title_srf");
    new_window->title_area.x = 0;
    new_window->title_area.y = 0;
    new_window->title_area.w = 0;
    new_window->title_area.h = 0;

    new_window->wm_widget = MyWM_WMWidget_New( new_window->object->widget );

    return new_window;
}

static void Window_destroy( SDLGuiTK_Window * window )
{
    /*   MySDL_FreeSurface( window->active_srf ); */
    /*   MySDL_FreeSurface( window->shaded_srf ); */
    if( window->title_srf->srf!=NULL ) {
        SDL_FreeSurface (window->title_srf->srf);
        window->title_srf->srf = NULL;
    }
    MySDL_Surface_free( window->title_srf );
    MySDL_Surface_free( window->srf );

    PROT__bin_destroy( window->bin );
    MyWM_WMWidget_Delete( window->wm_widget );
    free( window );
}

static void Window_MakeTitleSurface( SDLGuiTK_Window * window )
{
    SDLGuiTK_Theme * theme;

    if(window->title_srf->srf!=NULL) return;
    theme = PROT__theme_get_and_lock();
    window->title_srf->srf = MyTTF_Render_Blended( window->title_srf->srf, \
                                                   window->title, 18, \
                                                   MyTTF_STYLE_BOLD, \
                                                   theme->bgcolor );
    PROT__theme_unlock( theme );
}

static void Window_DrawTitleSurface( SDLGuiTK_Window * window )
{
    Uint32 bdcolor;
    SDLGuiTK_Theme * theme;

    theme = PROT__theme_get_and_lock();
    bdcolor = SDL_MapRGBA( window->srf->srf->format, \
                           theme->bdcolor.r, \
                           theme->bdcolor.g, \
                           theme->bdcolor.b, \
                           255 );
    PROT__theme_unlock( theme );

    window->title_area.x = 0;
    window->title_area.y = 0;
    window->title_area.w = window->srf->srf->w;
    window->title_area.h = window->title_srf->srf->h;
    MySDL_FillRect( window->srf, &window->title_area, bdcolor );
    /* //SDL_UpdateRects( window->srf, 1, &window->title_area ); */
    /* //SDL_UpdateWindowSurface( window->srf ); */

    window->title_area.x = ( window->srf->srf->w - window->title_srf->srf->w )/2;
    window->title_area.y = 0;
    window->title_area.w = window->title_srf->srf->w;
    window->title_area.h = window->title_srf->srf->h;


    MySDL_BlitSurface( window->title_srf, NULL, \
                     window->srf, &window->title_area );
    //SDL_UpdateRect( window->srf, 0, 0, 0, 0 );
}


static void Window_UpdatePosition( SDLGuiTK_Window * window )
{
    SDLGuiTK_Widget * widget=window->object->widget;
    //SDL_Surface * surface;
    SDL_Window * mwindow;
    int w,h;

    mwindow = MySDL_GetVideoWindow();
    switch( window->position ) {
    case SDLGUITK_WINDOW_CENTER:
        SDL_GetWindowSize(mwindow,&w,&h);
        widget->abs_area.x =
            (int)(w/2) - (int)(widget->abs_area.w/2);
        widget->abs_area.y =
            (int)(h/2) - (int)(widget->abs_area.h/2);

        /*       printf( "Window2-%d\n",widget->abs_area.w); */
        /*     widget->abs_area.x =  */
        /*         (int)(surface->w/2) - (int)(widget->abs_area.w/2); */
        /* printf( "Window3a\n" ); */
        /*     widget->abs_area.y =  */
        /*       (int)(surface->h/2) - (int)(widget->abs_area.h/2); */
        /* printf( "Window3\n" ); */
        break;
    case SDLGUITK_WINDOW_MOUSE:

        break;
    case SDLGUITK_WINDOW_MOUSEALWAYS:

        break;
    default:
        break;
    }
}


static void Window_MakeBaseSurface( SDLGuiTK_Window * window )
{
    //Uint32 bgcolor;
    //Uint32 bdcolor;
    //SDLGuiTK_Theme * theme;

    Window_MakeTitleSurface( window );

    /* Prepare surface */
    MySDL_CreateRGBSurface( window->srf,\
                            window->wm_widget->child_area.w, \
                            window->wm_widget->child_area.h );

    /* Load theme values */
    //theme = PROT__theme_get_and_lock();
    /* bdcolor = SDL_MapRGBA( window->srf->format, \ */
    /* 	 theme->bdcolor.r, \ */
    /* 	 theme->bdcolor.g, \ */
    /* 	 theme->bdcolor.b, \ */
    /* 	 255 ); */
    /* bgcolor = SDL_MapRGBA( window->srf->format, \ */
    /* 	 theme->bgcolor.r, \ */
    /* 	 theme->bgcolor.g, \ */
    /* 	 theme->bgcolor.b, \ */
    /* 	 255 ); */
    /*   SDL_FillRect( window->object->widget->srf, &window->area, bgcolor ); */
    /* PROT__theme_unlock( theme ); */

    Window_DrawTitleSurface( window );

    /* Blit border, backgroud and title */
    /*   SDL_FillRect( window->active_srf, NULL, bdcolor ); */
    /*   SDL_UpdateRect( window->active_srf, 0, 0, 0, 0 ); */

    window->area.x = 0;
    if( window->show_title )
        window->area.y = window->title_srf->srf->h;
    else
        window->area.y = 0;
    window->area.w = window->object->widget->abs_area.w;
    window->area.h = window->object->widget->abs_area.h;

    if( window->show_title )
        window->wm_widget->area.y -= window->title_srf->srf->h;

    /*   SDL_FillRect( window->active_srf, &window->area, bgcolor ); */
    /*   SDL_UpdateRect( window->active_srf, 0, 0, 0, 0 ); */

    /*   printf( " window->area %d %d %d %d\n", window->area.x, window->area.y, window->area.w, window->area.h ); */
    MySDL_BlitSurface( window->object->widget->srf, NULL, \
                       window->srf, &window->area );
    //2SDL_UpdateRects( window->srf, 1, &window->area );
    //SDL_UpdateWindowSurface( window->object->widget->srf );
}


static void Window_MakeShadedSurface( SDLGuiTK_Window * window )
{
    /*   window->shaded_srf = MySDL_CopySurface_with_alpha( window->shaded_srf, \ */
    /* 						     window->active_srf,\ */
    /* 						     128 ); */
}


static void * Window_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Window * window=widget->container->bin->window;

    PROT__widget_reset_req_area( widget );
    Window_MakeTitleSurface( window );

    /* UPDATE SELF ASCENDENTS */
    widget->parent = widget;
    widget->top = widget;

    PROT__bin_DrawUpdate( window->bin );

    if( window->show_title ) {
        if( widget->req_area.w<window->title_srf->srf->w ) {
            widget->req_area.w = window->title_srf->srf->w;
            //widget->abs_area.w = window->title_srf->w;
        }
    } else {
        if( widget->req_area.w<50 ) {
            widget->req_area.w = 50;
            //widget->abs_area.w = 50;
        }
    }

    /* Prepare WMWidget dimensions */
    window->wm_widget->child_area.w = widget->req_area.w;
    if( window->show_title )
        window->wm_widget->child_area.h = widget->req_area.h + window->title_srf->srf->h;
    else
        window->wm_widget->child_area.h = widget->req_area.h;

    window->wm_widget->surface2D_flag = 1;

    MyWM_WMWidget_DrawUpdate( window->wm_widget );

    return (void *) NULL;
}


static void * Window_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Window   * window=widget->container->bin->window;
    int wdiff=0, hdiff=0;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;

    if( window->bin->child!=NULL ) {

        wdiff = widget->abs_area.w - (2*widget->container->border_width) \
                - window->bin->child->rel_area.w;
        hdiff = widget->abs_area.h - (2*widget->container->border_width) \
                - window->bin->child->rel_area.h;

        wdiff = 0; /* TODO: actually unused ??? */
        hdiff = 0;
        if( wdiff>0 ) {
            widget->container->children_area.w = window->bin->child->rel_area.w + wdiff;
            widget->container->children_area.x = widget->container->border_width;
        }
        if( hdiff>0 ) {
            widget->container->children_area.h = window->bin->child->rel_area.h + hdiff;
            widget->container->children_area.y = widget->container->border_width;
        }
    }

    PROT__bin_DrawBlit( window->bin );

    theme = PROT__theme_get_and_lock();
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->bgcolor.r, \
                           theme->bgcolor.g, \
                           theme->bgcolor.b, \
                           255 );
    PROT__theme_unlock( theme );

    if( window->bin->child!=NULL ) {
        /*     SDL_mutexP( window->bin->child->object->mutex ); */
        if( window->bin->child->shown==1 ) {
            MySDL_FillRect( widget->srf, &widget->rel_area, bgcolor );
            MySDL_BlitSurface( window->bin->child->srf, NULL, \
                               widget->srf, &window->bin->child->rel_area );
            //2SDL_UpdateRects( widget->srf, 1, &window->bin->child->rel_area );
            //SDL_UpdateWindowSurface( window->bin->child->srf );
        }
        /*     SDL_mutexV( window->bin->child->object->mutex ); */
    }

    Window_MakeBaseSurface( window );

    Window_MakeShadedSurface( window );

    //widget->rel_area.w = widget->abs_area.w;
    //widget->rel_area.h = widget->abs_area.h;

    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    MyWM_WMWidget_DrawBlit( window->wm_widget );

    MySDL_BlitSurface(  window->srf, NULL, \
                        window->wm_widget->srf, &window->wm_widget->child_area );
    //2SDL_UpdateRects( window->wm_widget->srf, 1, &window->wm_widget->child_area );
    //SDL_UpdateWindowSurface( window->srf );

    return (void *) NULL;
}


static SDLGuiTK_Widget * Window_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{
    SDLGuiTK_Widget * child;
    SDLGuiTK_Widget * active;

    child = SDLGuiTK_bin_get_child( widget->container->bin );
    if( child==NULL ) {
        SDLGUITK_ERROR( "Window_RecursiveEntering(): child==NULL\n" );
        return NULL;
    }
    if( child->shown==0 ) {
        return NULL;
    }
    active = PROT__widget_is_entering( child, x, y );

    return active;
}

static void * Window_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * child;

    PROT__context_unref_wmwidget( widget->container->bin->window->wm_widget );
    child = SDLGuiTK_bin_get_child( widget->container->bin );
    /*   PROT__bin_remove( widget->container->bin, child ); */

    if( child!=NULL ) {
        SDLGuiTK_widget_destroy( child );
    }

    return (void *) NULL;
}

static void * Window_Free( SDLGuiTK_Widget * widget )
{
    Window_destroy( widget->container->bin->window );

    return (void *) NULL;
}




static void * Window_Realize( SDLGuiTK_Widget * widget, \
                              void * data, void * event )
{

    Window_UpdatePosition( widget->container->bin->window );
    Window_DrawUpdate( widget );
    Window_DrawBlit( widget );

    return (void *) NULL;
}

static void * Window_Destroy( SDLGuiTK_Widget * widget, \
                              void * data, void * event )
{
    /*   SDLGuiTK_Window * window=widget->container->bin->window; */

    /*   SDL_mutexV( widget->object->mutex ); */
    /*   PROT__context_unref_wmwidget( window->wm_widget ); */
    /*   SDL_mutexP( widget->object->mutex ); */
    widget->shown = 0;

    return (void *) NULL;
}

static void * Window_Show( SDLGuiTK_Widget * widget, \
                           void * data, void * event )
{
    SDLGuiTK_Window * window=widget->container->bin->window;

    if( widget->shown==1 ) return (void *) NULL;
    widget->shown = 1;
    Window_UpdatePosition( window );
    /*   SDL_mutexV( widget->object->mutex ); */
    PROT__context_ref_wmwidget( window->wm_widget );
    /*   SDL_mutexP( widget->object->mutex ); */

    return (void *) NULL;
}

static void * Window_Hide( SDLGuiTK_Widget * widget, \
                           void * data, void * event )
{
    SDLGuiTK_Window * window=widget->container->bin->window;

    if( widget->shown==0 ) return (void *) NULL;
    /*   SDL_mutexV( widget->object->mutex ); */
    PROT__context_unref_wmwidget( window->wm_widget );
    /*   SDL_mutexP( widget->object->mutex ); */
    widget->shown = 0;

    return (void *) NULL;
}

static void * Window_FrameEvent( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{
    Window_DrawUpdate( widget );
    Window_DrawBlit( widget );
    PROT_MyWM_checkactive( widget );

    return (void *) NULL;
}

static void * Window_MouseEnter( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{
    /*   widget->WM_srf = widget->container->bin->window->active_srf; */
    /*   if( widget->absolute_srf!=widget->container->bin->window->active_srf ) { */
    /*     widget->absolute_srf = widget->container->bin->window->active_srf; */
    /*   } */
    /*   PROT__box_activate( window->window->main_box->box ); */
    MyCursor_Set( SDLGUITK_CURSOR_DEFAULT );

    return (void *) NULL;
}

static void * Window_MouseLeave( SDLGuiTK_Widget * widget, \
                                 void * data, void * event )
{
    /*   widget->WM_srf = widget->container->bin->window->shaded_srf; */
    /*   if( widget->absolute_srf!=widget->container->bin->window->shaded_srf ) { */
    /*     widget->absolute_srf = widget->container->bin->window->shaded_srf; */
    /*   } */
    /*   PROT__box_update_relwin( window->window->main_box->box ); */
    /*   Intern__SDLGuiTK_events_unref_widgets(); */
    MyCursor_Unset();

    return (void *) NULL;
}



/* static void * Window_MousePressed( SDLGuiTK_Widget * window, \ */
/* 				   void * data ) */
/* { */
/*   int x, y; */
/*   if( SDL_GetMouseState( &x, &y) ) { */
/*     printf( "Mouse pressed in: x=%d y=%d\n", x, y ); */
/*     window->moving = 1; */
/*   } */

/*   return (void *) NULL; */
/* } */

static void * Window_MouseReleased( SDLGuiTK_Widget * window, \
                                    void * data, void * event )
{
    /*   int x, y; */

    /*   if( SDL_GetMouseState( &x, &y) ) { */
    /*     printf( "Mouse released in: x=%d y=%d\n", x, y ); */
    /*   } */
    /*   printf( "Mouse released in: x=%d y=%d\n", x, y ); */
    /*   PROT__box_update_relwin( window->window->main_box->box ); */
    Window_DrawUpdate( window );
    Window_DrawBlit( window );

    return (void *) NULL;
}


static void Window_setsignals( SDLGuiTK_Window * window )
{
    SDLGuiTK_SignalHandler * handler;

    handler = (SDLGuiTK_SignalHandler *) window->object->signalhandler;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
            Window_Realize;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_DESTROY]->function = \
            Window_Destroy;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
            Window_Show;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
            Window_Hide;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_FRAMEEVENT]->function = \
            Window_FrameEvent;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_ENTER]->function = \
            Window_MouseEnter;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_LEAVE]->function = \
            Window_MouseLeave;

    /*   handler->fdefault[SDLGUITK_SIGNAL_TYPE_PRESSED]->function = \ */
    /*     Window_MousePressed; */
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_RELEASED]->function = \
            Window_MouseReleased;
}

SDLGuiTK_Widget * SDLGuiTK_window_new()
{
    SDLGuiTK_Window * window;

    window = Window_create();

    window->object->widget->RecursiveEntering = Window_RecursiveEntering;
    window->object->widget->RecursiveDestroy = Window_RecursiveDestroy;
    window->object->widget->Free = Window_Free;

    window->object->widget->DrawUpdate = Window_DrawUpdate;
    window->object->widget->DrawBlit = Window_DrawBlit;

    Window_setsignals( window );

    window->object->widget->parent = NULL;
    window->object->widget->top = NULL;

    PROT__signal_push( window->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return window->object->widget;
}

void SDLGuiTK_window_set_title( SDLGuiTK_Widget * window,\
                                const char *title )
{
    /*   SDL_mutexP( window->object->mutex ); */
    strcpy( window->container->bin->window->title, title );
    window->container->bin->window->show_title = 1;
    /*   SDL_mutexV( window->object->mutex ); */

    PROT__signal_push( window->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
}

void SDLGuiTK_window_set_position( SDLGuiTK_Widget * window, \
                                   int position, \
                                   void *data )
{
    /*   SDL_mutexP( window->object->mutex ); */

    window->container->bin->window->position = position;
    PROT__signal_push( window->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );

    /*   SDL_mutexV( window->object->mutex ); */
}

