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


/* SDLGuiTK_WMWidget structure definition */
struct SDLGuiTK_WMWidget {
  SDLGuiTK_Object    * object;      /* referent widget */

  SDLGuiTK_Widget * widget;         /* herits from */

  /* "public" data */
  int               border_width;
  int               title_shown;
  char              title[256];

  /* "private" data */
  int                 enter;
  int                 moving;
  MySDL_Surface     * srf;
  SDL_Rect            area;
  SDL_Rect            child_area;     /* (w,h) setted in DrawUpdate() window,menushell,..
                                     * (x,y) setted in self DrawUpdate*/
  SDLGuiTK_Surface2D * surface2D;
  int                  surface2D_flag;
};


extern void MyWM_Init();
extern void MyWM_Uninit();

extern SDLGuiTK_WMWidget * MyWM_WMWidget_New( SDLGuiTK_Widget * widget );
extern void              MyWM_WMWidget_Delete( SDLGuiTK_WMWidget * wm_widget );

extern void MyWM_WMWidget_DrawUpdate( SDLGuiTK_WMWidget * wm_widget );
extern void MyWM_WMWidget_DrawBlit( SDLGuiTK_WMWidget * wm_widget );

extern int MyWM_push_MOUSEMOTION( SDL_Event *event );
extern int MyWM_push_MOUSEBUTTONDOWN( SDL_Event *event );
extern int MyWM_push_MOUSEBUTTONUP( SDL_Event *event );
extern int MyWM_push_TEXTINPUT( SDL_Event *event );
extern int MyWM_push_KEYDOWN( SDL_Event *event );


/* extern void MyWM_ref_wmwidget( SDLGuiTK_WMWidget * wm_widget ); */
/* extern void MyWM_unref_wmwidget( SDLGuiTK_WMWidget * wm_widget ); */

extern void * MyWM_blitsurface( SDLGuiTK_WMWidget * wm_widget );

//extern void MyWM_blitsurface_setopengl( int flag );

/* extern void MyWM_append_activable( SDLGuiTK_Widget * widget ); */

/* extern void PROT_MyWM_checkfornew( SDLGuiTK_Widget * widget ); */
extern void PROT_MyWM_leaveall();
extern void PROT_MyWM_checkactive( SDLGuiTK_Widget * widget );
