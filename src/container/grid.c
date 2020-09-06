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

#include <SDL2/SDL_guitk.h>
#include "../debug.h"
#include "../mysdl.h"
#include "../signal.h"
#include "../list.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../theme_prot.h"
#include "../container_prot.h"
#include "grid_prot.h"

#include "gridchild.h"


static SDL_bool SDLGuiTK_IS_GRID( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->container!=NULL ) {
            if( widget->container->grid!=NULL ) {
                return SDL_TRUE;
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_GRID(): widget is not a grid\n" );
    return SDL_FALSE;
}

SDLGuiTK_Grid * SDLGuiTK_GRID( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_GRID(widget) ) return NULL;
    return widget->container->grid;
}


static int current_id=0;

static SDLGuiTK_Grid * Grid_create()
{
    SDLGuiTK_Grid * new_grid;

    new_grid = malloc( sizeof( struct SDLGuiTK_Grid ) );
    new_grid->container = PROT__container_new();
    new_grid->object = new_grid->container->object;
    new_grid->container->grid = new_grid;
    sprintf( new_grid->object->name, "grid%d", ++current_id );

    new_grid->column_homogeneous = SDL_FALSE;
    new_grid->column_spacing = 0;
    new_grid->row_homogeneous = SDL_FALSE;
    new_grid->row_spacing = 0;

    new_grid->column_homogeneous_size = 0;
    new_grid->row_homogeneous_size = 0;
    new_grid->children = SDLGuiTK_list_new();

    return new_grid;
}

static void Grid_destroy( SDLGuiTK_Grid * grid )
{
    SDLGuiTK_list_destroy( grid->children );

    PROT__container_destroy( grid->object->widget->container );
    free( grid );
}

static void Grid_SetSize( SDLGuiTK_Grid * grid )
{
    GridChild * current_child;
    grid->columns = 0;
    grid->rows = 0;
    grid->shown_nb = 0;
    current_child = (GridChild *) SDLGuiTK_list_ref_init( grid->children );
    while( current_child ) {
        if( grid->container->widget->visible && current_child->child->visible ) {
            //printf("Grid_SetSize %s (w:%d h:%d)\n",
                   //current_child->child->object->name,
                   //current_child->area.w, current_child->area.h);
            GridChild_DrawUpdate( current_child );
            if(grid->columns<(current_child->left+current_child->width))
                grid->columns = current_child->left+current_child->width;
            if(grid->rows<(current_child->top+current_child->height))
                grid->rows = current_child->top+current_child->height;
            grid->shown_nb++;
        }
        current_child = (GridChild *) SDLGuiTK_list_ref_next( grid->children );
    }
    grid->gx = calloc(grid->columns+1, sizeof(int));
    grid->gy = calloc(grid->rows+1, sizeof(int));
    int i=0;
    while(i<=grid->columns) {
        grid->gx[i++] = 0;
    }
    i=0;
    while(i<=grid->rows) {
        grid->gy[i++] = 0;
    }
}

static void Update_gx(SDLGuiTK_Grid * grid, int n, int value)
{
    int i;
    for(i=n; i<=grid->columns; i++)
        if(grid->gx[i]<value)
            grid->gx[i] = value;
}

static void Grid_Child_WidthCompute(SDLGuiTK_Grid * grid, GridChild * child, int n)
{
    int n2 = n + child->width;
    if((grid->gx[n2]-grid->gx[n])<(child->area.w+(2*grid->column_spacing))) {
        Update_gx(grid, n2, grid->gx[n]+child->area.w+(2*grid->column_spacing));
    }
}

static void Grid_Child_WidthHomogeneous(SDLGuiTK_Grid * grid)
{
    GridChild * current_child;
    int max_width = 0;

    for(int i=0; i<grid->columns; i++) {
        if((grid->gx[i+1]-grid->gx[i])>max_width)
           max_width = grid->gx[i+1]-grid->gx[i];
    }

    //printf("WidthHomogeneous:%d\n", max_width);

    for(int i=0; i<=grid->columns; i++) {
        grid->gx[i] = i * max_width;
    }

    int size, delta;
    int min_delta = 9999999;

    current_child = (GridChild *) SDLGuiTK_list_ref_init( grid->children );
    while( current_child ) {
        size = max_width*current_child->width - 2*grid->column_spacing;
        delta = (size-current_child->area.w) / grid->columns;
        //printf("S:%d D:%d\n", size, delta);
        if(delta<min_delta)
            min_delta = delta;
        current_child = (GridChild *) SDLGuiTK_list_ref_next( grid->children );
    }
    max_width -= min_delta;

    //printf("WidthHomogeneous OPTI:%d\n", max_width);

    /*int * Delta=calloc(grid->columns, sizeof(int));
    for(int i=0; i<grid->columns; i++) {
        Delta[i] =
    }*/
}

