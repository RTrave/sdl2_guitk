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

#include "debug.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "signal.h"


static SDLGuiTK_List   * internal_signals=NULL;
static SDLGuiTK_List   * unused_signals=NULL;


static SDLGuiTK_Signal * Signal_create()
{
    SDLGuiTK_Signal * new_signal;

    SDLGuiTK_list_lock( unused_signals );
    new_signal = (SDLGuiTK_Signal *) SDLGuiTK_list_pop_head( unused_signals );
    if( new_signal==NULL ) {
        new_signal = malloc( sizeof( struct SDLGuiTK_Signal ) );
    }
    SDLGuiTK_list_unlock( unused_signals );
    return new_signal;
}

static void Signal_destroy( SDLGuiTK_Signal * signal )
{
    SDLGuiTK_list_lock( unused_signals );
    if( SDLGuiTK_list_length(unused_signals)<=20 ) {
        SDLGuiTK_list_append( unused_signals, (SDLGuiTK_Object *) signal );
    } else {
        free( signal );
    }
    SDLGuiTK_list_unlock( unused_signals );
}

static void Signal_push( SDLGuiTK_Signal * signal )
{
    SDLGuiTK_list_lock( internal_signals );
    SDLGuiTK_list_append( internal_signals, (SDLGuiTK_Object *) signal );
    SDLGuiTK_list_unlock( internal_signals );
}

static SDLGuiTK_Signal * Signal_check()
{
    SDLGuiTK_Signal * signal=NULL;

    SDLGuiTK_list_lock( internal_signals );
    signal = (SDLGuiTK_Signal *) SDLGuiTK_list_pop_head(internal_signals);
    SDLGuiTK_list_unlock( internal_signals );

    return signal;
}

static char * Signal_translate( int type )
{
    switch( type ) {
    case SDLGUITK_SIGNAL_TYPE_REALIZE:
        return SDLGUITK_SIGNAL_TEXT_REALIZE;
    case SDLGUITK_SIGNAL_TYPE_DESTROY:
        return SDLGUITK_SIGNAL_TEXT_DESTROY;
    case SDLGUITK_SIGNAL_TYPE_SHOW:
        return SDLGUITK_SIGNAL_TEXT_SHOW;
    case SDLGUITK_SIGNAL_TYPE_HIDE:
        return SDLGUITK_SIGNAL_TEXT_HIDE;
    case SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY:
        return SDLGUITK_SIGNAL_TEXT_CHILDNOTIFY;
    case SDLGUITK_SIGNAL_TYPE_KEYBOARD:
        return SDLGUITK_SIGNAL_TEXT_KEYBOARD;
    case SDLGUITK_SIGNAL_TYPE_MAP:
        return SDLGUITK_SIGNAL_TEXT_MAP;
    case SDLGUITK_SIGNAL_TYPE_UNMAP:
        return SDLGUITK_SIGNAL_TEXT_UNMAP;
    case SDLGUITK_SIGNAL_TYPE_ENTER:
        return SDLGUITK_SIGNAL_TEXT_ENTER;
    case SDLGUITK_SIGNAL_TYPE_LEAVE:
        return SDLGUITK_SIGNAL_TEXT_LEAVE;
    case SDLGUITK_SIGNAL_TYPE_PRESSED:
        return SDLGUITK_SIGNAL_TEXT_PRESSED;
    case SDLGUITK_SIGNAL_TYPE_RELEASED:
        return SDLGUITK_SIGNAL_TEXT_RELEASED;
    case SDLGUITK_SIGNAL_TYPE_CLICKED:
        return SDLGUITK_SIGNAL_TEXT_CLICKED;
    case SDLGUITK_SIGNAL_TYPE_TOGGLED:
        return SDLGUITK_SIGNAL_TEXT_TOGGLED;
    case SDLGUITK_SIGNAL_TYPE_ACTIVATECURRENT:
        return SDLGUITK_SIGNAL_TEXT_ACTIVATECURRENT;
    case SDLGUITK_SIGNAL_TYPE_DEACTIVATE:
        return SDLGUITK_SIGNAL_TEXT_DEACTIVATE;
    case SDLGUITK_SIGNAL_TYPE_SELECTIONDONE:
        return SDLGUITK_SIGNAL_TEXT_SELECTIONDONE;
    case SDLGUITK_SIGNAL_TYPE_TEXTINPUT:
        return SDLGUITK_SIGNAL_TEXT_TEXTINPUT;
    case SDLGUITK_SIGNAL_TYPE_CHANGED:
        return SDLGUITK_SIGNAL_TEXT_CHANGED;
    case SDLGUITK_SIGNAL_TYPE_VALUECHANGED:
        return SDLGUITK_SIGNAL_TEXT_VALUECHANGED;
    case SDLGUITK_SIGNAL_TYPE_GROUPCHANGED:
        return SDLGUITK_SIGNAL_TEXT_GROUPCHANGED;
    case SDLGUITK_SIGNAL_TYPE_NONE:
        return "none";
    default:
        break;
    }

    return "undef";
}


