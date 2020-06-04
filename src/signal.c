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
/* #include <SDL/SDL_thread.h> */

#include "debug.h"
#include "mysdl.h"
#include "object_prot.h"
#include "widget_prot.h"
#include "context_prot.h"
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
    if( signal->tievent!=NULL ) { free( signal->tievent ); };
    if( signal->kevent!=NULL ) { free( signal->kevent ); };
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

static char * Signal_translate( SDLGuiTK_Signal * signal )
{
  switch( signal->type ) {
  case SDLGUITK_SIGNAL_TYPE_REALIZE:
    return SDLGUITK_SIGNAL_TEXT_REALIZE;
  case SDLGUITK_SIGNAL_TYPE_DESTROY:
    return SDLGUITK_SIGNAL_TEXT_DESTROY;
  case SDLGUITK_SIGNAL_TYPE_SHOW:
    return SDLGUITK_SIGNAL_TEXT_SHOW;
  case SDLGUITK_SIGNAL_TYPE_HIDE:
    return SDLGUITK_SIGNAL_TEXT_HIDE;
  case SDLGUITK_SIGNAL_TYPE_FRAMEEVENT:
    return SDLGUITK_SIGNAL_TEXT_FRAMEEVENT;
  case SDLGUITK_SIGNAL_TYPE_KEYBOARD:
    return SDLGUITK_SIGNAL_TEXT_KEYBOARD;
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
  case SDLGUITK_SIGNAL_TYPE_ACTIVATE:
    return SDLGUITK_SIGNAL_TEXT_ACTIVATE;
  case SDLGUITK_SIGNAL_TYPE_DEACTIVATE:
    return SDLGUITK_SIGNAL_TEXT_DEACTIVATE;
  case SDLGUITK_SIGNAL_TYPE_SELECTIONDONE:
    return SDLGUITK_SIGNAL_TEXT_SELECTIONDONE;
  case SDLGUITK_SIGNAL_TYPE_TEXTINPUT:
    return SDLGUITK_SIGNAL_TEXT_TEXTINPUT;
  case SDLGUITK_SIGNAL_TYPE_NONE:
    return "none";
  default:
    break;
  }

  return "undef";
}


static void Signal_process( SDLGuiTK_Signal * signal )
{
#if DEBUG_LEVEL >= 2
  char tmpstr[512];
#endif
  SDLGuiTK_SignalHandler * handler;
/*   SDLGuiTK_Widget * parent; */

/*   SDL_mutexP( signal->object->mutex ); */
#if DEBUG_LEVEL >= 2
  sprintf( tmpstr, "Signal \"%s\" on %s\n", \
	   Signal_translate(signal), signal->object->name );
  SDLGUITK_LOG( tmpstr );
#endif
/*   parent = signal->widget->parent; */
/*   SDL_mutexP( signal->widget->mutex ); */

  handler = (SDLGuiTK_SignalHandler *) signal->object->signalhandler;
  if( signal->type==SDLGUITK_SIGNAL_TYPE_TEXTINPUT ) {
    (*handler->fdefault[signal->type]->function)( \
				signal->object->widget, \
				(void *) signal->tievent, \
        NULL );
  } else if( signal->type==SDLGUITK_SIGNAL_TYPE_KEYBOARD ) {
    (*handler->fdefault[signal->type]->function)( \
				signal->object->widget, \
				(void *) signal->kevent, \
        NULL );
  } else {
    (*handler->fdefault[signal->type]->function)( \
				signal->object->widget, \
				handler->fdefault[signal->type]->data, \
        NULL );
  }
  if( signal->type==SDLGUITK_SIGNAL_TYPE_TEXTINPUT ) {
    (*handler->fuserdef[signal->type]->function)( \
				signal->object->widget, \
				  handler->fuserdef[signal->type]->data, \
				(void *) signal->tievent );
  } else if( signal->type==SDLGUITK_SIGNAL_TYPE_KEYBOARD ) {
      (*handler->fuserdef[signal->type]->function)( \
				  signal->object->widget, \
				  handler->fuserdef[signal->type]->data, \
				  (void *) signal->kevent );
    } else {
    (*handler->fuserdef[signal->type]->function)( \
				  signal->object->widget, \
				  handler->fuserdef[signal->type]->data, \
          NULL );
    }
  /*   SDL_mutexV( signal->object->mutex ); */
/*   if( parent!=NULL ) { */
/*     if( signal->type==SDLGUITK_SIGNAL_TYPE_SHOW || \ */
/* 	signal->type==SDLGUITK_SIGNAL_TYPE_HIDE ) */
/*       { */
/* 	SDL_mutexP( parent->mutex ); */
/* 	(*parent->DrawUpdate)( parent ); */
/* 	(*parent->DrawBlit)( parent ); */
/* 	SDL_mutexV( parent->mutex ); */
/*       } */
/*   } */
  Signal_destroy( signal );
}

