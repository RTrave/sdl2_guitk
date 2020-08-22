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

#pragma once

#include <SDL2/SDL_guitk.h>
#include "../mysdl.h"


/* SDLGuiTK_SpinButton structure definition */
struct SDLGuiTK_SpinButton {
    SDLGuiTK_Object * object;           /* referent object */

    SDLGuiTK_Entry * entry;             /* herits from */

    /* "public" data */
    double value;
    double climb_rate;
    int digits;
    SDLGuiTK_Adjustment *adjustment;

    /* "private" data */
    SDL_bool        mouseOn;
    MySDL_Surface * controlD_srf;
    SDL_Rect        controlD_area;
    SDL_bool        controlD_flag;
    MySDL_Surface * controlU_srf;
    SDL_Rect        controlU_area;
    SDL_bool        controlU_flag;
    MySDL_Surface * active_srf;
    int             active_cflag;
    float           active_alpha_mod;

};

void PROT__spinbutton_DrawUpdate(SDLGuiTK_SpinButton * spinbutton);
void PROT__spinbutton_DrawBlit(SDLGuiTK_SpinButton * spinbutton);
void PROT__spinbutton_RecursiveEntering(SDLGuiTK_SpinButton * spinbutton,
                                        int x, int y);
void PROT__spinbutton_enter(SDLGuiTK_SpinButton * spinbutton);
void PROT__spinbutton_leave(SDLGuiTK_SpinButton * spinbutton);
void PROT__spinbutton_clicked(SDLGuiTK_SpinButton * spinbutton);
void PROT__spinbutton_pressed(SDLGuiTK_SpinButton * spinbutton);
void PROT__spinbutton_released(SDLGuiTK_SpinButton * spinbutton);
SDL_bool PROT__spinbutton_inputtext_verif(SDLGuiTK_SpinButton * spinbutton,
                                          char *text);
SDL_bool PROT__spinbutton_keyboard(SDLGuiTK_SpinButton * spinbutton, SDL_Keysym *keysym);
const char * PROT__spinbutton_set_text(SDLGuiTK_SpinButton * spinbutton,
                                       const char *str);

void PROT__spinbutton_destroy(SDLGuiTK_SpinButton * spinbutton);

