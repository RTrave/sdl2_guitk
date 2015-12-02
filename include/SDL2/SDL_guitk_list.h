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

#ifndef _SDLguitk_list_h
#define _SDLguitk_list_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"


/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_List structure definition */
  typedef struct SDLGuiTK_List SDLGuiTK_List;


  /* Allocate a new list */
  extern DECLSPEC
  SDLGuiTK_List *SDLGuiTK_list_new();
  extern DECLSPEC
  SDLGuiTK_List *SDLGuiTK_list_copy( SDLGuiTK_List *list );
  /* Free it */
  extern DECLSPEC
  void SDLGuiTK_list_destroy( SDLGuiTK_List *list );

  /* Lock and unlock it */
  extern DECLSPEC
  void SDLGuiTK_list_lock(   SDLGuiTK_List *list );
  extern DECLSPEC
  void SDLGuiTK_list_unlock( SDLGuiTK_List *list );

  /* Add a new element at end of list */
  extern DECLSPEC
  void SDLGuiTK_list_append(  SDLGuiTK_List *list, \
			      SDLGuiTK_Object *object );
  /* Add a new element at start of list */
  extern DECLSPEC
  void SDLGuiTK_list_prepend( SDLGuiTK_List *list, \
			      SDLGuiTK_Object *object );
  /* Remove and return first element */
  extern DECLSPEC
  SDLGuiTK_Object *SDLGuiTK_list_pop_head( SDLGuiTK_List *list );
  /* Remove and return last element */
  extern DECLSPEC
  SDLGuiTK_Object *SDLGuiTK_list_pop_tail( SDLGuiTK_List *list );

  /* Remove and return element if found */
  extern DECLSPEC
  SDLGuiTK_Object *SDLGuiTK_list_remove( SDLGuiTK_List *list, \
					 SDLGuiTK_Object *object );
  /* Clean the list of all elements */
  extern DECLSPEC
  void SDLGuiTK_list_remove_all( SDLGuiTK_List *list );


  /* Size of list (elements) */
  extern DECLSPEC
  Uint32 SDLGuiTK_list_length( SDLGuiTK_List *list );
  /* Return element if found */
  extern DECLSPEC
  SDLGuiTK_Object *SDLGuiTK_list_find( SDLGuiTK_List *list, \
				       SDLGuiTK_Object *object );

  /* Scan the List "elem by elem", with a internal List_Element ptr */
  /* First elem return by seq_refelem_init(), next by seq_refelem_next() */
  /* You have to be sure List is locked. */
  extern DECLSPEC
  SDLGuiTK_Object *SDLGuiTK_list_ref_init( SDLGuiTK_List *list );
  extern DECLSPEC
  SDLGuiTK_Object *SDLGuiTK_list_ref_next( SDLGuiTK_List *list );


  /* Same but from last to first */
  extern DECLSPEC
  SDLGuiTK_Object *SDLGuiTK_list_refrv_init( SDLGuiTK_List *list );
  extern DECLSPEC
  SDLGuiTK_Object *SDLGuiTK_list_refrv_next( SDLGuiTK_List *list );


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk__h */


