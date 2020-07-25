/* 
   sdl_guitk - GUI toolkit designed for SDL environnements (GTK-style).

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



/* SDLGuiTK_Context modes */
#define SDLGUITK_CONTEXT_MODE_NULL  ( (int) 0 )
#define SDLGUITK_CONTEXT_MODE_SELF  ( (int) 1 )
#define SDLGUITK_CONTEXT_MODE_SLAVE ( (int) 2 )


typedef struct SDLGuiTK_Context SDLGuiTK_Context;

/* SDLGuiTK_Context structure definition */
struct SDLGuiTK_Context {
  SDLGuiTK_Object * object;

  SDL_mutex * mutex;

  /* parent SDL surface, informations and associate mutex */
  SDL_Surface *surface;
  int type;                 /* type of context */
  int w, h;                 /* size of window */
  int bpp;                  /* depth size */
  int opengl;               /* SDL setting: OpenGL enabled */
  int resizable;            /* SDL setting: resizing ability */

  /* Shared list of actives objects */
  /* They are added in fifo: ref->activables->unref */
  SDLGuiTK_List   * activables;  /* activables WMWidgets */
  SDLGuiTK_List   * ref;         /* WMWidget's Surface2D for creation */
  SDLGuiTK_List   * unref;       /* WMWidget's Surface2D for destruction */

  /* Current active window. */
  SDLGuiTK_WMWidget * active_wmwidget;
};


/* Current session context */
SDLGuiTK_Context * current_context;


/* Create and destroy context */
void PROT__context_new( SDL_Surface *surface, int type );

void PROT__context_quit();

void PROT__context_uninit();

/* Add and remove WMWidget in current_context->activables */
void PROT__context_ref_wmwidget( SDLGuiTK_WMWidget * wm_widget );
void PROT__context_unref_wmwidget( SDLGuiTK_WMWidget * wm_widget );


