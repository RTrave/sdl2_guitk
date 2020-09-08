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

#include <SDL2/SDL_guitk.h>
#include "../debug.h"
#include "../mysdl.h"
#include "../signal.h"
#include "../list.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../theme_prot.h"
#include "../container_prot.h"
#include "box_prot.h"

#include "boxchild.h"



SDL_bool SDLGuiTK_IS_BOX( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->box!=NULL ) {
                return SDL_TRUE;
            }
        }
    }
    return SDL_FALSE;
}

SDLGuiTK_Box * SDLGuiTK_BOX( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_BOX(widget) ) {
        SDLGUITK_ERROR( "SDLGuiTK_IS_BOX(): widget is not a box\n" );
        return NULL;
    }
    return widget->container->box;
}


static int current_id=0;

static SDLGuiTK_Box * Box_create()
{
    SDLGuiTK_Box * new_box;

    new_box = malloc( sizeof( struct SDLGuiTK_Box ) );
    new_box->container = PROT__container_new();
    new_box->object = new_box->container->object;
    new_box->container->box = new_box;
    sprintf( new_box->object->name, "box%d", ++current_id );

    new_box->type = 0;
    new_box->spacing = 0;
    new_box->homogeneous = SDL_FALSE;
    new_box->homogeneous_size = 0;
    new_box->children = SDLGuiTK_list_new();

    return new_box;
}

static void Box_destroy( SDLGuiTK_Box * box )
{
    SDLGuiTK_list_destroy( box->children );

    PROT__container_destroy( box->object->widget->container );
    free( box );
}



static void * Box_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Box * box=widget->container->box;
    BoxChild * current_child;
    //SDLGuiTK_Widget * current;

    /* INIT BOX VALUES */
    box->shown_nb = 0;
    box->base_size = 0;
    box->expanded_nb = 0;
    box->expanded_size = 0;
    box->homogeneous_size = 0;
    box->container->children_area.w = 0;
    box->container->children_area.h = 0;

    PROT__widget_reset_req_area( widget );

    /* START CHILDREN UPDATE */
    //SDLGuiTK_list_lock( box->children );

    /* IF TOP WIDGET NO EXISTS ... */
    //if( widget->top==NULL ) {
    //    SDLGUITK_ERROR( "Box_UpdateDim(): can't be a top widget\n" );
    //    return 0;
    //}

    current_child = (BoxChild *) SDLGuiTK_list_ref_init( box->children );
    while( current_child ) {
        //current = current_child->child;

        /* UPDATE CHILD ASCENDENTS */
        //current->top = widget->top;
        //current->parent = widget;

        /* IF CHILD SHOWN */
        if( widget->visible && current_child->child->visible ) {

            BoxChild_DrawUpdate( current_child );

            if( box->type==SDLGUITK_BOX_H ) {
                /* Set homogoneous_size to the max value */
                if( box->homogeneous_size<current_child->area.w ) {
                    box->homogeneous_size = current_child->area.w;
                }
                /* Suggested values for container */
                if( box->shown_nb!=0 ) {
                    box->container->children_area.w += box->spacing;
                }
                box->container->children_area.w += current_child->area.w;
                if( box->container->children_area.h<current_child->area.h ) {
                    box->container->children_area.h = current_child->area.h;
                }
            }

            if( box->type==SDLGUITK_BOX_V ) {
                /* Set homogoneous_size to the max value */
                if( box->homogeneous_size<current_child->area.h ) {
                    box->homogeneous_size = current_child->area.h;
                }
                /* Suggested values for container */
                if( box->shown_nb!=0 ) {
                    box->container->children_area.h += box->spacing;
                }
                box->container->children_area.h += current_child->area.h;
                if( box->container->children_area.w<current_child->area.w ) {
                    box->container->children_area.w = current_child->area.w;
                }
            }

            box->shown_nb++;
        }

        current_child = (BoxChild *) SDLGuiTK_list_ref_next( box->children );
    }


    if( box->homogeneous==SDL_TRUE ) {
        if( box->type==SDLGUITK_BOX_H ) {
            box->container->children_area.w = \
                                              box->shown_nb*box->homogeneous_size + (box->shown_nb-1)*box->spacing;

        }
        if( box->type==SDLGUITK_BOX_V ) {
            box->container->children_area.h = \
                                              box->shown_nb*box->homogeneous_size + (box->shown_nb-1)*box->spacing;
        }
    }
    if( box->type==SDLGUITK_BOX_H ) {
        box->base_size = box->container->children_area.w;
    }
    if( box->type==SDLGUITK_BOX_V ) {
        box->base_size = box->container->children_area.h;
    }
    PROT__container_DrawUpdate( box->container );

    return (void *) NULL;
}

