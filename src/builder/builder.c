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

#include <libxml2/libxml/parser.h>
#include <SDL2/SDL_guitk.h>

#include "../debug.h"
#include "../mysdl.h"
#include "../myttf.h"
#include "../signal.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../adjustment_prot.h"
#include "builder.h"
#include "extract.h"


static SDLGuiTK_Builder * Builder_create()
{
    SDLGuiTK_Builder * new_builder;

    new_builder = malloc( sizeof( struct SDLGuiTK_Builder ) );
    new_builder->object = PROT__object_new();

    new_builder->top_widget = NULL;
    new_builder->widgets = SDLGuiTK_list_new();

    return new_builder;
}

static void Builder_destroy( SDLGuiTK_Builder * builder )
{
    SDLGuiTK_list_destroy (builder->widgets);
    free( builder );
}

static SDLGuiTK_Builder * current_builder;

SDLGuiTK_Widget * Extract_object(xmlNode * node)
{
    SDLGuiTK_Widget * widget = NULL;
    if(propcmp(node, "class", "GtkMenu"))
        widget = Extract_Menu(node);
    else if(propcmp(node, "class", "GtkMenuItem"))
        widget = Extract_MenuItem(node);
    else if(propcmp(node, "class", "GtkBox"))
        widget = Extract_Box(node);
    else if(propcmp(node, "class", "GtkScrollbar"))
        widget = Extract_Scrollbar(node);
    else if(propcmp(node, "class", "GtkEntry"))
        widget = Extract_Entry(node);
    else if(propcmp(node, "class", "GtkSpinButton"))
        widget = Extract_SpinButton(node);
    else if(propcmp(node, "class", "GtkButton"))
        widget = Extract_Button(node);
    else if(propcmp(node, "class", "GtkToggleButton"))
        widget = Extract_ToggleButton(node);
    else if(propcmp(node, "class", "GtkCheckButton"))
        widget = Extract_CheckButton(node);
    else if(propcmp(node, "class", "GtkRadioButton"))
        widget = Extract_RadioButton(node);
    else if(propcmp(node, "class", "GtkMenuButton"))
        widget = Extract_MenuButton(node);
    else if(propcmp(node, "class", "GtkLabel"))
        widget = Extract_Label(node);
    else if(propcmp(node, "class", "GtkImage"))
        widget = Extract_Image(node);
    else if(propcmp(node, "class", "GtkViewport"))
        widget = Extract_Viewport(node);
    else if(propcmp(node, "class", "GtkScrolledWindow"))
        widget = Extract_ScrolledWindow(node);
    else if(propcmp(node, "class", "GtkWindow")) {
        widget = Extract_Window(node);
    }
    else if(propcmp(node, "class", "GtkAdjustment")) {
        SDLGuiTK_Object *adjustment = Extract_Adjustment(node);
        SDLGuiTK_list_append (current_builder->widgets,
                              adjustment);
        if(propget(node, "id"))
            SDLGuiTK_object_set_name(adjustment, propget(node, "id"));
    }
    else {
        SDLGUITK_ERROR("No Builder process for ");
        SDLGUITK_ERROR2(propget(node, "class"));
    }
    if(widget) {
        if(propget(node, "id"))
            SDLGuiTK_object_set_name(widget->object, propget(node, "id"));
        SDLGuiTK_list_append (current_builder->widgets,
                              widget->object);
    }
    return widget;
}



static void Extract_widgets(xmlNode * node, SDLGuiTK_Widget * parent)
{
    while(node)
    {
        if(namecmp(node,"object"))
            Extract_object(node);
        node = node->next;
    }
}

SDLGuiTK_Builder * SDLGuiTK_builder_new()
{
    SDLGuiTK_Builder * new_builder = Builder_create ();

    return new_builder;
}

SDLGuiTK_Builder * SDLGuiTK_builder_new_from_file (const char *filename)
{
    SDLGuiTK_Builder * new_builder = Builder_create ();
    xmlNode * node;
    current_builder = new_builder;

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile(filename, NULL, 0);

    if (doc == NULL) {
    printf("Could not parse the XML file");
    }

    root_element = xmlDocGetRootElement(doc);
    node = root_element;
    while(node)
    {
        if(namecmp(node,"interface"))
            Extract_widgets(node->children, NULL);
        node = node->next;
    }

    xmlFreeDoc(doc);

    xmlCleanupParser();

    return new_builder;
}

void SDLGuiTK_builder_destroy(SDLGuiTK_Builder * builder)
{
    Builder_destroy (builder);
}

SDLGuiTK_Object * SDLGuiTK_builder_get_object (SDLGuiTK_Builder *builder,
                                               const char *name)
{
    SDLGuiTK_Object *current;
    if(!builder)
        builder = current_builder;
    current = SDLGuiTK_list_ref_init (builder->widgets);
    while(current) {
        if(strcmp(current->name,name)==0)
            return current;
        current = SDLGuiTK_list_ref_next (builder->widgets);
    }
    return NULL;
}

SDLGuiTK_Widget * SDLGuiTK_builder_get_widget (SDLGuiTK_Builder *builder,
                                               const char *name)
{
    SDLGuiTK_Object *current;
    if(!builder)
        builder = current_builder;
    current = SDLGuiTK_list_ref_init (builder->widgets);
    while(current) {
        if(strcmp(current->name,name)==0)
            return current->widget;
        current = SDLGuiTK_list_ref_next (builder->widgets);
    }
    return NULL;
}


SDLGuiTK_List * SDLGuiTK_builder_get_objects (SDLGuiTK_Builder *builder)
{
    return builder->widgets;
}



int isnode(xmlNode * node)
{
    if(node->type == XML_ELEMENT_NODE)
        return 1;
    return 0;
}
int namecmp(xmlNode * node, char * string)
{
    if(node->type == XML_ELEMENT_NODE)
        if(strcmp((const char*)node->name, string)==0)
            return 1;
    return 0;
}
char * nameget(xmlNode * node)
{
    if(node->type != XML_ELEMENT_NODE)
        return "";
    return (char*) node->name;
}
int contentcmp(xmlNode * node, char * string)
{
    if(node->type == XML_ELEMENT_NODE)
        if(strcmp((const char*)xmlNodeGetContent(node), string)==0)
            return 1;
    return 0;
}
char * contentget(xmlNode * node)
{
    if(node->type == XML_ELEMENT_NODE)
        return (char*) xmlNodeGetContent(node);
    return "";
}
int propcmp(xmlNode * node, char * name, const char * string)
{
    if(node->type == XML_ELEMENT_NODE)
        if(strcmp((const char *)xmlGetProp(node, (const xmlChar*)name), string)==0)
            return 1;
    return 0;
}
char * propget(xmlNode * node, const char * name)
{
    if(node->type == XML_ELEMENT_NODE)
        return (char*) xmlGetProp(node, (const xmlChar*)name);
    return (char*) NULL;

}


