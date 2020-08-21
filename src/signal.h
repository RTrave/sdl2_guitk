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

#pragma once

#include <SDL2/SDL_guitk.h>

typedef struct SDLGuiTK_SignalHandler SDLGuiTK_SignalHandler;
typedef struct SDLGuiTK_SignalFunction SDLGuiTK_SignalFunction;


/* SDLGuiTK_Signal types defines */
#define SDLGUITK_SIGNAL_TYPE_NONE                 ((int)  0)
// Object
#define SDLGUITK_SIGNAL_TYPE_REALIZE              ((int)  1)
#define SDLGUITK_SIGNAL_TYPE_DESTROY              ((int)  2)
// Widget
#define SDLGUITK_SIGNAL_TYPE_SHOW                 ((int) 10)
#define SDLGUITK_SIGNAL_TYPE_HIDE                 ((int) 11)
#define SDLGUITK_SIGNAL_TYPE_CHILDNOTIFY          ((int) 12)
#define SDLGUITK_SIGNAL_TYPE_KEYBOARD             ((int) 13)
#define SDLGUITK_SIGNAL_TYPE_MAP                  ((int) 14)
#define SDLGUITK_SIGNAL_TYPE_UNMAP                ((int) 15)
#define SDLGUITK_SIGNAL_TYPE_ENTER                ((int) 16)
#define SDLGUITK_SIGNAL_TYPE_LEAVE                ((int) 17)
#define SDLGUITK_SIGNAL_TYPE_PRESSED              ((int) 18)
#define SDLGUITK_SIGNAL_TYPE_RELEASED             ((int) 19)
// Button
#define SDLGUITK_SIGNAL_TYPE_CLICKED              ((int) 30)
// ToggleButton
#define SDLGUITK_SIGNAL_TYPE_TOGGLED              ((int) 31)
// RadioButton
#define SDLGUITK_SIGNAL_TYPE_GROUPCHANGED         ((int) 32)
// Menu
#define SDLGUITK_SIGNAL_TYPE_ACTIVATECURRENT      ((int) 40)
#define SDLGUITK_SIGNAL_TYPE_DEACTIVATE           ((int) 41)
#define SDLGUITK_SIGNAL_TYPE_SELECTIONDONE        ((int) 42)
// Entry
#define SDLGUITK_SIGNAL_TYPE_TEXTINPUT            ((int) 50)
#define SDLGUITK_SIGNAL_TYPE_CHANGED              ((int) 51)
// ??
#define SDLGUITK_SIGNAL_TYPE_VALUECHANGED         ((int) 52)

#define SDLGUITK_SIGNAL_TYPE_MAXID                ((int) 64)


/* SDLGuiTK_SignalHandler structure definition */
struct SDLGuiTK_SignalHandler {
    SDLGuiTK_Object * object;
    SDL_bool has_fdefault1[SDLGUITK_SIGNAL_TYPE_MAXID];
    SDLGuiTK_SignalFunction * fdefault1[SDLGUITK_SIGNAL_TYPE_MAXID];
    SDL_bool has_fdefault2[SDLGUITK_SIGNAL_TYPE_MAXID];
    SDLGuiTK_SignalFunction * fdefault2[SDLGUITK_SIGNAL_TYPE_MAXID];
    SDL_bool has_fexternal[SDLGUITK_SIGNAL_TYPE_MAXID];
    SDLGuiTK_SignalFunction * fexternal[SDLGUITK_SIGNAL_TYPE_MAXID];
};


/* SDLGuiTK_SignalFunction structure definition */
struct SDLGuiTK_SignalFunction {
    SDLGuiTK_Object * object; /* unused */
    void * (*function)( SDLGuiTK_Signal *, void * );
    void * data;
};


struct SDLGuiTK_Signal {
    SDLGuiTK_Object * object;   /* Signal receptor */
    int type;
    Uint8 state;                /* from SDL_KeyboardEvent */
    Uint8 repeat;               /* from SDL_KeyboardEvent */
    SDL_Keysym keysym;          /* from SDL_KeyboardEvent */
    char text[32];              /* from SDL_TextInputEvent */
};


/* INTERNAL USE (../context.c) */
extern void PROT__signal_init();
extern void PROT__signal_start();
extern void PROT__signal_stop();
extern void PROT__signal_uninit();
extern void PROT__signal_check();

extern
void PROT__signal_push( SDLGuiTK_Object *object, int type );
extern // for Key and TextInput
void PROT__signal_push_event( SDLGuiTK_Object *object, int type,
                              SDL_Event *event );

#define SDLGUITK_SIGNAL_LEVEL1         ((int)  1)
#define SDLGUITK_SIGNAL_LEVEL2         ((int)  2)
extern
void PROT_signal_connect( SDLGuiTK_Object * object,
                          int type,
                          void * (*fct)( SDLGuiTK_Signal *, void * ),
                          int signal_level );

/* Create a new initialized SDLGuiTK_Signal structure */
extern
SDLGuiTK_Object *PROT__signalhandler_new();
extern
void PROT__signalhandler_destroy( SDLGuiTK_Object *handler );


/* signal traductions */
// Object
#define SDLGUITK_SIGNAL_TEXT_REALIZE            ("realize")
#define SDLGUITK_SIGNAL_TEXT_DESTROY            ("destroy")
// Widget
#define SDLGUITK_SIGNAL_TEXT_SHOW               ("show")
#define SDLGUITK_SIGNAL_TEXT_HIDE               ("hide")
#define SDLGUITK_SIGNAL_TEXT_CHILDNOTIFY        ("child-notify")
#define SDLGUITK_SIGNAL_TEXT_KEYBOARD           ("key-press-event")
#define SDLGUITK_SIGNAL_TEXT_MAP                ("map")
#define SDLGUITK_SIGNAL_TEXT_UNMAP              ("unmap")
#define SDLGUITK_SIGNAL_TEXT_ENTER              ("enter-notify-event")
#define SDLGUITK_SIGNAL_TEXT_LEAVE              ("leave-notify-event")
#define SDLGUITK_SIGNAL_TEXT_PRESSED            ("button-press-event")
#define SDLGUITK_SIGNAL_TEXT_RELEASED           ("button-release-event")
// Button
#define SDLGUITK_SIGNAL_TEXT_CLICKED            ("clicked")
// ToggleButton
#define SDLGUITK_SIGNAL_TEXT_TOGGLED            ("toggled")
// RadioButton
#define SDLGUITK_SIGNAL_TEXT_GROUPCHANGED       ("group-changed")
// Menu
#define SDLGUITK_SIGNAL_TEXT_ACTIVATECURRENT    ("activate-current")
#define SDLGUITK_SIGNAL_TEXT_DEACTIVATE         ("deactivate")
#define SDLGUITK_SIGNAL_TEXT_SELECTIONDONE      ("selection-done")
// Entry
#define SDLGUITK_SIGNAL_TEXT_TEXTINPUT          ("text_input")
#define SDLGUITK_SIGNAL_TEXT_CHANGED            ("changed")
// ??
#define SDLGUITK_SIGNAL_TEXT_VALUECHANGED       ("value-changed")

