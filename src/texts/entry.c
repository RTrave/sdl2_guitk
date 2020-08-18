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

#include "../debug.h"
#include "../myttf.h"
#include "../mycursor.h"
#include "../signal.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../theme_prot.h"
#include "../render/mywm.h"

#include "editable_prot.h"
#include "entry_prot.h"



static SDL_bool SDLGuiTK_IS_ENTRY( SDLGuiTK_Widget * widget )
{
    if( widget!=NULL ) {
        if( widget->entry!=NULL ) {
            return SDL_TRUE;
        }
    }
    SDLGUITK_ERROR( "SDLGuiTK_IS_ENTRY(): widget is not an entry\n" );
    return SDL_FALSE;
}

SDLGuiTK_Entry * SDLGuiTK_ENTRY( SDLGuiTK_Widget * widget )
{
    if( !SDLGuiTK_IS_ENTRY(widget) ) return NULL;
    return widget->entry;
}


static int current_id=0;


static SDLGuiTK_Entry * Entry_create()
{
    SDLGuiTK_Entry * new_entry;

    new_entry = malloc( sizeof( struct SDLGuiTK_Entry ) );
    new_entry->widget = PROT__widget_new();
    new_entry->widget->entry = new_entry;
    new_entry->object = new_entry->widget->object;
    sprintf( new_entry->object->name, "entry%d", ++current_id );

    new_entry->editable = PROT__editable_new( new_entry->object );

    new_entry->is_editable = 1;
    new_entry->max_length = 1;

    new_entry->srf = MySDL_Surface_new ("Entry_srf");
    new_entry->text_flag = 1;
    new_entry->view_area.x = 0;
    new_entry->view_area.y = 0;
    new_entry->view_area.w = 0;
    new_entry->view_area.h = 0;

    return new_entry;
}

static void Entry_destroy( SDLGuiTK_Entry * entry )
{
    MySDL_Surface_free( entry->srf );

    PROT__editable_destroy( entry->editable );
    PROT__widget_destroy( entry->widget );
    free( entry );
}


static void Entry_make_surface( SDLGuiTK_Entry * entry )
{
    SDLGuiTK_Theme * theme;

    if(entry->text_flag==0) return;
    theme = PROT__theme_get_and_lock();
    /*   strcat( entry->text, "/2588" ); */
    /*   strcat( entry->text, "\u2588" ); */
    PROT__editable_makeblended( entry->srf, \
                                entry->editable, \
                                16, theme->ftcolor );
    PROT__theme_unlock( theme );

    entry->text_flag = 0;
}


static void * Entry_DrawUpdate( SDLGuiTK_Widget * widget )
{
    SDLGuiTK_Entry * entry=widget->entry;

    Entry_make_surface( entry );

    PROT__widget_set_req_area(widget, 100, 22);
    //entry->widget->req_area.w = entry->srf->srf->w;
    //entry->widget->req_area.h = entry->srf->srf->h;

    PROT__widget_DrawUpdate( entry->widget );

    return (void *) NULL;
}


static void Entry_active_area( SDLGuiTK_Entry * entry )
{
    SDLGuiTK_Widget * widget=entry->widget;

    widget->act_area.x = widget->abs_area.x;
    widget->act_area.y = widget->abs_area.y;
    widget->act_area.w = widget->abs_area.w;
    widget->act_area.h = widget->abs_area.h;
}

static void * Entry_DrawBlit( SDLGuiTK_Widget * widget )
{
    int wdiff=0; // hdiff=0;
    float cursor_ratio=1;
    //int xdiff=0, ydiff=0;
    SDL_Rect fg_area;
    SDLGuiTK_Entry * entry=widget->entry;
    SDLGuiTK_Editable * editable=entry->editable;
    Uint32 bgcolor;
    SDLGuiTK_Theme * theme;

    PROT__widget_DrawBlit( entry->widget );

    Entry_active_area( entry );

    fg_area.x = 2;
    fg_area.y = 2;
    fg_area.w = widget->abs_area.w-4;
    fg_area.h = widget->abs_area.h-4;
    theme = PROT__theme_get_and_lock();
#if DEBUG_LEVEL >= 3
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, 0x00, 0xff, 0x00, 0xff );
#else
    bgcolor = SDL_MapRGBA( widget->srf->srf->format, \
                           theme->bgcolor.r, \
                           theme->bgcolor.g, \
                           theme->bgcolor.b, \
                           255 );
