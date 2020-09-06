/*
   SDL_guitk - GUI toolkit designed for SDL environnements.

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
#include "../mysdl.h"
#include "surface2d.h"
#include "../wmwidget.h"


void MyWM_Init();
void MyWM_Uninit();

// Check widget focus, or leave all focuses
void MyWM_WidgetFocus();
// Leave all focuses
void MyWM_WidgetUnFocus();
// Inhibition of mouse motion in MODE_MULTIPLES if mouse is not on focused
void MyWM_inhibit_mousemotion(SDL_bool is_inhibit);
// Check WMWidget current_context->focused
void MyWM_UpdateFocused();

// Mouse grab and Keyboard focus handling
void MyWM_set_keyboard_focus(SDLGuiTK_Widget * widget);
void MyWM_unset_keyboard_focus(SDLGuiTK_Widget * widget);
void MyWM_set_mouse_focus(SDLGuiTK_Widget * widget);
void MyWM_unset_mouse_focus(SDLGuiTK_Widget * widget);
void MyWM_start_textinput();
void MyWM_stop_textinput();

// Context callbacks for events
int MyWM_MOUSEMOTION( SDL_Event *event );
int MyWM_MOUSEBUTTONDOWN( SDL_Event *event );
int MyWM_MOUSEBUTTONUP( SDL_Event *event );
int MyWM_TEXTINPUT( SDL_Event *event );
int MyWM_KEYDOWN( SDL_Event *event );

// Blit a WMWidget on main surface
void * MyWM_blitsurface( SDLGuiTK_WMWidget * wm_widget );

