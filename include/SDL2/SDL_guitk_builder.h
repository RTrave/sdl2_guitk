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

#ifndef _SDLguitk_builder_h
#define _SDLguitk_builder_h


/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


  /* SDLGuiTK_Builder structure definition */
  /* Contains all informations and object references for current session */
  typedef struct SDLGuiTK_Builder SDLGuiTK_Builder;


  /*  */
  extern DECLSPEC
  SDLGuiTK_Builder * SDLGuiTK_builder_new();

  /*  */
  extern DECLSPEC
  SDLGuiTK_Builder * SDLGuiTK_builder_new_from_file (const char *filename);


  /*  */
  extern DECLSPEC
  void SDLGuiTK_builder_destroy(SDLGuiTK_Builder * builder);

  /*  */
  extern DECLSPEC
  SDLGuiTK_Object * SDLGuiTK_builder_get_object (SDLGuiTK_Builder *builder,
                                                 const char *name);
  /*  */
  extern DECLSPEC
  SDLGuiTK_Widget * SDLGuiTK_builder_get_widget (SDLGuiTK_Builder *builder,
                                                 const char *name);

  /*  */
  extern DECLSPEC
  SDLGuiTK_List * SDLGuiTK_builder_get_objects (SDLGuiTK_Builder *builder);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_builder_h */