static void Update_gy(SDLGuiTK_Grid * grid, int n, int value)
{
    int i;
    for(i=n; i<=grid->rows; i++)
        if(grid->gy[i]<value)
            grid->gy[i] = value;
}

static void Grid_Child_HeightCompute(SDLGuiTK_Grid * grid, GridChild * child, int n)
{
    int n2 = n + child->height;
    if((grid->gy[n2]-grid->gy[n])<(child->area.h+(2*grid->row_spacing))) {
        Update_gy(grid, n2, grid->gy[n]+child->area.h+(2*grid->row_spacing));
    }
}

static void Grid_Child_HeightHomogeneous(SDLGuiTK_Grid * grid)
{
    GridChild * current_child;
    int max_height = 0;

    for(int i=0; i<grid->rows; i++) {
        if((grid->gy[i+1]-grid->gy[i])>max_height)
           max_height = grid->gy[i+1]-grid->gy[i];
    }

    //printf("HeightHomogeneous:%d\n", max_height);

    for(int i=0; i<=grid->rows; i++) {
        grid->gy[i] = i * max_height;
    }

    int size, delta;
    int min_delta = 9999999;

    current_child = (GridChild *) SDLGuiTK_list_ref_init( grid->children );
    while( current_child ) {
        size = max_height*current_child->height - 2*grid->row_spacing;
        delta = (size-current_child->area.h) / grid->rows;
        //printf("S:%d D:%d\n", size, delta);
        if(delta<min_delta)
            min_delta = delta;
        current_child = (GridChild *) SDLGuiTK_list_ref_next( grid->children );
    }
    max_height -= min_delta;

    //printf("HeightHomogeneous OPTI:%d\n", max_height);
}

static void * Grid_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Grid * grid=widget->container->grid;
    GridChild * current_child;

    Grid_SetSize( grid );

    //printf("TEST: columns:%d rows:%d\n", grid->columns, grid->rows);

    //Compute minimal widths
    int n=0;
    while(n<grid->columns) {
        current_child = (GridChild *) SDLGuiTK_list_ref_init( grid->children );
        while( current_child ) {
            if(current_child->left==n)
                Grid_Child_WidthCompute(grid, current_child, n);
            current_child = (GridChild *) SDLGuiTK_list_ref_next( grid->children );
        }
        n++;
    }

    int i;
    //for(i=0; i<=grid->columns; i++) {
    //    printf("gx[%d]=%d\n", i, grid->gx[i]);
    //}

    if(grid->column_homogeneous)
        Grid_Child_WidthHomogeneous(grid);

    //Compute minimal heigths
    n=0;
    while(n<grid->rows) {
        current_child = (GridChild *) SDLGuiTK_list_ref_init( grid->children );
        while( current_child ) {
            if(current_child->top==n)
                Grid_Child_HeightCompute(grid, current_child, n);
            current_child = (GridChild *) SDLGuiTK_list_ref_next( grid->children );
        }
        n++;
    }

    //for(i=0; i<=grid->rows; i++) {
    //    printf("gy[%d]=%d\n", i, grid->gy[i]);
    //}

    if(grid->row_homogeneous)
        Grid_Child_HeightHomogeneous(grid);

    //Suppress first and last spacings
    for(i=1; i<=grid->columns; i++) {
        if(grid->gx[i]>grid->column_spacing)
            grid->gx[i] -= grid->column_spacing;
    }
    grid->gx[grid->columns] -= grid->column_spacing;
    for(i=grid->columns-1; i>=0; i--) {
        if(grid->gx[i]>grid->gx[grid->columns])
            grid->gx[i] -= grid->column_spacing;
    }
    //for(i=0; i<=grid->columns; i++) {
    //    printf("Gx[%d]=%d\n", i, grid->gx[i]);
    //}
    for(i=1; i<=grid->rows; i++) {
        if(grid->gy[i]>grid->row_spacing)
            grid->gy[i] -= grid->row_spacing;
    }
    grid->gy[grid->rows] -= grid->row_spacing;
    for(i=grid->rows-1; i>=0; i--) {
        if(grid->gy[i]>grid->gy[grid->rows])
            grid->gy[i] -= grid->row_spacing;
    }
    //for(i=0; i<=grid->rows; i++) {
    //    printf("Gy[%d]=%d\n", i, grid->gy[i]);
    //}

    grid->container->children_area.w = grid->gx[grid->columns];
    grid->container->children_area.h = grid->gy[grid->rows];
    grid->base_width = grid->container->children_area.w;
    grid->base_height = grid->container->children_area.h;

    PROT__container_DrawUpdate( grid->container );

    return (void *) NULL;
}