//static SDL_Thread * signal_thread;
static int          signal_thread_flag=0;
static SDL_mutex  * signal_thread_mutex=NULL;

/*
static int Signal_thread(void *data)
{
  
  SDLGuiTK_Signal * signal;

  SDLGUITK_THREAD_REF( "signals" );


  SDL_mutexP( signal_thread_mutex );
  signal_thread_flag = 1;
  while( signal_thread_flag==1 ) {
    SDL_mutexV( signal_thread_mutex );


    SDLGuiTK_threads_enter();
    signal = Signal_check();
    while( signal!=NULL ) {
      Signal_process( signal );
      signal = Signal_check();
    }

    PROT__widget_destroypending();
    SDLGuiTK_threads_leave();

    SDL_mutexP( signal_thread_mutex );
  }
  signal_thread_flag = 0;
  SDL_mutexV( signal_thread_mutex );

  SDLGUITK_THREAD_UNREF();
  return 0;
}
*/


void PROT__signal_init()
{
  internal_signals = SDLGuiTK_list_new();
  unused_signals = SDLGuiTK_list_new();
  signal_thread_mutex = SDL_CreateMutex();
/*   signal_thread = SDL_CreateThread( Signal_thread, "SDLsign" ); */
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


static void * Function_notsetted( SDLGuiTK_Widget * widget, void * data, void * event )
{
  return (void *)NULL;
}

static SDLGuiTK_SignalFunction * SignalFunction_create()
{
  SDLGuiTK_SignalFunction * new_function;

  new_function = malloc( sizeof( struct SDLGuiTK_SignalFunction ) );
  new_function->function = Function_notsetted;
  new_function->data = (void *) 0;
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
    new_handler->fdefault[i] = SignalFunction_create();
    new_handler->fuserdef[i] = SignalFunction_create();
  }

  return (SDLGuiTK_Object *) new_handler;
}

void PROT__signalhandler_destroy( SDLGuiTK_Object * handler )
{
  SDLGuiTK_SignalHandler * signalhandler;
  int i;

  signalhandler = (SDLGuiTK_SignalHandler *) handler;
  for( i=0; i<SDLGUITK_SIGNAL_TYPE_MAXID; i++ ) {
    SignalFunction_destroy( signalhandler->fdefault[i] );
    SignalFunction_destroy( signalhandler->fuserdef[i] );
  }
  free( signalhandler );
}

void PROT__signal_push( SDLGuiTK_Object * object, \
			int type )
{
  SDLGuiTK_Signal * signal;
#if DEBUG_LEVEL >= 2
  char tmpstr[512];
#endif

  signal = Signal_create();
  signal->type = type;
  signal->object = object;
  signal->tievent = NULL;
  signal->kevent = NULL;

#if DEBUG_LEVEL >= 2
  sprintf( tmpstr, "Signal_push \"%s\" on %s\n", \
	   Signal_translate(signal), signal->object->name );
  SDLGUITK_LOG( tmpstr );
#endif

  Signal_push( signal );
}