#endif
    PROT__theme_unlock( theme );
    MySDL_FillRect( widget->srf, &fg_area, bgcolor );
    wdiff = entry->srf->srf->w - widget->abs_area.w +4;
    //hdiff = entry->srf->srf->h - widget->abs_area.h;
    if(wdiff>0) {
        cursor_ratio = ((float)editable->cursor_position /
                        (float)unicode_strlen(editable->text,-1));
        entry->view_area.x = wdiff*cursor_ratio;
        entry->view_area.y = 0;
        entry->view_area.w = widget->abs_area.w-4;
        entry->view_area.h = entry->srf->srf->h;
        MySDL_BlitSurface(  entry->srf, &entry->view_area, \
                            widget->srf, &fg_area );
    } else {
        MySDL_BlitSurface(  entry->srf, NULL, \
                            widget->srf, &fg_area );
    }

    return (void *) NULL;
}

static void * Entry_Realize( SDLGuiTK_Widget * widget, \
                             void * data, void * event )
{
    if( widget->entry->text_flag!=0 ) {
        Entry_make_surface( widget->entry );
    }
    /*   widget->changed = 0; */

    return (void *) NULL;
}

static void * Entry_Show( SDLGuiTK_Widget * widget, \
                          void * data, void * event )
{
    /*   widget->changed = 1; */
/*
    widget->shown = 1;
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }
*/
    return (void *) NULL;
}

static void * Entry_Hide( SDLGuiTK_Widget * widget, \
                          void * data, void * event )
{
    /*   widget->changed = 1; */
/*
    widget->shown = 0;
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }
*/
    MyWM_unset_keyboard_focus (widget);
    return (void *) NULL;
}

static SDLGuiTK_Widget * Entry_RecursiveEntering( SDLGuiTK_Widget * widget, \
        int x, int y )
{
    /*   printf( "cursor load RecursiveEntering\n" ); */
    /*   MyCursor_Set( SDLGUITK_CURSOR_TEXT ); */
    return NULL;
}

static void * Entry_RecursiveDestroy( SDLGuiTK_Widget * widget )
{
    return (void *) NULL;
}

static void * Entry_Free( SDLGuiTK_Widget * widget )
{
    Entry_destroy( widget->entry );

    return (void *) NULL;
}

static SDL_Surface * srf_act1 = NULL;
static SDL_Surface * srf_act2 = NULL;
static int i=0, idef=0;


static int Entry_UpdateActive( SDLGuiTK_Widget * widget )
{
    /*   int iA=1; */

    if( i==30 ) {
        i=0;
    }
    else {
        i++;
        return 0;
    };

    if( idef!=0 ) {
        idef = 0;
        widget->act_srf->srf = srf_act2;
        /*     widget->top->container->bin->window->wm_widget->active_2D->texture_flag = 1; */
    } else {
        idef = 1;
        widget->act_srf->srf = srf_act1;
        /*     widget->top->container->bin->window->wm_widget->active_2D->texture_flag = 1; */
    }

    return 1;
}

static void * Entry_MouseEnter( SDLGuiTK_Widget * widget, \
                                void * data, void * event )
{
    MyCursor_Set( SDLGUITK_CURSOR_TEXT );
    //Enable text input
    MyWM_start_textinput();

    return (void *) NULL;
}

static void * Entry_MouseLeave( SDLGuiTK_Widget * widget, \
                                void * data, void * event )
{
    //Disable text input
    MyWM_stop_textinput ();
    MyCursor_Set( SDLGUITK_CURSOR_DEFAULT );

    return (void *) NULL;
}

void Push_textinput( SDLGuiTK_Entry * entry, SDL_TextInputEvent * tievent )
{
    SDLGuiTK_Editable *editable=entry->editable;
#ifdef HAVE_UNICODE_H
    Uint16 unich[3];
#endif

    SDLGuiTK_editable_insert_text( editable, \
                                   tievent->text, \
                                   -1,
                                   &editable->cursor_position );
}

