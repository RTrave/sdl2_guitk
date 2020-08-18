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

#include "ctype.h"

#include <SDL2/SDL_guitk.h>

#include "../debug.h"
#include "../mysdl.h"
#include "../myttf.h"
#include "../theme_prot.h"


#include "../object_prot.h"
#include "../widget_prot.h"

#include "editable_prot.h"


static int current_id=0;

SDLGuiTK_Editable * PROT__editable_new( SDLGuiTK_Object * object )
{
    SDLGuiTK_Editable * new_editable;

    new_editable = malloc( sizeof( struct SDLGuiTK_Editable ) );
    new_editable->object = object;
    //sprintf( new_editable->object->name, "editable%d", ++current_id );
    new_editable->text = calloc( MyTTF_MAX_CHARS, sizeof( char ) );
    strcpy( new_editable->text, "" );
    new_editable->cursor_position = 0;

    return new_editable;
}

void PROT__editable_destroy( SDLGuiTK_Editable * editable )
{
    free( editable->text );
    free( editable );
}


void SDLGuiTK_editable_delete_text( SDLGuiTK_Editable *editable, \
                                    int start_pos, int end_pos )
{
    char *s, *t;
    char *result;
    int from_size, index;

    char *ts;
    from_size=unicode_strlen(editable->text,-1);

    if( start_pos<=-1 ) return;
    if( end_pos>from_size ) return;

    result = calloc( MyTTF_MAX_CHARS, sizeof(char) );
    s = editable->text;
    t = result;
    index = 0;
    while( *s ) {
        if( index<start_pos || index>=end_pos ) {
            ts = s;
            s = unicode_next_utf8( s );
            while( ts!=s ) {
                *t = *ts;
                ts++;
                t++;
            };
        } else {
            s = unicode_next_utf8( s );
        }
        index++;
    }
    *t = 0;
    free( editable->text );
    editable->text = result;
}


void SDLGuiTK_editable_insert_text( SDLGuiTK_Editable *editable, \
                                    const char *new_text, \
                                    int new_text_length, int *position )
{
    int from_index=0, result_index=0;
    char *result=calloc( MyTTF_MAX_CHARS, sizeof( char ) );
    int from_size, new_text_size, cursor_mvt=0;
    char *f, *nt;

    char *ts;

    from_size = unicode_strlen( editable->text, -1 );
    new_text_size = unicode_strlen( new_text, -1 );
    if( new_text_size==0 || new_text_length==0 ) return;
    if( new_text_length>=0 && new_text_length<new_text_size ) {
        new_text_size = new_text_length;
    }
    /*   ch = PROT_editable_getUTF8( keysym->unicode ); */
    f = editable->text;
    nt = (char *) new_text;

    while( *f ) {
        if( from_index==*position ) {
            while( new_text_size!=0 && *nt ) {
                ts = nt;
                nt = unicode_next_utf8( nt );
                while( ts!=nt ) {
                    result[result_index++] = *ts;
                    ts++;
                }
                cursor_mvt++;
                new_text_size--;
            }
        }
        ts = f;
        f = unicode_next_utf8( f );
        while( ts!=f ) {
            result[result_index++] = *ts;
            ts++;
        }
        from_index++;
    }
    if( *position==from_size ) {
        while( new_text_size!=0 && *nt ) {
            ts = nt;
            nt = unicode_next_utf8( nt );
            while( ts!=nt ) {
                result[result_index++] = *ts;
                ts++;
            }
            cursor_mvt++;
            new_text_size--;
        }
    }
    result[result_index] = 0;
    /*   printf( "A from_size=%d from_index=%d to_index=%d (%s)\n", from_size, from_index, to_index, result ); */

    free( editable->text );
    editable->text = &result[0];
    *position += cursor_mvt;

}


