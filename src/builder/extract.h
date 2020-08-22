/*
   SDL_guitk - GUI toolkit designed for SDL environnements (GTK-style).

   Copyright (C) 2020 Trave Roman

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

#include <SDL2/SDL_guitk.h>
#include <libxml2/libxml/parser.h>


SDLGuiTK_Object * Extract_Adjustment(xmlNode * node);
SDLGuiTK_Widget * Extract_Scrollbar(xmlNode * node);
SDLGuiTK_Widget * Extract_Entry(xmlNode * node);
SDLGuiTK_Widget * Extract_SpinButton(xmlNode * node);
SDLGuiTK_Widget * Extract_Button(xmlNode * node);
SDLGuiTK_Widget * Extract_ToggleButton(xmlNode * node);
SDLGuiTK_Widget * Extract_CheckButton(xmlNode * node);
SDLGuiTK_Widget * Extract_RadioButton(xmlNode * node);
SDLGuiTK_Widget * Extract_MenuButton(xmlNode * node);
SDLGuiTK_Widget * Extract_Label(xmlNode * node);
SDLGuiTK_Widget * Extract_Image(xmlNode * node);

SDLGuiTK_Widget * Extract_Viewport(xmlNode * node);
SDLGuiTK_Widget * Extract_ScrolledWindow(xmlNode * node);
SDLGuiTK_Widget * Extract_MenuItem(xmlNode * node);
SDLGuiTK_Widget * Extract_Menu(xmlNode * node);
SDLGuiTK_Widget * Extract_Window(xmlNode * node);
SDLGuiTK_Widget * Extract_Box(xmlNode * node);