static void Grid_UpdateChild(SDLGuiTK_Grid * grid, GridChild * child)
{
    child->area.x = grid->gx[child->left]+grid->container->border_width;
    if(child->left!=0)
        child->area.x += grid->column_spacing;
    child->area.y = grid->gy[child->top]+grid->container->border_width;
    if(child->top!=0)
        child->area.y += grid->row_spacing;
    child->area.w = grid->gx[child->left+child->width] - grid->gx[child->left]
        -2*grid->column_spacing;
    if(child->left==0)
        child->area.w += grid->column_spacing;
    if(child->left==(grid->columns-child->width))
        child->area.w += grid->column_spacing;
    child->area.h = grid->gy[child->top+child->height] - grid->gy[child->top]
        -2*grid->row_spacing;
    if(child->top==0)
        child->area.h += grid->row_spacing;
    if(child->top==(grid->rows-child->height))
        child->area.h += grid->row_spacing;
}

static void * Grid_DrawBlit( SDLGuiTK_Widget * widget )
{
     SDLGuiTK_Grid * grid=widget->container->grid;
    GridChild * current_child;
    //int current_x=grid->container->border_width;
    //int current_y=grid->container->border_width;
    int i;
    int expand = 0;

    if( grid->shown_nb<1 ) {
        PROT__container_DrawBlit( grid->container );
        SDLGUITK_ERROR( "Grid_DrawBlit(): grid->shown_nb<1\n" );
        return 0;
    }

    //Expand Grid size if necessary
    if(grid->base_width<(widget->req_area.w-2*grid->container->border_width)) {
        expand = ((widget->req_area.w-2*grid->container->border_width)-grid->base_width) /
            grid->columns;
        for(i=1; i<grid->columns; i++)
            grid->gx[i] += expand;
        grid->gx[grid->columns] = (widget->req_area.w-2*grid->container->border_width);
    }
    if(grid->base_height<(widget->req_area.h-2*grid->container->border_width)) {
        expand = ((widget->req_area.h-2*grid->container->border_width)-grid->base_height) /
            grid->rows;
        for(i=1; i<grid->rows; i++)
            grid->gy[i] += expand;
        grid->gy[grid->rows] = (widget->req_area.h-2*grid->container->border_width);
    }

    PROT__container_DrawBlit( grid->container );

    current_child = (GridChild *) SDLGuiTK_list_ref_init( grid->children );
    while( current_child ) {
        Grid_UpdateChild(grid, current_child);
        GridChild_DrawBlit (current_child);
        current_child = (GridChild *) SDLGuiTK_list_ref_next( grid->children );
    }

    widget->rel_area.w = widget->abs_area.w; //TODO ???
    widget->rel_area.h = widget->abs_area.h; //TODO ???

    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;

    return (void *) NULL;
}

static SDLGuiTK_Widget * Grid_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{
    SDLGuiTK_Grid * grid=widget->container->grid;
    GridChild * current_child;
    SDLGuiTK_Widget * child;
    SDLGuiTK_Widget * active;

    current_child = (GridChild *) SDLGuiTK_list_ref_init( grid->children );
    while( current_child!=NULL ) {
        child = current_child->child;
        if( child->visible ) {
            active = PROT__widget_is_entering( child, x, y );
            if( active!=NULL ) {
                SDLGuiTK_list_ref_reset( grid->children );
                //SDLGuiTK_list_unlock( grid->children );
                return active;
            }
        }
        current_child = (GridChild *) SDLGuiTK_list_ref_next( grid->children );
    }
    return NULL;

}

static void * Grid_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Grid * grid=widget->container->grid;
    SDLGuiTK_Widget * current;
    GridChild * child;

    child = (GridChild *) SDLGuiTK_list_ref_init( grid->children );
    while( child!=NULL ) {
        current = child->child;
        SDLGuiTK_widget_destroy( current );
        child = (GridChild *) SDLGuiTK_list_ref_next( grid->children );
    }
    return (void *) NULL;
}