static SDLGuiTK_Signal * previous = NULL;

static void Signal_process( SDLGuiTK_Signal * signal )
{
    SDLGuiTK_SignalHandler * handler;
#if DEBUG_LEVEL >= 2
    char tmpstr[512];
    sprintf( tmpstr, "Signal \"%s\" on %s\n", \
             Signal_translate(signal->type), signal->object->name );
    SDLGUITK_LOG( tmpstr );
#endif

    if(previous) {
        if(previous->type==signal->type && previous->object==signal->object) {
            Signal_destroy (signal);
            return;
        } else {
            Signal_destroy (previous);
            previous = signal;
        }
    } else
        previous = signal;

    handler = (SDLGuiTK_SignalHandler *) signal->object->signalhandler;
    if(handler->has_fdefault1[signal->type])
        (*handler->fdefault1[signal->type]->function)(
            signal,
            NULL);
    if(handler->has_fdefault2[signal->type])
        (*handler->fdefault2[signal->type]->function)(
            signal,
            NULL);
    if(handler->has_fexternal[signal->type])
        (*handler->fexternal[signal->type]->function)(
            signal,
            handler->fexternal[signal->type]->data);
}

static int          signal_thread_flag=0;
static SDL_mutex  * signal_thread_mutex=NULL;


void PROT__signal_init()
{
    internal_signals = SDLGuiTK_list_new();
    unused_signals = SDLGuiTK_list_new();
    signal_thread_mutex = SDL_CreateMutex();
}

void PROT__signal_check()
{
    SDLGuiTK_Signal * signal;
    signal = Signal_check();
    while( signal!=NULL ) {
        Signal_process( signal );
        signal = Signal_check();
    }
    PROT__widget_destroypending();
}
void PROT__signal_start()
{
    //  signal_thread = SDL_CreateThread( Signal_thread, "SDLsign" );
}

void PROT__signal_stop()
{
    SDL_mutexP( signal_thread_mutex );
    if( signal_thread_flag==1 ) {
        signal_thread_flag = 2;
    } else {
        SDL_mutexV( signal_thread_mutex );
        return;
    }
    SDL_mutexV( signal_thread_mutex );

    SDL_mutexP( signal_thread_mutex );
    while( signal_thread_flag!=0 ) {
        SDL_mutexV( signal_thread_mutex );
        SDL_Delay( 200 );
        SDL_mutexP( signal_thread_mutex );
    }
    SDL_mutexV( signal_thread_mutex );
}

void PROT__signal_uninit()
{
    SDLGuiTK_Signal * signal;

    PROT__signal_stop();

    signal = (SDLGuiTK_Signal *) SDLGuiTK_list_pop_head( unused_signals );
    while( signal!=NULL ) {
        free( signal );
        signal = (SDLGuiTK_Signal *) SDLGuiTK_list_pop_head( unused_signals );
    }
    SDLGuiTK_list_destroy( internal_signals );
    SDLGuiTK_list_destroy( unused_signals );
    SDL_DestroyMutex( signal_thread_mutex );
}


static void * Function_notsetted( SDLGuiTK_Signal * signal, void * data )
{
    return (void *)NULL;
}

