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

/* SDLGuiTK_Editable structure definition */
struct SDLGuiTK_Editable {
  SDLGuiTK_Object    * object;      /* referent widget */

  char * text;
  int cursor_position;
};

extern 
SDLGuiTK_Editable * PROT__editable_new( SDLGuiTK_Object * object );

extern 
void PROT__editable_destroy( SDLGuiTK_Editable * editable );

extern 
void PROT__editable_settext(  SDLGuiTK_Editable * editable, \
			                        const char *text );

extern
void PROT__editable_makeblended(  MySDL_Surface * render_text, \
					                        SDLGuiTK_Editable * editable, \
					                        int size, SDL_Color color );

extern
void PROT__editable_keysym( SDLGuiTK_Editable * editable, \
			   SDL_KeyboardEvent * kevent );
