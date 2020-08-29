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
#include "../debug.h"
#include "../mysdl.h"
#include "../list.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../container_prot.h"

#include "grid_prot.h"
#include "gridchild.h"


GridChild * GridChild_create( SDLGuiTK_Widget * widget )
{
    GridChild * new_child;

    new_child = malloc( sizeof( struct GridChild ) );
    new_child->child = widget;
    new_child->left = 0;
    new_child->top = 0;
    new_child->width = 1;
    new_child->height = 1;

    return new_child;
}

void GridChild_destroy( GridChild * child )
{
    free( child );
}


GridChild * GridChild_find( SDLGuiTK_Grid * grid, SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * current;
    GridChild * current_child;

    current_child = (GridChild *) SDLGuiTK_list_ref_init( grid->children );
    while( current_child!=NULL ) {
        current = current_child->child;
        if( current==widget ) {
            return current_child;
        }
        current_child = (GridChild *) SDLGuiTK_list_ref_next( grid->children );
    }

    return NULL;
}

GridChild * GridChild_remove( SDLGuiTK_Grid * grid, SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Widget * current;
    GridChild * current_child;

    current_child = (GridChild *) SDLGuiTK_list_refrv_init( grid->children );
    while( current_child!=NULL ) {
        current = current_child->child;
        if( current==widget ) {
            SDLGuiTK_list_remove( grid->children, (SDLGuiTK_Object *) current_child );
            PROT_List_refrv_reinit( grid->children );
            return current_child;
        }
        current_child = (GridChild *) SDLGuiTK_list_refrv_next( grid->children );
    }
    return NULL;
}

void       GridChild_DrawUpdate( GridChild * child )
{
    //SDLGuiTK_Grid * grid=child->child->parent->container->grid;
    SDLGuiTK_Widget * widget=child->child;

    /* IF CHILD AND PARENT ARE NOT SHOWN */
    if( !widget->visible ) return;   /* && widget->hided_parent==0 */

    /* UPDATE CHILD WIDGET */
    (*widget->DrawUpdate)( widget );

    /* Set expanded count */
    //if( child->expand==SDL_TRUE ) {
    //    grid->expanded_nb++;
    //};

    /* update child area for parent box suggestion */
    child->area.w = widget->req_area.w;// + 2*child->padding;
    child->area.h = widget->req_area.h;// + 2*child->padding;

}


void       GridChild_DrawBlit( GridChild * child )
{
    //SDLGuiTK_Grid * grid=child->child->parent->container->grid;
    SDLGuiTK_Widget * wgrid=child->child->parent;
    SDLGuiTK_Widget * widget=child->child;

    if( !widget->visible ) return;   /* && widget->hided_parent==0 */

    /* CHILD SIZE SUGGESTION */
    widget->abs_area.x = wgrid->abs_area.x + child->area.x;
    widget->abs_area.y = wgrid->abs_area.y + child->area.y;
    widget->rel_area.x = child->area.x;
    widget->rel_area.y = child->area.y;

    //widget->req_area.w = child->area.w;
    //widget->req_area.h = child->area.h;

    //if( child->fill==SDL_TRUE ) {
        //?widget->abs_area.x += child->padding;
        //?    widget->rel_area.x += child->padding;
    //widget->req_area.w = child->area.w; // - (2*child->padding);
    //widget->req_area.h = child->area.h;
    PROT__widget_set_req_area (widget, child->area.w, child->area.h);
    /*}
    if( child->fill==SDL_FALSE ) {
        widget->abs_area.x += (int)((child->area.w-widget->req_area.w)/2);
            widget->rel_area.x += (int)((child->area.w-widget->req_area.w)/2);
            //widget->req_area.w -= (child->area.w-widget->req_area.w);
            //widget->req_area.w = child->area.w;
            widget->req_area.h = child->area.h;
    }*/

    (*widget->DrawBlit)( widget );

    MySDL_BlitSurface(  widget->srf, NULL, \
                        wgrid->srf, &widget->rel_area );
}