void Push_keysym( SDLGuiTK_Entry * entry, SDL_KeyboardEvent * kevent )
{
    SDLGuiTK_Editable *editable=entry->editable;
#ifdef HAVE_UNICODE_H
    Uint16 unich[3];
#endif
    //char *tch;
    //char tch;

    switch( kevent->keysym.sym ) {
    case SDLK_RIGHT:
        editable->cursor_position++;
        break;
    case SDLK_LEFT:
        editable->cursor_position--;
        break;
    case SDLK_BACKSPACE:
        SDLGuiTK_editable_delete_text( editable, \
                                       editable->cursor_position-1, \
                                       editable->cursor_position );
        editable->cursor_position--;
        break;
    case SDLK_DELETE:
        SDLGuiTK_editable_delete_text( editable, \
                                       editable->cursor_position, \
                                       editable->cursor_position+1 );
        break;
    case SDLK_RETURN:
        break;
    case SDLK_TAB:
        break;
    case SDLK_KP_ENTER:
        break;
    default:
        break;
    }
}

static void * Entry_TextInput( SDLGuiTK_Widget * widget, \
                               void * data, void * event )
{
    SDLGuiTK_Entry * entry=widget->entry;
    char tmpstr[128];
    SDL_TextInputEvent * tievent=(SDL_TextInputEvent *) data;

    sprintf( tmpstr, "Entry_TextInput(): %s\n", tievent->text );
    SDLGUITK_LOG(tmpstr);

    Push_textinput( entry, tievent );
    entry->text_flag = 1;
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }
    return (void *) NULL;
}

static void * Entry_Keyboard( SDLGuiTK_Widget * widget, \
                              void * data, void * event )
{
    SDLGuiTK_Entry * entry=widget->entry;
    char tmpstr[128];
    SDL_KeyboardEvent * kevent=(SDL_KeyboardEvent *) data;

    sprintf( tmpstr, "Entry_Keyboard(): %s\n", SDL_GetKeyName(kevent->keysym.sym) );
    SDLGUITK_LOG(tmpstr);

    Push_keysym( entry, kevent );
    entry->text_flag = 1;
    if( widget->top!=NULL ) {
        PROT__signal_push( widget->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
    }
    return (void *) NULL;
}

static void * Entry_MousePressed( SDLGuiTK_Widget * widget, \
                             void * data, void * event )
{
    MyWM_set_keyboard_focus (widget);

    return (void *) NULL;
}

static void Entry_set_functions( SDLGuiTK_Entry * entry )
{
    SDLGuiTK_SignalHandler * handler;

    handler = (SDLGuiTK_SignalHandler *) entry->object->signalhandler;

    entry->object->widget->RecursiveEntering = Entry_RecursiveEntering;
    entry->object->widget->RecursiveDestroy = Entry_RecursiveDestroy;
    entry->object->widget->Free = Entry_Free;
    entry->object->widget->UpdateActive = Entry_UpdateActive;

    entry->object->widget->DrawUpdate = Entry_DrawUpdate;
    entry->object->widget->DrawBlit = Entry_DrawBlit;

    handler->fdefault[SDLGUITK_SIGNAL_TYPE_REALIZE]->function = \
            Entry_Realize;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_SHOW]->function = \
            Entry_Show;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_HIDE]->function = \
            Entry_Hide;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_ENTER]->function = \
            Entry_MouseEnter;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_LEAVE]->function = \
            Entry_MouseLeave;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_TEXTINPUT]->function = \
            Entry_TextInput;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_KEYBOARD]->function = \
            Entry_Keyboard;
    handler->fdefault[SDLGUITK_SIGNAL_TYPE_PRESSED]->function = \
            Entry_MousePressed;
}



SDLGuiTK_Widget * SDLGuiTK_entry_new( const char *str )
{
    SDLGuiTK_Entry * entry;

    entry = Entry_create();
    SDLGuiTK_editable_insert_text( entry->editable, \
                                   str, \
                                   -1, \
                                   &entry->editable->cursor_position );
    Entry_set_functions( entry );
    /*   entry->object->widget->changed = 1; */
    PROT__signal_push( entry->object, SDLGUITK_SIGNAL_TYPE_REALIZE );

    return entry->object->widget;
}

void SDLGuiTK_entry_set_text( SDLGuiTK_Widget * entry, const char *str )
{
    PROT__editable_settext( entry->entry->editable, str );

    PROT__signal_push( entry->top->object, SDLGUITK_SIGNAL_TYPE_FRAMEEVENT );
}


char * SDLGuiTK_entry_get_text( SDLGuiTK_Widget *entry )
{
    char *text;
    text = calloc( MyTTF_MAX_CHARS, sizeof( char ) );
    strcpy( text, entry->entry->editable->text );
    return text;
}
