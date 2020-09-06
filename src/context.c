/*
   sdl_guitk - GUI toolkit designed for SDL environnements (GTK-style).

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


#include "context_prot.h"
#include "render/surface2d.h"
#include "render/render.h"
#include "render/mywm.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "render/tooltip_prot.h"
#include "signal.h"
#include "debug.h"
#include "list.h"


static SDL_bool tooltip_enabled = SDL_FALSE;


static SDLGuiTK_Context * Context_create()
{
    SDLGuiTK_Context * new_context;

    new_context = malloc( sizeof( struct SDLGuiTK_Context ) );
    new_context->surface = NULL;
    new_context->type = SDLGUITK_CONTEXT_MODE_NULL;
    new_context->mutex = SDL_CreateMutex();

    new_context->main_render = NULL;
    new_context->activables = SDLGuiTK_list_new();
    new_context->ref = SDLGuiTK_list_new();
    new_context->unref = SDLGuiTK_list_new();
    new_context->focused = NULL;
    new_context->active_wmwidget = NULL;

    return new_context;
}

static void Context_destroy( SDLGuiTK_Context * context )
{
    SDLGuiTK_list_destroy( context->ref );
    SDLGuiTK_list_destroy( context->unref );
    SDLGuiTK_list_destroy( context->activables );
    SDL_DestroyMutex( context->mutex );
    free( context );
}


void PROT__context_new( int type, SDL_Window * window, SDL_Renderer * renderer )
{
    SDLGuiTK_Render * new_render;
    MyWM_Init ();
    current_context = Context_create();
    current_context->type = type;
    if(type==SDLGUITK_CONTEXT_MODE_SELF) {
        new_render = Render_create();
        current_context->surface = SDL_GetWindowSurface(new_render->window);
        current_context->main_render = new_render;
    } else if(type==SDLGUITK_CONTEXT_MODE_EMBED) {
        new_render = Render_set( window, renderer );
        current_context->surface = SDL_GetWindowSurface(new_render->window);
        current_context->main_render = new_render;
    } else if(type==SDLGUITK_CONTEXT_MODE_MULTIPLE) {
        Render_ModeSetWidth (10);
        Render_ModeSetHeight (10);
    }
}


void PROT__context_quit()
{
    SDLGuiTK_WMWidget * rest=NULL;

    SDLGuiTK_list_lock( current_context->activables );
    rest = (SDLGuiTK_WMWidget *) \
           SDLGuiTK_list_ref_init( current_context->activables );
    while( rest!=NULL ) {
        SDLGuiTK_widget_destroy( rest->widget );
        rest = (SDLGuiTK_WMWidget *) \
               SDLGuiTK_list_ref_next( current_context->activables );
    }
    SDLGuiTK_list_unlock( current_context->activables );

    PROT__signal_check();
    SDL_Delay( 200 );
    PROT__signal_check();
}

void PROT__context_uninit()
{
    Context_destroy( current_context );
    MyWM_Uninit ();
}


void PROT__context_ref_wmwidget( SDLGuiTK_WMWidget * wm_widget )
{
    SDLGuiTK_list_append( current_context->ref, \
                          (SDLGuiTK_Object *) wm_widget );
#if DEBUG_LEVEL >= 2
    char tmpstr[512];
    sprintf( tmpstr, \
             "_context_ref_wmwidget(): %s\n", wm_widget->object->name );
    SDLGUITK_LOG( tmpstr );
#endif
}

void PROT__context_unref_wmwidget( SDLGuiTK_WMWidget * wm_widget )
{
    SDLGuiTK_WMWidget * rem_widget=NULL;

    rem_widget = (SDLGuiTK_WMWidget *)
                 SDLGuiTK_list_remove(current_context->activables,
                                      (SDLGuiTK_Object *) wm_widget );
    if( rem_widget ) {
        SDLGuiTK_list_append( current_context->unref, \
                              (SDLGuiTK_Object *) wm_widget->surface2D );
        if(wm_widget->render && !wm_widget->is_wmchild) {
            Render_destroy (wm_widget->render);
            wm_widget->render = NULL;
        } else if(wm_widget->is_wmchild) {

            SDLGuiTK_list_remove (wm_widget->wmparent->children,
                                  (SDLGuiTK_Object *)wm_widget);

        }
        if( current_context->focused && current_context->focused==rem_widget) {
            current_context->focused->enter = SDL_FALSE;
            current_context->focused = NULL;
            MyWM_WidgetUnFocus();
            if(current_context->type!=SDLGUITK_CONTEXT_MODE_MULTIPLE &&
               rem_widget->is_wmchild)
                MyWM_UpdateFocused();
        }
        if( rem_widget==wm_widget ) {
            PROT__signal_push( rem_widget->widget->object,
                               SDLGUITK_SIGNAL_TYPE_UNMAP );
#if DEBUG_LEVEL >= 2
            char tmpstr[512];
            sprintf( tmpstr, \
                     "_context_unref_wmwidget(): %s\n", rem_widget->object->name );
            SDLGUITK_LOG( tmpstr );
#endif
        } else {
            SDLGUITK_ERROR( "_context_unref_wmwidget(): cancel!\n" );
            exit(0);
        }
    }
}

static SDLGuiTK_WMWidget * Context_get_wmwidget(SDLGuiTK_Widget * widget)
{
    SDLGuiTK_WMWidget * current;
    current = (SDLGuiTK_WMWidget *)
              SDLGuiTK_list_ref_init( current_context->activables );
    while( current ) {
        if(current->widget==widget) {
            SDLGuiTK_list_ref_reset( current_context->activables );
            return current;
        }
        current = (SDLGuiTK_WMWidget *)
                  SDLGuiTK_list_ref_next( current_context->activables );
    }
    SDLGUITK_ERROR ("WMWidget top not found !\n");
    return NULL;
}

void SDLGuiTK_update()
{
    SDLGuiTK_WMWidget * current=NULL;
    SDLGuiTK_Surface2D * current2D=NULL;

    SDL_mutexP( current_context->mutex );

    current = (SDLGuiTK_WMWidget *)
              SDLGuiTK_list_pop_head( current_context->ref );
    while( current ) {
        current->surface2D = Surface2D_new();
        if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE) {
            if(!current->is_wmchild) {
                current->render = Render_create ();
                if(current->title_shown)
                    SDL_SetWindowTitle(current->render->window,
                                   current->title);
                else
                    SDL_SetWindowTitle(current->render->window,
                                   current->widget->object->name);

                SDL_SetWindowSize (current->render->window,
                                   current->area.w-2*current->border_width,
                                   current->area.h-2*current->border_width);
                Render_clean ();
                SDL_RenderPresent (current->render->renderer);
            } else {
                current->render = NULL;
            }
        }
        if(current->is_wmchild) {
            SDLGuiTK_WMWidget * top;
            top = Context_get_wmwidget(current->parent);
            if(top) {
                current->wmparent = top;
                SDLGuiTK_list_append (top->children,
                                      (SDLGuiTK_Object *)current);
            }
        }
        SDLGuiTK_list_append( current_context->activables,
                              (SDLGuiTK_Object *) current );
        PROT__signal_push( current->widget->object,
                           SDLGUITK_SIGNAL_TYPE_MAP );
        if(current_context->focused &&
           current->widget!=current_context->focused->widget) {
            MyWM_WidgetUnFocus();
            MyWM_UpdateFocused();
        }

        current = (SDLGuiTK_WMWidget *)
                  SDLGuiTK_list_pop_head( current_context->ref );
    }

    current2D = (SDLGuiTK_Surface2D *)
                SDLGuiTK_list_pop_head( current_context->unref );
    while( current2D ) {
        Surface2D_destroy( current2D );
        current2D = (SDLGuiTK_Surface2D *)
                    SDLGuiTK_list_pop_head( current_context->unref );
    }

    SDL_mutexV( current_context->mutex );
    PROT__signal_check();
}

static void Context_blit_children(SDLGuiTK_WMWidget * wmwidget)
{
    SDLGuiTK_WMWidget * current;
    current = (SDLGuiTK_WMWidget *)
              SDLGuiTK_list_ref_init( wmwidget->children );
    while(current)
    {
        MyWM_blitsurface( current );
        current =   (SDLGuiTK_WMWidget *)
                    SDLGuiTK_list_ref_next( wmwidget->children );
    }
    if( tooltip_enabled ) {
        SDLGuiTK_Render * render=WMWidget_getrender(wmwidget);
        if(render==WMWidget_getrender(current_context->focused)) {
            if(!render)
                render = current_context->main_render;
            PROT__Tooltip_blit (render->tooltip);
        }
    }
}

void SDLGuiTK_blitsurfaces()
{
    SDLGuiTK_WMWidget * current=NULL;

    SDL_mutexP( current_context->mutex );

    if( SDLGuiTK_list_length( current_context->activables )==0 ) {
        SDL_mutexV( current_context->mutex );
        return;
    }
    current = (SDLGuiTK_WMWidget *) \
              SDLGuiTK_list_ref_init( current_context->activables );
    while( current!=NULL ) {

        if(!current->is_wmchild) {
            if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE) {
                SDL_GL_MakeCurrent (current->render->window, current->render->context);
            }
            MySDL_GL_Enter2DMode();
             if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE) {
                Render_clean();
            }
            MyWM_blitsurface( current );
            Context_blit_children(current);
            if(current_context->type!=SDLGUITK_CONTEXT_MODE_MULTIPLE)
                MySDL_GL_Leave2DMode();
            else {
                glFlush();
                SDL_GL_SwapWindow(current->render->window);
            }
        }
        current = (SDLGuiTK_WMWidget *) \
                  SDLGuiTK_list_ref_next( current_context->activables );
    }

    SDL_mutexV( current_context->mutex );
}


static SDLGuiTK_WMWidget * Context_getwmwidget_withwindowid(int id)
{
    SDLGuiTK_WMWidget * current=NULL;
    current = (SDLGuiTK_WMWidget *) \
                  SDLGuiTK_list_refrv_init( current_context->activables );
    while(current) {
        if(SDL_GetWindowID(current->render->window)==id) {
            return current;
        }
        current = (SDLGuiTK_WMWidget *) \
                  SDLGuiTK_list_refrv_next( current_context->activables );
    }
    SDLGUITK_ERROR ("No WMWidget found corresponding to window\n");
    return NULL;
}


void PROT__context_ref_tooltip( SDLGuiTK_Widget * widget )
{
    if(!widget->has_tooltip) return;
    if(current_context->focused) {
        SDLGuiTK_Render * render = WMWidget_getrender (current_context->focused);
        if(!render)
            render = current_context->main_render;
        PROT__Tooltip_update (render->tooltip, widget->tooltip_text);
        tooltip_enabled = SDL_TRUE;
    }
}

void PROT__context_unref_tooltip()
{
    tooltip_enabled = SDL_FALSE;
}


void PROT__context_renderclean()
{
    if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE)
        return;
    Render_clean();
}

void PROT__context_renderswap()
{
    if(current_context->type==SDLGUITK_CONTEXT_MODE_MULTIPLE)
        return;
    if(current_context->main_render)
        Render_swapbuffers(current_context->main_render);
}


void  SDLGuiTK_threads_enter()
{
    SDL_mutexP( current_context->mutex );
}

void  SDLGuiTK_threads_leave()
{
    SDL_mutexV( current_context->mutex );
}

