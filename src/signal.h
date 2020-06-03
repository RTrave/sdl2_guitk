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


typedef struct SDLGuiTK_SignalHandler SDLGuiTK_SignalHandler;
typedef struct SDLGuiTK_SignalFunction SDLGuiTK_SignalFunction;


/* SDLGuiTK_Signal types defines */
#define SDLGUITK_SIGNAL_TYPE_NONE         ((int)  0)
#define SDLGUITK_SIGNAL_TYPE_REALIZE      ((int)  1)
#define SDLGUITK_SIGNAL_TYPE_DESTROY      ((int)  2)
#define SDLGUITK_SIGNAL_TYPE_SHOW         ((int)  3)
#define SDLGUITK_SIGNAL_TYPE_HIDE         ((int)  4)
#define SDLGUITK_SIGNAL_TYPE_FRAMEEVENT   ((int)  5)
#define SDLGUITK_SIGNAL_TYPE_KEYBOARD     ((int)  6)
#define SDLGUITK_SIGNAL_TYPE_ENTER        ((int) 10)
#define SDLGUITK_SIGNAL_TYPE_LEAVE        ((int) 11)
#define SDLGUITK_SIGNAL_TYPE_PRESSED      ((int) 20)
#define SDLGUITK_SIGNAL_TYPE_RELEASED     ((int) 21)
#define SDLGUITK_SIGNAL_TYPE_CLICKED      ((int) 22)
#define SDLGUITK_SIGNAL_TYPE_ACTIVATE      ((int) 30)
#define SDLGUITK_SIGNAL_TYPE_DEACTIVATE    ((int) 31)
#define SDLGUITK_SIGNAL_TYPE_SELECTIONDONE ((int) 32)
#define SDLGUITK_SIGNAL_TYPE_TEXTINPUT    ((int) 33)
#define SDLGUITK_SIGNAL_TYPE_MAXID        ((int) 64)


/* SDLGuiTK_SignalHandler structure definition */
struct SDLGuiTK_SignalHandler {
  SDLGuiTK_Object * object;
  SDLGuiTK_SignalFunction * fdefault[SDLGUITK_SIGNAL_TYPE_MAXID];
  SDLGuiTK_SignalFunction * fuserdef[SDLGUITK_SIGNAL_TYPE_MAXID];
};


/* SDLGuiTK_SignalFunction structure definition */
struct SDLGuiTK_SignalFunction {
  SDLGuiTK_Object * object; /* unused */
  void * (*function)( SDLGuiTK_Widget *, void *, void * );
  void * data;
};


struct SDLGuiTK_Signal {
  SDLGuiTK_Object * object;  /* Signal receptor */
  int type;
    SDL_KeyboardEvent * kevent;
    SDL_TextInputEvent * tievent;
};


/* INTERNAL USE (../context.c) */
extern void PROT__signal_init();
extern void PROT__signal_start();
extern void PROT__signal_stop();
extern void PROT__signal_uninit();
extern void PROT__signal_check();

extern
void PROT__signal_push( SDLGuiTK_Object *object, int type );

extern
void PROT__signal_textinput( SDLGuiTK_Object *object, int type,
			   SDL_TextInputEvent * tievent );
extern
void PROT__signal_pushkey( SDLGuiTK_Object *object, int type,
			   SDL_KeyboardEvent * kevent );

/* Create a new initialized SDLGuiTK_Signal structure */
extern
SDLGuiTK_Object *PROT__signalhandler_new();
extern
void PROT__signalhandler_destroy( SDLGuiTK_Object *handler );


/* /\* Use to unsafely call events: have to be called by signals thread *\/ */
/* extern void PROT__signal_directpush( SDLGuiTK_Widget * widget, \ */
/* 				     int type ); */


/* signal traductions */
#define SDLGUITK_SIGNAL_TEXT_REALIZE     ("realize")
#define SDLGUITK_SIGNAL_TEXT_DESTROY     ("destroy")
#define SDLGUITK_SIGNAL_TEXT_SHOW        ("show")
#define SDLGUITK_SIGNAL_TEXT_HIDE        ("hide")
#define SDLGUITK_SIGNAL_TEXT_FRAMEEVENT  ("frame_event")
#define SDLGUITK_SIGNAL_TEXT_KEYBOARD    ("keyboard")
#define SDLGUITK_SIGNAL_TEXT_ENTER       ("enter")
#define SDLGUITK_SIGNAL_TEXT_LEAVE       ("leave")
#define SDLGUITK_SIGNAL_TEXT_PRESSED     ("pressed")
#define SDLGUITK_SIGNAL_TEXT_RELEASED    ("released")
#define SDLGUITK_SIGNAL_TEXT_CLICKED     ("clicked")
#define SDLGUITK_SIGNAL_TEXT_ACTIVATE    ("activate")
#define SDLGUITK_SIGNAL_TEXT_DEACTIVATE  ("deactivate")
#define SDLGUITK_SIGNAL_TEXT_SELECTIONDONE ("selection_done")
#define SDLGUITK_SIGNAL_TEXT_TEXTINPUT   ("text_input")

