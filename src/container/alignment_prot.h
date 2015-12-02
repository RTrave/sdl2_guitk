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


/* SDLGuiTK_Alignment structure definition */
struct SDLGuiTK_Alignment {
  SDLGuiTK_Object * object;      /* referent object */

  SDLGuiTK_Bin * bin;            /* herits from */

  float xalign;                  /* "public" data */
  float yalign;                  /* "public" data */
  float xscale;                  /* "public" data */
  float yscale;                  /* "public" data */
  float top_padding;             /* "public" data */
  float bottom_padding;          /* "public" data */
  float left_padding;            /* "public" data */
  float right_padding;           /* "public" data */

};


/* extern */
/* void PROT__alignment_destroy( SDLGuiTK_Alignment *alignment ); */

/* extern */
/* void PROT__alignment_Draw( SDLGuiTK_Alignment *alignment ); */