void PROT__editable_makeblended( MySDL_Surface * render_text, \
                                 SDLGuiTK_Editable * editable, \
                                 int size, SDL_Color color )
{
    char tmpstr[512];
    int text_lenght;
    int index=0, i=0;
    int current_pos=0;
    SDL_Surface * line_surf[3];
    SDL_Rect      line_area[3];
    SDLGuiTK_Theme * theme;
    char   text_displayed[512];
    char line_text[3][MyTTF_MAX_CHARS];
    char *s, *s_cursor;

    char *ts;

    strcpy( text_displayed, editable->text );
    strcat( text_displayed, " " );
    text_lenght = unicode_strlen( text_displayed, MyTTF_MAX_CHARS );
    s = text_displayed;
    /*   printf( "text_lenght=%d (%s) curs_pos=%d\n", text_lenght, text_displayed, editable->cursor_position); */

    if( render_text->srf!=NULL ) {
        MySDL_FreeSurface( render_text );
        //render_text = NULL;
    }

    if( editable->cursor_position>=text_lenght ) {
        SDLGUITK_ERROR( "MyTTF_Editable_MakeBlended(): cursor_pos>=text_lenght\n" );
        editable->cursor_position = text_lenght-1;
    } else if( editable->cursor_position<=-1 ) {
        SDLGUITK_ERROR( "MyTTF_Editable_MakeBlended(): cursor_pos<0\n" );
        editable->cursor_position = 0;
    }

    s_cursor = text_displayed;
    i=0;
    while(i!=editable->cursor_position) {
        s_cursor = unicode_next_utf8( s_cursor );
        i++;
    }

    if( editable->cursor_position>0 ) {
        while( s!=s_cursor ) {
            ts = s;
            s = unicode_next_utf8( s );
            while( ts!=s ) {
                line_text[0][current_pos++] = *ts;
                ts++;
            }

            index++;
        }
        line_text[0][current_pos] = 0;
        current_pos = 0;

        line_surf[0] = MyTTF_Render_Blended( NULL, \
                                             line_text[0], size, \
                                             MyTTF_STYLE_NORMAL, \
                                             color );
        sprintf( tmpstr, "MyTTF_Editable_MakeBlended(): part0 \"%s\"\n", \
                 line_text[0] );
        SDLGUITK_LOG( tmpstr );
        line_area[0].x = 0;
        line_area[0].y = 0;
        line_area[0].w = line_surf[0]->w;
        line_area[0].h = line_surf[0]->h;
    } else {
        line_surf[0] = NULL;
        line_area[0].x = 0;
        line_area[0].y = 0;
        line_area[0].w = 0;
        line_area[0].h = 0;
    }

    ts = s;
    s = unicode_next_utf8( s );
    while( ts!=s ) {
        line_text[1][current_pos++] = *ts;
        ts++;
    }
    line_text[1][current_pos++] = 0;
    index++;
    current_pos = 0;

    line_surf[1] = MyTTF_Render_Blended( NULL, \
                                         line_text[1], size, \
                                         MyTTF_STYLE_UNDERLINE, \
                                         color );
    sprintf( tmpstr, "MyTTF_Editable_MakeBlended(): part1 \"%s\"\n", \
             line_text[1] );
    SDLGUITK_LOG( tmpstr );
    line_area[1].x = line_area[0].w+1;
    line_area[1].y = 0;
    line_area[1].w = line_surf[1]->w;
    line_area[1].h = line_surf[1]->h;

    if( editable->cursor_position<text_lenght-1 ) {
        while( *s ) {
            ts = s;
            s = unicode_next_utf8( s );
            while( ts!=s ) {
                line_text[2][current_pos++] = *ts;
                ts++;
            }
            index++;
        }
        line_text[2][current_pos++] = 0;
        current_pos = 0;

        line_surf[2] = MyTTF_Render_Blended( NULL, \
                                             line_text[2], size, \
                                             MyTTF_STYLE_NORMAL, \
                                             color );
        sprintf( tmpstr, "MyTTF_Editable_MakeBlended(): part2 \"%s\"\n", \
                 line_text[2] );
        SDLGUITK_LOG( tmpstr );
        line_area[2].x = line_area[0].w+line_area[1].w+1;
        line_area[2].y = 0;
        line_area[2].w = line_surf[2]->w;
        line_area[2].h = line_surf[2]->h;
    } else {
        line_surf[2] = NULL;
        line_area[2].x = 0;
        line_area[2].y = 0;
        line_area[2].w = 0;
        line_area[2].h = 0;
    }
    theme = PROT__theme_get_and_lock();
    MySDL_CreateRGBSurface_WithColor( render_text, \
                                      line_area[0].w+line_area[1].w+line_area[2].w, \
                                      line_area[1].h, \
                                      theme->bgcolor );
    PROT__theme_unlock( theme );
    for( i=0; i<=2; i++ ) {
        if( line_surf[i]!=NULL ) {
            SDL_BlitSurface(  line_surf[i], NULL, \
                              render_text->srf, &line_area[i] );
            SDL_FreeSurface( line_surf[i] );
        }
    }

    return;
}


void PROT__editable_settext( SDLGuiTK_Editable * editable, \
                             const char *text )
{
    int text_lenght;

    text_lenght = unicode_strlen( text, -1 );

    free( editable->text );
    editable->text = calloc( MyTTF_MAX_CHARS, sizeof( char ) );
    strcpy( editable->text, text );
    editable->cursor_position = text_lenght;
}


char * SDLGuiTK_editable_get_chars( SDLGuiTK_Editable *editable, \
                                    int start_pos, int end_pos )
{
    int index=0;
    char *s, *ts, *r, *result;

    result = calloc( MyTTF_MAX_CHARS, sizeof( char ) );
    s = editable->text;
    r = result;

    if( end_pos<=start_pos ) end_pos=999;

    while( *s ) {
        if( index>=start_pos && index<end_pos ) {
            ts = s;
            s = unicode_next_utf8( s );
            while( ts!=s ) {
                *r = *ts;
                ts++;
                r++;
            }
        } else {
            s = unicode_next_utf8( s );
        }
        index++;
    }

    return result;
}
