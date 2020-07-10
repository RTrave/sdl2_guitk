/*
   SDL_guitk - GUI toolkit designed for SDL environnements.

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


/* SDLGuiTK_ScrolledWindow structure definition */
struct SDLGuiTK_Adjustment {
    SDLGuiTK_Object * object;     /* referent object */

  /* "public" data */
    SDLGuiTK_Widget *parent[5];
    int parent_nb;
    double value;
    double lower;
    double upper;
    double step_increment;

  /* "private" data */
};

extern
void PROT__adjustment_attach(SDLGuiTK_Adjustment *adjustment,
                             SDLGuiTK_Widget *parent);