static void * Box_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Box * box=widget->container->box;
    BoxChild * current_child;

    box->current_x=box->container->border_width;
    box->current_y=box->container->border_width;

    if( box->shown_nb<1 ) {
        //current_child = (BoxChild *) SDLGuiTK_list_ref_init( box->children );
        //while( current_child ) {
        //    current_child = (BoxChild *) SDLGuiTK_list_ref_next( box->children );
        //} //TODO loop useless

        PROT__container_DrawBlit( box->container );
        //SDLGuiTK_list_unlock( box->children );
        SDLGUITK_ERROR( "Box_DrawBlit(): box->shown_nb<1\n" );
        return 0;
    }

    int current_w = 0;
    int current_h = 0;
        if( box->type==SDLGUITK_BOX_H ) {
            box->homogeneous_size =
                (widget->req_area.w - 2*box->container->border_width
                 - (box->shown_nb-1)*box->spacing )
                / box->shown_nb ;
            current_w = box->base_size + 2*box->container->border_width;
            box->expanded_size = widget->req_area.w - current_w;
        }
        if( box->type==SDLGUITK_BOX_V ) {
            box->homogeneous_size =
                (widget->req_area.h - 2*box->container->border_width
                 - (box->shown_nb-1)*box->spacing )
                / box->shown_nb ;
            current_h = box->base_size + 2*box->container->border_width;
            box->expanded_size = widget->req_area.h - current_h;
        }
#if DEBUG_LEVEL >= 3
    printf(" ** BOX %s: current_w=%d current_h=%d\n", widget->object->name, current_w, current_h);
    printf("        req_w=%d     req_h=%d\n", widget->req_area.w, widget->req_area.h);
    printf("    current_w=%d current_h=%d\n", current_w, current_h);
#endif

/* DONE in Container drawblit
    box->container->children_area.w = \
                                      widget->req_area.w - 2*box->container->border_width;
    box->container->children_area.h = \
                                      widget->req_area.h - 2*box->container->border_width;
*/
    PROT__container_DrawBlit( box->container );

    //if( !widget->top ) {
        //SDLGuiTK_list_unlock( box->children );
    //    SDLGUITK_ERROR( "Box_UpdateDim(): can't be a top widget\n" );
    //    return 0;
    //}

    box->shown_nb = 0;

    current_child = (BoxChild *) SDLGuiTK_list_ref_init( box->children );
    while( current_child ) {

        if( widget->visible && current_child->child->visible  ) {

            current_child->area.x = box->current_x;
            current_child->area.y = box->current_y;
            if( box->homogeneous==SDL_TRUE ) {
                if( box->type==SDLGUITK_BOX_H ) {
                    current_child->area.w = box->homogeneous_size;
                    current_child->area.h = box->container->children_area.h;
                }
                if( box->type==SDLGUITK_BOX_V ) {
                    current_child->area.w = box->container->children_area.w;
                    current_child->area.h = box->homogeneous_size;
                }
            } else if( current_child->expand==SDL_TRUE ) {
                if( box->type==SDLGUITK_BOX_H ) {
                    current_child->area.w += (int)(box->expanded_size/box->expanded_nb);
                    current_child->area.h = box->container->children_area.h;
                }
                if( box->type==SDLGUITK_BOX_V ) {
                    current_child->area.w = box->container->children_area.w;
                    current_child->area.h += (int)(box->expanded_size/box->expanded_nb);
                }
            } else {
                if( box->type==SDLGUITK_BOX_H ) {
                    current_child->area.h = box->container->children_area.h;
                }
                if( box->type==SDLGUITK_BOX_V ) {
                    current_child->area.w = box->container->children_area.w;
                }
            }

            BoxChild_DrawBlit( current_child );

            if( box->type==SDLGUITK_BOX_H ) {
                box->current_x += ( current_child->area.w + box->spacing );
            }
            if( box->type==SDLGUITK_BOX_V ) {
                box->current_y += ( current_child->area.h + box->spacing );
            }

            box->shown_nb++;
        }

        current_child = (BoxChild *) SDLGuiTK_list_ref_next( box->children );
    }

    //SDLGuiTK_list_unlock( box->children );

    widget->rel_area.w = widget->abs_area.w;
    widget->rel_area.h = widget->abs_area.h;

    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    return (void *) NULL;
}


static SDLGuiTK_Widget * Box_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{
    SDLGuiTK_Box * box=widget->container->box;
    BoxChild * current_child;
    SDLGuiTK_Widget * child;
    SDLGuiTK_Widget * active;

    //SDLGuiTK_list_lock( box->children );
    current_child = (BoxChild *) SDLGuiTK_list_ref_init( box->children );
    while( current_child!=NULL ) {
        child = current_child->child;
        if( child->visible ) {
            active = PROT__widget_is_entering( child, x, y );
            if( active!=NULL ) {
                SDLGuiTK_list_ref_reset( box->children );
                //SDLGuiTK_list_unlock( box->children );
                return active;
            }
        }
        current_child = (BoxChild *) SDLGuiTK_list_ref_next( box->children );
    }


    //SDLGuiTK_list_unlock( box->children );

    return NULL;

}

