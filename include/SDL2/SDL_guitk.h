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

#ifndef _SDLguitk_h
#define _SDLguitk_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#ifdef HAVE_GL_GL_H
#include <SDL2/SDL_opengl.h>
#endif

/* Down-level includes for GuiTK */
#include "SDL_guitk_context.h"
#include "SDL_guitk_main.h"
#include "SDL_guitk_object.h"
#include "SDL_guitk_list.h"

/* Abstract widgets */
#include "SDL_guitk_widget.h"
#include "SDL_guitk_signal.h"
#include "SDL_guitk_tooltips.h"

/* Interface widgets */
#include "SDL_guitk_editable.h"

/* User-level objects */
#include "SDL_guitk_misc.h"
#include "SDL_guitk_button.h"
#include "SDL_guitk_entry.h"
#include "SDL_guitk_label.h"
#include "SDL_guitk_container.h"
#include "SDL_guitk_bin.h"
#include "SDL_guitk_window.h"
#include "SDL_guitk_alignment.h"
#include "SDL_guitk_box.h"
#include "SDL_guitk_image.h"
#include "SDL_guitk_menu.h"
#include "SDL_guitk_panel.h"
#include "SDL_guitk_scrolledwindow.h"

/* #include "SDL_guitk_entry.h" */


/* Common includes for SDL */

#include <SDL2/begin_code.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif



/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif
#include <SDL2/close_code.h>

#endif /* _SDLguitk_h */


