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

#ifndef _SDLguitk_widget_h
#define _SDLguitk_widget_h

//#include <SDL/SDL.h>
#include "SDL_guitk.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif



  /* SDLGuiTK_Widget structure definition */
  /* General common widget */
  typedef struct SDLGuiTK_Widget SDLGuiTK_Widget;

  /* Size requisition structure */
  typedef struct SDLGuiTK_Requisition SDLGuiTK_Requisition;
  struct SDLGuiTK_Requisition {
    int width;
    int height;
  };


  extern DECLSPEC
  SDLGuiTK_Object * SDLGuiTK_OBJECT( SDLGuiTK_Widget *widget );


  /*  */
  extern DECLSPEC
  void SDLGuiTK_widget_show( SDLGuiTK_Widget *widget );

  extern DECLSPEC
  void SDLGuiTK_widget_hide( SDLGuiTK_Widget *widget );

  extern DECLSPEC
  void SDLGuiTK_widget_destroy( SDLGuiTK_Widget *widget );

  extern DECLSPEC
  void SDLGuiTK_widget_set_size_request( SDLGuiTK_Widget *widget, \
					 int width, int height );

/*   extern DECLSPEC */
/*   void SDLGuiTK_widget_size_request( SDLGuiTK_Widget *widget, \ */
/* 				     SDLGuiTK_Requisition *requisition ); */



/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif /* _SDLguitk_widget_h */


