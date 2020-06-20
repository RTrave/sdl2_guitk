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


#include <stdio.h>
#include <sys/types.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef STDC_HEADERS
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <SDL2/SDL_guitk.h>

#include "debug.h"
#include "mysdl.h"
#include "myttf.h"
#include "signal.h"
#include "object_prot.h"

#include "adjustment_prot.h"


SDLGuiTK_Adjustment * SDLGuiTK_adjustment_new(double value,
                                              double lower,
                                              double upper,
                                              double step_increment)
{
    SDLGuiTK_Adjustment * new_adjustment;

    new_adjustment = malloc( sizeof( struct SDLGuiTK_Adjustment ) );
    new_adjustment->object = PROT__object_new();

    new_adjustment->value = value;
    new_adjustment->lower = lower;
    new_adjustment->upper = upper;
    new_adjustment->step_increment = step_increment;

    return new_adjustment;
}

void SDLGuiTK_adjustment_set_value(SDLGuiTK_Adjustment *adjustment,
                                   double               value)
{
    adjustment->value = value;
}