static SDLGuiTK_SignalFunction * SignalFunction_create()
{
    SDLGuiTK_SignalFunction * new_function;

    new_function = malloc( sizeof( struct SDLGuiTK_SignalFunction ) );
    new_function->function = Function_notsetted;
    new_function->data = NULL;
    return new_function;
}

static void SignalFunction_destroy( SDLGuiTK_SignalFunction * sigfct )
{
    free( sigfct );
}


SDLGuiTK_Object * PROT__signalhandler_new()
{
    int i;
    SDLGuiTK_SignalHandler * new_handler;

    new_handler = malloc( sizeof( struct SDLGuiTK_SignalHandler ) );
    for( i=0; i<SDLGUITK_SIGNAL_TYPE_MAXID; i++ ) {
        new_handler->has_fdefault1[i] = SDL_FALSE;
        new_handler->has_fdefault2[i] = SDL_FALSE;
        new_handler->has_fexternal[i] = SDL_FALSE;
    }

    return (SDLGuiTK_Object *) new_handler;
}

void PROT__signalhandler_destroy( SDLGuiTK_Object * handler )
{
    SDLGuiTK_SignalHandler * signalhandler;
    int i;

    signalhandler = (SDLGuiTK_SignalHandler *) handler;
    for( i=0; i<SDLGUITK_SIGNAL_TYPE_MAXID; i++ ) {
        if(signalhandler->has_fdefault1[i])
            SignalFunction_destroy( signalhandler->fdefault1[i] );
        if(signalhandler->has_fdefault2[i])
            SignalFunction_destroy( signalhandler->fdefault2[i] );
        if(signalhandler->has_fexternal[i])
            SignalFunction_destroy( signalhandler->fexternal[i] );
    }
    free( signalhandler );
}

void PROT__signal_push( SDLGuiTK_Object *object, int type )
{
    SDLGuiTK_Signal * signal;
#if DEBUG_LEVEL >= 2
    char tmpstr[512];
#endif

    signal = Signal_create();
    signal->type = type;
    signal->object = object;

#if DEBUG_LEVEL >= 2
    sprintf( tmpstr, "Signal_push \"%s\" on %s\n", \
             Signal_translate(signal->type), signal->object->name );
    SDLGUITK_LOG( tmpstr );
#endif

    Signal_push( signal );
}

void PROT__signal_push_event( SDLGuiTK_Object *object, int type,
                              SDL_Event *event )
{
    SDLGuiTK_Signal * signal;
#if DEBUG_LEVEL >= 2
    char tmpstr[512];
#endif

    signal = Signal_create();
    signal->type = type;
    signal->object = object;
    if(event->type==SDL_KEYDOWN || event->type==SDL_KEYUP) {
        signal->state = event->key.state;
        signal->repeat = event->key.repeat;
        signal->keysym = event->key.keysym;
    }
    else if(event->type==SDL_TEXTINPUT) {
        strcpy(signal->text, event->text.text);
    }

#if DEBUG_LEVEL >= 2
    sprintf( tmpstr, "Signal_push \"%s\" on %s\n", \
             Signal_translate(signal->type), signal->object->name );
    SDLGUITK_LOG( tmpstr );
#endif

    Signal_push( signal );
}


static int Signal_get_type( char *ctype )
{
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_REALIZE)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_REALIZE;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_DESTROY)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_DESTROY;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_SHOW)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_SHOW;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_HIDE)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_HIDE;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_CHILDNOTIFY)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_MAP)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_MAP;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_UNMAP)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_UNMAP;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_PRESSED)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_PRESSED;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_RELEASED)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_RELEASED;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_ENTER)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_ENTER;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_LEAVE)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_LEAVE;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_CLICKED)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_CLICKED;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_TOGGLED)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_TOGGLED;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_ACTIVATECURRENT)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_ACTIVATECURRENT;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_DEACTIVATE)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_DEACTIVATE;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_SELECTIONDONE)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_SELECTIONDONE;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_TEXTINPUT)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_TEXTINPUT;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_KEYBOARD)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_KEYBOARD;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_CHANGED)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_CHANGED;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_VALUECHANGED)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_VALUECHANGED;
    }
    if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_GROUPCHANGED)==0 ) {
        return SDLGUITK_SIGNAL_TYPE_GROUPCHANGED;
    }
    return 0;
}


