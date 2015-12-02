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


typedef struct SDLGUITK_Video SDLGUITK_Video;

struct SDLGUITK_Video {
  int width, height;        /* public data */
  int bpp;                  /* public data */
  int fullscreen;           /* public data */
  int opengl_ask;           /* public data */

  Uint32 flags;             /* private data */
  //SDL_Surface * surface;    /* private data */
  SDL_Window * window;      /* private data */
  SDL_Renderer *renderer;   /* private data */
};

/* static SDLGUITK_Video * main_video=NULL; */

extern void MySDL_Init();
extern void MySDL_Uninit();

extern
void MySDL_MainSurface_set( SDL_Window * window, SDL_Renderer * renderer );
extern 
SDLGUITK_Video * MySDL_MainSurface_create();
extern
void             MySDL_MainSurface_destroy();
extern
void           * (*MySDL_MainSurface_clean)();
extern
void           * (*MySDL_SwapBuffers)();

extern
SDL_Window * MySDL_GetVideoWindow();
extern
SDL_Surface * MySDL_GetVideoSurface();


extern
SDL_Surface *MySDL_CreateRGBSurface( SDL_Surface *dst, int w, int h );
extern
SDL_Surface *MySDL_CreateRGBSurface_WithColor( SDL_Surface *dst, \
					       int w, int h, \
					       SDL_Color color );

extern
void         MySDL_FreeSurface( SDL_Surface *surface );

extern
SDL_Surface *MySDL_CopySurface( SDL_Surface *dst, SDL_Surface *src );
extern
SDL_Surface *MySDL_CopySurface_with_alpha( SDL_Surface *dst, \
					   SDL_Surface *src, \
					   Uint8 alpha );

/* extern Uint32 MySDL_MapRGBA( SDL_PixelFormat *fmt, SDL_Color color ); */


extern
void MySDL_ModeOpenGL( SDL_bool state );
extern
void MySDL_ModeFullScreen( SDL_bool state );
extern
void MySDL_ModeSetWidth( int width );
extern
void MySDL_ModeSetHeight( int height );


/* SDLGuiTK_2DWidget structure definition */
struct SDLGuiTK_Surface2D {
  SDLGuiTK_Object    * object;      /* referent widget */

  SDL_Surface * srf;                /* "private" data */
/*   int           texture_flag;       /\* "private" data *\/ */
  int           w, h;               /* "private" data */
#ifdef HAVE_GL_GL_H
  GLfloat   alpha;                  /* "private" data */
/*   GLfloat   alpha_flag;             /\* "private" data *\/ */
  GLuint    texture[1];             /* "private" data */
  GLfloat   texMinX, texMinY;       /* "private" data */
  GLfloat   texMaxX, texMaxY;       /* "private" data */
#endif
};

extern
SDLGuiTK_Surface2D * MySDL_surface2D_new();
extern
void MySDL_surface2D_destroy(     SDLGuiTK_Surface2D * surface2D );

extern
void MySDL_surface2D_update(      SDLGuiTK_Surface2D * surface2D, \
				  SDL_Surface *srf );
extern
void MySDL_surface2D_blitsurface( SDLGuiTK_Surface2D * surface2D, \
				  int x, int y );


#ifdef HAVE_GL_GL_H
extern
void MySDL_OpenGLEnable();  /* in SLAVE_MODE to init GL *functions */
extern
void MySDL_GL_Enter2DMode();
extern
void MySDL_GL_Leave2DMode();
extern
GLuint MySDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord, GLuint *tex);
#endif

extern 
void MySDL_UpdateWindowSurface();