static SDL_TextInputEvent * TIevent_copy( SDL_TextInputEvent * tievent )
{
  SDL_TextInputEvent * tievent_c;

  tievent_c = malloc( sizeof( SDL_TextInputEvent ) );
  tievent_c->type = tievent->type;
  tievent_c->timestamp = tievent->timestamp;
  tievent_c->windowID = tievent->windowID;
  size_t destination_size = sizeof (tievent_c->text);
  snprintf(tievent_c->text, destination_size, "%s", tievent->text);
  return tievent_c;
}

void PROT__signal_textinput( SDLGuiTK_Object *object, int type,
			   SDL_TextInputEvent * tievent )
{
  SDLGuiTK_Signal * signal;
#if DEBUG_LEVEL >= 2
  char tmpstr[512];
#endif

  signal = Signal_create();
  signal->type = type;
  signal->object = object;
  //signal->kevent = Kevent_copy( kevent );
  signal->tievent = TIevent_copy( tievent );
  
#if DEBUG_LEVEL >= 2
  sprintf( tmpstr, "Signal_push \"%s\" on %s\n", \
	   Signal_translate(signal), signal->object->name );
  SDLGUITK_LOG( tmpstr );
#endif

  Signal_push( signal );
}

static SDL_KeyboardEvent * Kevent_copy( SDL_KeyboardEvent * kevent )
{
  SDL_KeyboardEvent * kevent_c;

  kevent_c = malloc( sizeof( SDL_KeyboardEvent ) );
  kevent_c->type = kevent->type;
  kevent_c->timestamp = kevent->timestamp;
  kevent_c->windowID = kevent->windowID;
  kevent_c->state = kevent->state;
  kevent_c->repeat = kevent->repeat;
  kevent_c->keysym = kevent->keysym;
  return kevent_c;
}

void PROT__signal_pushkey( SDLGuiTK_Object *object, int type,
			   SDL_KeyboardEvent * kevent )
{
  SDLGuiTK_Signal * signal;
#if DEBUG_LEVEL >= 2
  char tmpstr[512];
#endif

  signal = Signal_create();
  signal->type = type;
  signal->object = object;
  signal->kevent = Kevent_copy( kevent );

#if DEBUG_LEVEL >= 2
  sprintf( tmpstr, "Signal_push \"%s\" on %s\n", \
	   Signal_translate(signal), signal->object->name );
  SDLGUITK_LOG( tmpstr );
#endif

  Signal_push( signal );
}

/* void PROT__signal_directpush( SDLGuiTK_Widget * widget, \ */
/* 					 int type ) */
/* { */
/*   SDLGuiTK_Signal * signal; */

/*   signal = malloc( sizeof( SDLGuiTK_Signal ) ); */
/*   signal->type = type; */
/*   signal->widget = widget; */

/*   Signal_process( signal ); */
/* } */



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
  if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_FRAMEEVENT)==0 ) {
    return SDLGUITK_SIGNAL_TYPE_FRAMEEVENT;
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
  if( strcmp(ctype,SDLGUITK_SIGNAL_TEXT_ACTIVATE)==0 ) {
    return SDLGUITK_SIGNAL_TYPE_ACTIVATE;
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
  return 0;
}

void SDLGuiTK_signal_connect( SDLGuiTK_Object * object, \
			      char * type, \
			      void * (*fct)( SDLGuiTK_Widget *, void *, void* )  , \
			      void * data )
{
  char tmpstr[512];
  int inttype;
  SDLGuiTK_SignalHandler * handler;

  inttype = Signal_get_type(type);
  if( inttype==0 ) {
    sprintf( tmpstr, "Type \"%s\" for signal not found !\n", type );
    SDLGUITK_ERROR( tmpstr );
  }
/*   SDL_mutexP( object->mutex ); */
  handler = (SDLGuiTK_SignalHandler *) object->signalhandler;
  handler->fuserdef[inttype]->function = fct;
  handler->fuserdef[inttype]->data = data;
/*   SDL_mutexV( object->mutex ); */
}