void PROT_signal_connect( SDLGuiTK_Object * object,
                          int type,
                          void * (*fct)( SDLGuiTK_Signal *, void * ),
                          int signal_level )
{
    SDLGuiTK_SignalHandler * handler = (SDLGuiTK_SignalHandler *) object->signalhandler;

#if DEBUG_LEVEL >= 1
    char tmpstr[512];
    if(signal_level==SDLGUITK_SIGNAL_LEVEL1 && handler->has_fdefault1[type]) {
        sprintf( tmpstr, "Type \"%s\" for signal (level1) is set !\n",
                 Signal_translate ( type ));
        SDLGUITK_ERROR( tmpstr );
    }
    if(signal_level==SDLGUITK_SIGNAL_LEVEL2 && handler->has_fdefault2[type]) {
        sprintf( tmpstr, "Type \"%s\" for signal (level2) is set !\n",
                 Signal_translate ( type ));
        SDLGUITK_ERROR( tmpstr );
    }
#endif
    if(signal_level==SDLGUITK_SIGNAL_LEVEL1) {
        handler->fdefault1[type] = SignalFunction_create ();
        handler->fdefault1[type]->function = fct;
        handler->fdefault1[type]->data = 0;
        handler->has_fdefault1[type] = SDL_TRUE;
    }
    else if(signal_level==SDLGUITK_SIGNAL_LEVEL2) {
        handler->fdefault2[type] = SignalFunction_create ();
        handler->fdefault2[type]->function = fct;
        handler->fdefault2[type]->data = 0;
        handler->has_fdefault2[type] = SDL_TRUE;
    }
}


void SDLGuiTK_signal_connect( SDLGuiTK_Object * object, \
                              char * type, \
                              void * (*fct)( SDLGuiTK_Signal *, void * ), \
                              void * data )
{
    int inttype;
    SDLGuiTK_SignalHandler * handler;

    inttype = Signal_get_type(type);
#if DEBUG_LEVEL >= 1
    if( inttype==0 ) {
        char tmpstr[512];
        sprintf( tmpstr, "Type \"%s\" for signal not found !\n", type );
        SDLGUITK_ERROR( tmpstr );
    }
#endif
    handler = (SDLGuiTK_SignalHandler *) object->signalhandler;
    handler->fexternal[inttype] = SignalFunction_create ();
    handler->fexternal[inttype]->function = fct;
    handler->fexternal[inttype]->data = data;
    handler->has_fexternal[inttype] = SDL_TRUE;
}


SDLGuiTK_Object * SDLGuiTK_signal_get_object( SDLGuiTK_Signal *signal)
{
    return signal->object;
}

Uint8 SDLGuiTK_signal_get_state( SDLGuiTK_Signal *signal)
{
    if(signal->type!=SDLGUITK_SIGNAL_TYPE_KEYBOARD) {
        SDLGUITK_ERROR( "Signal is not KEYBOARD type\n" );
        return 0;
    }
    return signal->state;
}

Uint8 SDLGuiTK_signal_get_repeat( SDLGuiTK_Signal *signal)
{
    if(signal->type!=SDLGUITK_SIGNAL_TYPE_KEYBOARD) {
        SDLGUITK_ERROR( "Signal is not KEYBOARD type\n" );
        return 0;
    }
    return signal->repeat;
}

SDL_Keysym * SDLGuiTK_signal_get_keysym( SDLGuiTK_Signal *signal)
{
    if(signal->type!=SDLGUITK_SIGNAL_TYPE_KEYBOARD) {
        SDLGUITK_ERROR( "Signal is not KEYBOARD type\n" );
        return NULL;
    }
    return &signal->keysym;
}

char * SDLGuiTK_signal_get_text( SDLGuiTK_Signal *signal)
{
    if(signal->type!=SDLGUITK_SIGNAL_TYPE_TEXTINPUT) {
        SDLGUITK_ERROR( "Signal is not TEXTINPUT type\n" );
        return NULL;
    }
    return signal->text;
}


