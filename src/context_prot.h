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

#pragma once

#include <SDL2/SDL_guitk.h>
#include "wmwidget.h"


/* SDLGuiTK_Context modes */
#define SDLGUITK_CONTEXT_MODE_NULL        ( (int) 0 )
#define SDLGUITK_CONTEXT_MODE_SELF        ( (int) 1 )
#define SDLGUITK_CONTEXT_MODE_MULTIPLE    ( (int) 2 )
#define SDLGUITK_CONTEXT_MODE_EMBED       ( (int) 3 )

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

  SDLGuiTK_Render   * main_render;      /* main render in SELF and EMBED modes */
  //SDLGuiTK_Render   * active_render;    /* current active render in MULTIPLE mode */
  SDLGuiTK_WMWidget * active_wmwidget;  /* current active WMWidget in MULTIPLE mode */
  /* Shared list of actives objects */
  /* They are added in fifo: ref->activables->unref */
  SDLGuiTK_List     * activables;  /* activables WMWidgets */
  SDLGuiTK_List     * ref;         /* WMWidget's push in render list */
  SDLGuiTK_List     * unref;       /* WMWidget's Surface2D for destruction */

  /* Current active window. */
  SDLGuiTK_WMWidget * focused;    /* handled by render/mywm.c */
};


/* Current session context */
SDLGuiTK_Context * current_context;


/* Create and destroy context */
void PROT__context_new( int type, SDL_Window * window, SDL_Renderer * renderer );

void PROT__context_quit();

void PROT__context_uninit();

/* Add and remove WMWidget in current_context->activables */
void PROT__context_ref_wmwidget( SDLGuiTK_WMWidget * wm_widget );
void PROT__context_unref_wmwidget( SDLGuiTK_WMWidget * wm_widget );

void PROT__context_ref_tooltip( SDLGuiTK_Widget * widget );
void PROT__context_unref_tooltip();

void PROT__context_renderclean();
void PROT__context_renderswap();