static void * Grid_Free( SDLGuiTK_Widget * widget )
{
    Grid_destroy( widget->container->grid );

    return (void *) NULL;
}

static void Grid_Init_functions( SDLGuiTK_Grid * grid )
{

    grid->object->widget->RecursiveEntering = Grid_RecursiveEntering;
    grid->object->widget->RecursiveDestroy = Grid_RecursiveDestroy;
    grid->object->widget->Free = Grid_Free;

    grid->object->widget->DrawUpdate = Grid_DrawUpdate;
    grid->object->widget->DrawBlit = Grid_DrawBlit;
}

SDLGuiTK_Widget * SDLGuiTK_grid_new()
{
    SDLGuiTK_Grid * grid;

    grid = Grid_create();
    Grid_Init_functions( grid );
    PROT__signal_push( grid->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return grid->object->widget;
}

void SDLGuiTK_grid_attach ( SDLGuiTK_Grid *grid,
                            SDLGuiTK_Widget *child,
                            int left,
                            int top,
                            int width,
                            int height )
{
    GridChild * gchild;

    gchild = GridChild_create( child );
    gchild->left = left;
    gchild->top = top;
    gchild->width = width;
    gchild->height = height;

    child->parent = grid->object->widget;
    PROT__widget_set_top (child, grid->object->widget->top);

    SDLGuiTK_list_append( grid->children, (SDLGuiTK_Object *) gchild );
#if DEBUG_LEVEL > 1
    char tmplog[256];
    sprintf( tmplog, "__grid_attach():  %s in %s\n",
             child->object->name, grid->object->name );
    SDLGUITK_LOG( tmplog );
#endif
}


void PROT__grid_remove( SDLGuiTK_Grid *this_grid, SDLGuiTK_Widget *widget )
{
    GridChild * child;
#if DEBUG_LEVEL > 1
    char tmplog[256];
    sprintf( tmplog, "__grid_remove():  %s from %s\n",
             widget->object->name, this_grid->object->name );
    SDLGUITK_LOG( tmplog );
#endif

    if( (child=(GridChild *) SDLGuiTK_list_remove(this_grid->children,
                                                  (SDLGuiTK_Object *)widget)) ) {
        if( this_grid->object->widget->parent ) {
            PROT__signal_push( this_grid->object->widget->parent->object,
                               SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY );
        }
        widget->parent = NULL;
        PROT__widget_set_top (widget, NULL);
        GridChild_destroy( child );
    } else {
        SDLGUITK_ERROR( "PROT__grid_remove(): widget not found in grid\n" );
    }
}

void PROT__grid_set_top( SDLGuiTK_Grid *grid, SDLGuiTK_Widget *top)
{
    GridChild * child;
    child = (GridChild*) SDLGuiTK_list_ref_init (grid->children);
    while(child) {
        PROT__widget_set_top (child->child, top);
        child = (GridChild*) SDLGuiTK_list_ref_next (grid->children);
    }
}



void SDLGuiTK_grid_set_column_spacing (SDLGuiTK_Grid *grid,
                                       int column_spacing)
{
    grid->column_spacing = column_spacing;
}

int SDLGuiTK_grid_get_column_spacing (SDLGuiTK_Grid *grid)
{
    return grid->column_spacing;
}

void SDLGuiTK_grid_set_row_spacing (SDLGuiTK_Grid *grid,
                                    int row_spacing)
{
    grid->row_spacing = row_spacing;
}

int SDLGuiTK_grid_get_row_spacing (SDLGuiTK_Grid *grid)
{
    return grid->row_spacing;
}

void SDLGuiTK_grid_set_column_homogeneous (SDLGuiTK_Grid *grid,
                                           SDL_bool column_homogeneous)
{
    grid->column_homogeneous = column_homogeneous;
}

SDL_bool SDLGuiTK_grid_get_column_homogeneous (SDLGuiTK_Grid *grid)
{
    return grid->column_homogeneous;
}

void SDLGuiTK_grid_set_row_homogeneous (SDLGuiTK_Grid *grid,
                                        SDL_bool row_homogeneous)
{
    grid->row_homogeneous = row_homogeneous;
}

SDL_bool SDLGuiTK_grid_get_row_homogeneous (SDLGuiTK_Grid *grid)
{
    return grid->row_homogeneous;
}