static void * Box_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * current;
    BoxChild * child;

    //SDLGuiTK_list_lock( widget->container->box->children );

    child = \
            (BoxChild *) SDLGuiTK_list_ref_init( widget->container->box->children );
    while( child!=NULL ) {
        current = child->child;
        SDLGuiTK_widget_destroy( current );
        child = \
                (BoxChild *) SDLGuiTK_list_ref_next( widget->container->box->children );
    }

    //SDLGuiTK_list_unlock( widget->container->box->children );

    return (void *) NULL;
}

static void * Box_Free( SDLGuiTK_Widget * widget )
{
    Box_destroy( widget->container->box );

    return (void *) NULL;
}


static void Box_Init_functions( SDLGuiTK_Box * box )
{
    box->object->widget->RecursiveEntering = Box_RecursiveEntering;
    box->object->widget->RecursiveDestroy = Box_RecursiveDestroy;
    box->object->widget->Free = Box_Free;

    box->object->widget->DrawUpdate = Box_DrawUpdate;
    box->object->widget->DrawBlit = Box_DrawBlit;
}


SDLGuiTK_Widget * SDLGuiTK_hbox_new()
{
    SDLGuiTK_Box * hbox;

    hbox = Box_create();
    hbox->type = SDLGUITK_BOX_H;
    Box_Init_functions( hbox );
    PROT__signal_push( hbox->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return hbox->object->widget;
}

SDLGuiTK_Widget * SDLGuiTK_vbox_new()
{
    SDLGuiTK_Box * vbox;

    vbox = Box_create();
    vbox->type = SDLGUITK_BOX_V;
    Box_Init_functions( vbox );
    PROT__signal_push( vbox->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return vbox->object->widget;
}



void SDLGuiTK_box_pack_start( SDLGuiTK_Box * box, \
                              SDLGuiTK_Widget * widget, \
                              SDL_bool expand,
                              SDL_bool fill,
                              int padding )
{
    BoxChild * child;

    child = BoxChild_create( widget );
    child->expand = expand;
    child->fill = fill;
    child->padding = padding;

    widget->parent = box->object->widget;
    PROT__widget_set_top (widget, box->object->widget->top);

    SDLGuiTK_list_append( box->children, (SDLGuiTK_Object *) child );
}



void SDLGuiTK_box_set_spacing( SDLGuiTK_Box * box, int spacing )
{
    box->spacing = spacing;
    if( box->object->widget->parent ) {
        PROT__signal_push( box->object->widget->parent->object,
                           SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    }
}


int  SDLGuiTK_box_get_spacing( SDLGuiTK_Box * box )
{
    int spacing;

    spacing = box->spacing;

    return spacing;
}


void SDLGuiTK_box_set_homogeneous( SDLGuiTK_Box * box, SDL_bool homogeneous )
{
    box->homogeneous = homogeneous;
    if( box->object->widget->parent ) {
        PROT__signal_push( box->object->widget->parent->object,
                           SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
    }
}

SDL_bool  SDLGuiTK_box_get_homogeneous( SDLGuiTK_Box * box )
{
    SDL_bool homogeneous;

    homogeneous = box->homogeneous;

    return homogeneous;
}

void SDLGuiTK_box_set_orientation( SDLGuiTK_Box *box, int orientation)
{
    box->type = orientation;
}

void PROT__box_remove( SDLGuiTK_Box * this_box, \
                       SDLGuiTK_Widget * widget )
{
    BoxChild * child;
#if DEBUG_LEVEL > 1
    char tmplog[256];
    sprintf( tmplog, "__box_remove():  %s from %s\n", widget->object->name, this_box->object->name );
    SDLGUITK_LOG( tmplog );
#endif

    if( (child=(BoxChild *) SDLGuiTK_list_remove(this_box->children,(SDLGuiTK_Object *)widget)) ) {
        if( this_box->object->widget->parent ) {
            PROT__signal_push( this_box->object->widget->parent->object,
                               SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
        }
        widget->parent = NULL;
        PROT__widget_set_top (widget, NULL);
        BoxChild_destroy( child );
    } else {
        SDLGUITK_ERROR( "PROT__box_remove(): widget not found in box\n" );
    }
}

void PROT__box_set_top( SDLGuiTK_Box *box, SDLGuiTK_Widget *top)
{
    BoxChild * child;
    child = (BoxChild*) SDLGuiTK_list_ref_init (box->children);
    while(child) {
        PROT__widget_set_top (child->child, top);
        child = (BoxChild*) SDLGuiTK_list_ref_next (box->children);
    }
}

