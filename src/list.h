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


typedef struct SDLGuiTK_List_Element SDLGuiTK_List_Element;

/* SDLGuiTK_List structure definition */
struct SDLGuiTK_List {
  SDLGuiTK_Object * object;

  SDL_mutex * mutex;

  int                     length;   /* number of elements in list */
  SDLGuiTK_List_Element * first;    /* first element */
  SDLGuiTK_List_Element * last;     /* last element */

  SDLGuiTK_List_Element * ref;      /* Referencement search method */
  SDLGuiTK_List_Element * refrv;    /* Referencement search method (reverse) */

  char name[16];                    /* Internal debug list name */
};


/* SDLGuiTK_List_Element structure definition */
struct SDLGuiTK_List_Element {
  SDLGuiTK_Object * object;

  SDLGuiTK_List_Element * next;     /* next element */
  SDLGuiTK_List_Element * previous; /* previous element */
};

extern void PROT_List_ref_reinit( SDLGuiTK_List * list );
extern void PROT_List_refrv_reinit( SDLGuiTK_List * list );
