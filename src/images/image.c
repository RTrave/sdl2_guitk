/*
   SDL_guitk - GUI toolkit designed for SDL environnements.

   Copyright (C) 2005 Trave Roman

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

#ifdef HAVE_SDL_SDL_IMAGE_H
#include <SDL2/SDL_image.h>
#endif

#include "../debug.h"
#include "../mysdl.h"
#include "../signal.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../misc_prot.h"

#include "image_prot.h"


static SDL_bool SDLGuiTK_IS_IMAGE( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->misc!=NULL ) {
            if( widget->misc->image!=NULL ) {
                return SDL_TRUE;
            }
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_IMAGE(): widget is not an image\n" );
    return SDL_FALSE;
}

SDLGuiTK_Image * SDLGuiTK_IMAGE( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_IMAGE(widget) ) return NULL;
    return widget->misc->image;
}

static int current_id=0;


static SDLGuiTK_Image * Image_create()
{
    SDLGuiTK_Image * new_image;

    new_image = malloc( sizeof( struct SDLGuiTK_Image ) );
    new_image->misc = PROT__misc_new();
    new_image->misc->image = new_image;
    new_image->object = new_image->misc->object;
    sprintf( new_image->object->name, "image%d", ++current_id );

    new_image->file = calloc( 256, sizeof( char ) );
    strcpy( new_image->file, "" );
    new_image->srf = MySDL_Surface_new ("Image_srf");

    return new_image;
}

static void Image_destroy( SDLGuiTK_Image * image )
{
    MySDL_Surface_free( image->srf );
    PROT__misc_destroy( image->misc );
    free(image->file);
    free( image );
}


static void Image_make_surface( SDLGuiTK_Image * image )
{
#if DEBUG_LEVEL >= 1
    char tmpstr[256];
#endif

    if( image->srf->srf!=NULL ) {
        return; //TODO: handling image change
        MySDL_FreeSurface( image->srf );
        //image->srf->srf = NULL;
    }
#ifndef HAVE_SDL_SDL_IMAGE_H
    image->srf->srf = SDL_LoadBMP(image->file);
#else
    image->srf->srf = IMG_Load( image->file );
#endif
#if DEBUG_LEVEL >= 1
    if( image->srf->srf==NULL ) {
        sprintf( tmpstr, "Image_make_surface(): %s\n\t format unsupported!!!\n", \
                 image->file );
        SDLGUITK_ERROR( tmpstr );
    }
#endif
}

static void * Image_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Image * image=SDLGuiTK_IMAGE( widget );

    Image_make_surface( image );
    if( image->srf->srf==NULL ) {
        image->misc->area.w = 0;
        image->misc->area.h = 0;
    } else {
        image->misc->area.w = image->srf->srf->w;
        image->misc->area.h = image->srf->srf->h;
    }

    PROT__misc_DrawUpdate( image->misc );

    return (void *) NULL;
}


static void * Image_active_area( SDLGuiTK_Image * image )
{
    SDLGuiTK_Widget * widget=image->misc->widget;

    if( image->srf->srf==NULL ) return NULL;
    widget->act_area.x = \
                         widget->abs_area.x + image->misc->area.x;
    widget->act_area.y = \
                         widget->abs_area.y + image->misc->area.y;
    widget->act_area.w = image->srf->srf->w;
    widget->act_area.h = image->srf->srf->h;

    return (void *) NULL;
}

static void * Image_DrawBlit( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Image * image=SDLGuiTK_IMAGE(widget);

    PROT__misc_DrawBlit( image->misc );

    Image_active_area( image );

    //widget->rel_area.w = widget->abs_area.w;
    //widget->rel_area.h = widget->abs_area.h;

    if( widget->top!=NULL ) {
        MySDL_BlitSurface(  image->srf, NULL, \
                            widget->srf, &image->misc->area );
        //SDL_UpdateRect( widget->srf, 0, 0, 0, 0 );
    }

    return (void *) NULL;
}

static SDLGuiTK_Widget * Image_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{

    return NULL;
}


static void * Image_RecursiveDestroy( SDLGuiTK_Widget * widget )
{

    return (void *) NULL;
}

static void * Image_Free( SDLGuiTK_Widget * widget )
{
    Image_destroy( widget->misc->image );

    return (void *) NULL;
}

static void * Image_Realize( SDLGuiTK_Signal * signal, void * data )
{
    Image_make_surface( signal->object->widget->misc->image );
    return (void *) NULL;
}


static void Image_set_functions( SDLGuiTK_Image * image )
{
    SDLGuiTK_Widget * widget=image->object->widget;

    widget->RecursiveEntering = Image_RecursiveEntering;
    widget->RecursiveDestroy = Image_RecursiveDestroy;
    widget->Free = Image_Free;

    widget->DrawUpdate = Image_DrawUpdate;
    widget->DrawBlit = Image_DrawBlit;

     PROT_signal_connect(image->object, SDLGUITK_SIGNAL_TYPE_REALIZE,
                         Image_Realize, SDLGUITK_SIGNAL_LEVEL2);
}


SDLGuiTK_Widget *SDLGuiTK_image_new_from_file( const char * filename )
{
    SDLGuiTK_Image * image;

    image = Image_create();
    strcpy( image->file, filename );
    Image_set_functions( image );
    PROT__signal_push( image->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return image->object->widget;
}


