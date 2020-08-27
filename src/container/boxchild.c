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
#include "../list.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../container_prot.h"

#include "box_prot.h"
#include "boxchild.h"


BoxChild * BoxChild_create( SDLGuiTK_Widget * widget )
{
    BoxChild * new_child;

    new_child = malloc( sizeof( struct BoxChild ) );
    new_child->child = widget;
    new_child->expand = SDL_TRUE;
    new_child->fill = SDL_TRUE;
    new_child->padding = 0;

    return new_child;
}

void BoxChild_destroy( BoxChild * child )
{
    free( child );
}


BoxChild * BoxChild_find( SDLGuiTK_Box * box, SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * current;
    BoxChild * current_child;

    current_child = (BoxChild *) SDLGuiTK_list_ref_init( box->children );
    while( current_child!=NULL ) {
        current = current_child->child;
        if( current==widget ) {
            return current_child;
        }
        current_child = (BoxChild *) SDLGuiTK_list_ref_next( box->children );
    }

    return NULL;
}

BoxChild * BoxChild_remove( SDLGuiTK_Box * box, SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * current;
    BoxChild * current_child;

    current_child = (BoxChild *) SDLGuiTK_list_refrv_init( box->children );
    while( current_child!=NULL ) {
        current = current_child->child;
        if( current==widget ) {
            SDLGuiTK_list_remove( box->children, (SDLGuiTK_Object *) current_child );
            PROT_List_refrv_reinit( box->children );
            return current_child;
        }
        current_child = (BoxChild *) SDLGuiTK_list_refrv_next( box->children );
    }
    return NULL;
}


void       BoxChild_DrawUpdate( BoxChild * child )
{
    SDLGuiTK_Box * box=child->child->parent->container->box;
    SDLGuiTK_Widget * widget=child->child;

    /* IF CHILD AND PARENT ARE NOT SHOWN */
    if( !widget->visible ) return;   /* && widget->hided_parent==0 */

    /* UPDATE CHILD WIDGET */
    (*widget->DrawUpdate)( widget );

    /* Set expanded count */
    if( child->expand==SDL_TRUE ) {
        box->expanded_nb++;
    };

    /* update child area for parent box suggestion */
    if( box->type==SDLGUITK_BOX_H ) {
        child->area.w = widget->req_area.w + 2*child->padding;
        child->area.h = widget->req_area.h;
    }
    if( box->type==SDLGUITK_BOX_V ) {
        child->area.w = widget->req_area.w;
        child->area.h = widget->req_area.h + 2*child->padding;
    }

}


void       BoxChild_DrawBlit( BoxChild * child )
{
    SDLGuiTK_Box * box=child->child->parent->container->box;
    SDLGuiTK_Widget * wbox=child->child->parent;
    SDLGuiTK_Widget * widget=child->child;

    if( !widget->visible ) return;   /* && widget->hided_parent==0 */

    /* CHILD SIZE SUGGESTION */
    widget->abs_area.x = wbox->abs_area.x + box->current_x;
    widget->abs_area.y = wbox->abs_area.y + box->current_y;
    widget->rel_area.x = box->current_x;
    widget->rel_area.y = box->current_y;

    //widget->req_area.w = child->area.w;
    //widget->req_area.h = child->area.h;

    if( child->fill==SDL_TRUE ) {
        if( box->type==SDLGUITK_BOX_H ) {
            widget->abs_area.x += child->padding;
            widget->rel_area.x += child->padding;
            widget->req_area.w = child->area.w - (2*child->padding);
            widget->req_area.h = child->area.h;
        }
        if( box->type==SDLGUITK_BOX_V ) {
            widget->abs_area.y += child->padding;
            widget->rel_area.y += child->padding;
            widget->req_area.w = child->area.w;
            widget->req_area.h = child->area.h - (2*child->padding);
        }
    }
    if( child->fill==SDL_FALSE ) {
        if( box->type==SDLGUITK_BOX_H ) {
            widget->abs_area.x += (int)((child->area.w-widget->req_area.w)/2);
            widget->rel_area.x += (int)((child->area.w-widget->req_area.w)/2);
            //widget->req_area.w -= (child->area.w-widget->req_area.w);
            //widget->req_area.w = child->area.w;
            widget->req_area.h = child->area.h;
        }
        if( box->type==SDLGUITK_BOX_V ) {
            widget->abs_area.y += (int)((child->area.h-widget->req_area.h)/2);
            widget->rel_area.y += (int)((child->area.h-widget->req_area.h)/2);
            //widget->req_area.h -= (child->area.h-widget->req_area.h);
            widget->req_area.w = child->area.w;
            //widget->req_area.h = child->area.h;
        }
    }

    (*widget->DrawBlit)( widget );

    MySDL_BlitSurface(  widget->srf, NULL, \
                        wbox->srf, &widget->rel_area );
}



