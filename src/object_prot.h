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


/* SDLGuiTK_Object structure definition */
struct SDLGuiTK_Object {

/*   SDL_mutex   * mutex; */

  SDLGuiTK_Widget * widget;
  SDLGuiTK_Tooltips * tooltips;

  int id;        /* Unique ID */
  char name[64]; /* Name of that object */

  /* Events handling defered to SDLGuiTK_SignalHandler object */
  SDLGuiTK_Object     * signalhandler;

};


extern void PROT__object_init();
extern void PROT__object_uninit();

extern
SDLGuiTK_Object * PROT__object_new();
extern
void              PROT__object_destroy( SDLGuiTK_Object * object );

