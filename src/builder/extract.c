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

#include <SDL2/SDL_guitk.h>

#include <libxml2/libxml/parser.h>

#include "../debug.h"
#include "../mysdl.h"
#include "../myttf.h"
#include "../signal.h"
#include "../object_prot.h"
#include "../widget_prot.h"
#include "../adjustment_prot.h"
#include "builder.h"
#include "extract.h"


SDLGuiTK_Object * Extract_Adjustment(xmlNode * node)
{
    SDLGuiTK_Adjustment *adjustment = NULL;
    xmlNode * child;
    double value = 0;
    double lower = 0;
    double upper = 100;
    double step_increment = 1;
    SDLGUITK_LOG ("Extract Adjustment ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "value")) {
                SDLGUITK_LOG ("Extract Adjustment property: value\n");
                value = atof(contentget (child));
            }
            else if(propcmp(child, "name", "lower")) {
                SDLGUITK_LOG ("Extract Adjustment property: lower\n");
                lower = atof(contentget (child));
            }
            else if(propcmp(child, "name", "upper")) {
                SDLGUITK_LOG ("Extract Adjustment property: upper\n");
                upper = atof(contentget (child));
            }
            else if(propcmp(child, "name", "step_increment")) {
                SDLGUITK_LOG ("Extract Adjustment property: step_increment\n");
                step_increment = atof(contentget (child));
            }
            else {
                SDLGUITK_ERROR("Node Adjustment property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Adjustment name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(!adjustment)
        adjustment = SDLGuiTK_adjustment_new(value, lower, upper, step_increment);
    return adjustment->object;
}

static SDL_bool Is_Widget_Property(char * name)
{
    if(strcmp(name,"visible")==0 || strcmp(name,"tooltip_text")==0)
        return SDL_TRUE;
    return SDL_FALSE;
}

static void Extract_Widget_properties(SDLGuiTK_Widget *widget,
                                      xmlNode * node)
{
    xmlNode * child=node->children;
    SDLGUITK_LOG ("Extract Widget properties ..\n");
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "visible")) {
                SDLGUITK_LOG ("Extract Widget property: visible\n");
                if(contentcmp(child, "True"))
                    SDLGuiTK_widget_show (widget);
            }
            else if(propcmp(child, "name", "tooltip_text")) {
                SDLGUITK_LOG ("Extract Widget property: tooltip_text\n");
                SDLGuiTK_widget_set_tooltip_text(widget,
                                                 contentget(child));
            }
        }
        child = child->next;
    }

}

SDLGuiTK_Widget * Extract_ProgressBar(xmlNode * node)
{
    SDLGuiTK_Widget *progressbar = NULL;
    xmlNode * child;
    double fraction = 0;
    SDLGUITK_LOG ("Extract ProgressBar ..\n");
    child = node->children;
    progressbar = SDLGuiTK_progress_bar_new ();
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "fraction")) {
                SDLGUITK_LOG ("Extract ProgressBar property: fraction\n");
                fraction = atof(contentget(child));
            }
            else if(propcmp(child, "name", "orientation")) {
                SDLGUITK_LOG ("Extract ProgressBar property: orientation\n");
                if(contentcmp(child, "vertical"))
                    SDLGuiTK_progress_bar_set_orientation(SDLGuiTK_PROGRESSBAR(progressbar),
                                                          SDLGUITK_ORIENTATION_VERTICAL);
                else
                    SDLGuiTK_progress_bar_set_orientation(SDLGuiTK_PROGRESSBAR(progressbar),
                                                          SDLGUITK_ORIENTATION_HORIZONTAL);
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node ProgressBar property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node ProgressBar name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(progressbar&&fraction)
        SDLGuiTK_progress_bar_set_fraction (SDLGuiTK_PROGRESSBAR(progressbar), fraction);
    if(progressbar)
        Extract_Widget_properties (progressbar, node);
    return progressbar;
}

SDLGuiTK_Widget * Extract_Scrollbar(xmlNode * node)
{
    SDLGuiTK_Widget *scrollbar = NULL;
    SDLGuiTK_Adjustment *adjustment = NULL;
    xmlNode * child;
    int orientation = SDLGUITK_ORIENTATION_HORIZONTAL;
    SDLGUITK_LOG ("Extract Scrollbar ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "orientation")) {
                SDLGUITK_LOG ("Extract Scrollbar property: orientation\n");
                if(contentcmp(child, "vertical"))
                    orientation = SDLGUITK_ORIENTATION_VERTICAL;
            }
            else if(propcmp(child, "name", "adjustment")) {
                SDLGUITK_LOG ("Extract Scrollbar property: adjustment\n");
                adjustment = SDLGuiTK_builder_get_object (NULL, contentget(child))->adjustment;
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node Scrollbar property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Scrollbar name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(adjustment)
        scrollbar = SDLGuiTK_scrollbar_new (orientation, adjustment);
    else
        scrollbar = SDLGuiTK_scrollbar_new (orientation, NULL);
    if(scrollbar)
        Extract_Widget_properties (scrollbar, node);
    return scrollbar;
}


SDLGuiTK_Widget * Extract_Entry(xmlNode * node)
{
    SDLGuiTK_Widget *entry = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Entry ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "text")) {
                SDLGUITK_LOG ("Extract Entry property: text\n");
                entry = SDLGuiTK_entry_new (contentget (child));
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node Entry property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Entry name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(!entry)
        entry = SDLGuiTK_entry_new ("");
    if(entry)
        Extract_Widget_properties (entry, node);
    return entry;
}

SDLGuiTK_Widget * Extract_SpinButton(xmlNode * node)
{
    SDLGuiTK_Widget *spinbutton = NULL;
    SDLGuiTK_Adjustment *adjustment = NULL;
    xmlNode * child;
    double climb_rate = 0;
    int digits = 0;
    SDL_bool have_text = SDL_FALSE;
    char text[1024];
    SDLGUITK_LOG ("Extract SpinButton ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "text")) {
                SDLGUITK_LOG ("Extract SpinButton property: text\n");
                strcpy (text, contentget (child));
                have_text = SDL_TRUE;
            }
            else if(propcmp(child, "name", "adjustment")) {
                SDLGUITK_LOG ("Extract SpinButton property: adjustment\n");
                adjustment = SDLGuiTK_builder_get_object (NULL, contentget(child))->adjustment;
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node SpinButton property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node SpinButton name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    spinbutton = SDLGuiTK_spin_button_new (adjustment, climb_rate, digits);
    if(spinbutton && have_text)
        SDLGuiTK_entry_set_text (spinbutton, text);
    if(spinbutton)
        Extract_Widget_properties (spinbutton, node);
    return spinbutton;
}

SDLGuiTK_Widget * Extract_Button(xmlNode * node)
{
    SDLGuiTK_Widget *button = NULL, *label = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Button ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "label")) {
                SDLGUITK_LOG ("Extract Button property: label\n");
                label = SDLGuiTK_label_new (contentget(child));
                SDLGuiTK_misc_set_padding( SDLGuiTK_MISC (label), 5, 3);
                SDLGuiTK_widget_show (label);
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node Button property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Button name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    button = SDLGuiTK_button_new ();
    if(label)
        SDLGuiTK_container_add (SDLGuiTK_CONTAINER (button), label);
    if(button)
        Extract_Widget_properties (button, node);
    return button;
}

SDLGuiTK_Widget * Extract_ToggleButton(xmlNode * node)
{
    SDLGuiTK_Widget *togglebutton = NULL, *label = NULL;
    xmlNode * child;
    SDL_bool active = SDL_FALSE;
    SDLGUITK_LOG ("Extract ToggleButton ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "active")) {
                SDLGUITK_LOG ("Extract ToggleButton property: active\n");
                if(contentcmp(child, "True"))
                    active = SDL_TRUE;
            }
            else if(propcmp(child, "name", "label")) {
                SDLGUITK_LOG ("Extract ToggleButton property: label\n");
                label = SDLGuiTK_label_new (contentget(child));
                SDLGuiTK_misc_set_padding( SDLGuiTK_MISC (label), 5, 3);
                SDLGuiTK_widget_show (label);
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node ToggleButton property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node ToggleButton name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    togglebutton = SDLGuiTK_toggle_button_new ();
    if(label)
        SDLGuiTK_container_add (SDLGuiTK_CONTAINER (togglebutton), label);
    if(togglebutton && active)
        SDLGuiTK_toggle_button_set_active(SDLGuiTK_TOGGLE_BUTTON( togglebutton ),
                                          active);
    if(togglebutton)
        Extract_Widget_properties (togglebutton, node);
    return togglebutton;
}

SDLGuiTK_Widget * Extract_CheckButton(xmlNode * node)
{
    SDLGuiTK_Widget *checkbutton = NULL, *label = NULL;
    xmlNode * child;
    SDL_bool active = SDL_FALSE;
    SDLGUITK_LOG ("Extract CheckButton ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "active")) {
                SDLGUITK_LOG ("Extract CheckButton property: active\n");
                if(contentcmp(child, "True"))
                    active = SDL_TRUE;
            }
            else if(propcmp(child, "name", "label")) {
                SDLGUITK_LOG ("Extract CheckButton property: label\n");
                label = SDLGuiTK_label_new (contentget(child));
                SDLGuiTK_misc_set_padding( SDLGuiTK_MISC (label), 5, 3);
                SDLGuiTK_widget_show (label);
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node CheckButton property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node CheckButton name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    checkbutton = SDLGuiTK_check_button_new ();
    if(label)
        SDLGuiTK_container_add (SDLGuiTK_CONTAINER (checkbutton), label);
    if(checkbutton && active)
        SDLGuiTK_toggle_button_set_active(SDLGuiTK_TOGGLE_BUTTON( checkbutton ),
                                          active);
    if(checkbutton)
        Extract_Widget_properties (checkbutton, node);
    return checkbutton;
}

SDLGuiTK_Widget * Extract_RadioButton(xmlNode * node)
{
    SDLGuiTK_Widget *radiobutton = NULL, *label = NULL;
    xmlNode * child;
    SDL_bool active = SDL_FALSE;
    SDLGuiTK_Widget * radiogroup = NULL;
    SDLGUITK_LOG ("Extract RadioButton ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "active")) {
                SDLGUITK_LOG ("Extract RadioButton property: active\n");
                if(contentcmp(child, "True"))
                    active = SDL_TRUE;
            }
            else if(propcmp(child, "name", "label")) {
                SDLGUITK_LOG ("Extract RadioButton property: label\n");
                label = SDLGuiTK_label_new (contentget(child));
                SDLGuiTK_misc_set_padding( SDLGuiTK_MISC (label), 5, 3);
                SDLGuiTK_widget_show (label);
            }
            else if(propcmp(child, "name", "group")) {
                SDLGUITK_LOG ("Extract RadioButton property: group\n");
                radiogroup = SDLGuiTK_builder_get_widget(NULL, contentget(child));
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node RadioButton property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node RadioButton name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    radiobutton = SDLGuiTK_radio_button_new ();
    if(label)
        SDLGuiTK_container_add (SDLGuiTK_CONTAINER (radiobutton), label);
    if(radiobutton && active)
        SDLGuiTK_toggle_button_set_active(SDLGuiTK_TOGGLE_BUTTON( radiobutton ),
                                          active);
    if(radiobutton && radiogroup)
        SDLGuiTK_radio_button_join_group(SDLGuiTK_RADIO_BUTTON (radiobutton),
                                         SDLGuiTK_RADIO_BUTTON (radiogroup));
    if(radiobutton)
        Extract_Widget_properties (radiobutton, node);
    return radiobutton;
}

SDLGuiTK_Widget * Extract_MenuButton(xmlNode * node)
{
    SDLGuiTK_Widget *menubutton = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract MenuButton ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "popup")) {
                SDLGUITK_LOG ("Extract MenuButton property: popup\n");
                menubutton = SDLGuiTK_builder_get_widget (NULL, contentget(child));
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node MenuButton property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node MenuButton name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(menubutton)
        Extract_Widget_properties (menubutton, node);
    return menubutton;
}

SDLGuiTK_Widget * Extract_Label(xmlNode * node)
{
    SDLGuiTK_Widget *label = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Label ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "label")) {
                SDLGUITK_LOG ("Extract Label property: label\n");
                label = SDLGuiTK_label_new (contentget(child));
                SDLGuiTK_misc_set_padding( SDLGuiTK_MISC (label), 5, 3);
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node Label property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Label name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(label)
        Extract_Widget_properties (label, node);
    return label;
}

SDLGuiTK_Widget * Extract_Image(xmlNode * node)
{
    SDLGuiTK_Widget *image = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Image ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property")) {
            if(propcmp(child, "name", "pixbuf")) {
                SDLGUITK_LOG ("Extract Image property: pixbuf\n");
                image = SDLGuiTK_image_new_from_file (contentget(child));
            }
            else if(!Is_Widget_Property (propget (child, "name"))) {
                SDLGUITK_ERROR("Node Image property unknown: ");
                SDLGUITK_ERROR2(propget(child,"name"));
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Image name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(image)
        Extract_Widget_properties (image, node);
    return image;
}



static void Extract_Viewport_children(xmlNode * node, SDLGuiTK_Widget *viewport)
{
    SDLGuiTK_Widget *viewportchild = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Viewport children ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"object"))
            viewportchild = Extract_object(child);
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Viewport name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(viewportchild)
        SDLGuiTK_container_add (SDLGuiTK_CONTAINER (viewport), viewportchild);
}

static void Extract_Viewport_property(xmlNode * node, SDLGuiTK_Widget * viewport)
{
    SDLGUITK_LOG ("Extract Viewport property ..\n");
    if(propcmp(node, "name", "visible")) {
        SDLGUITK_LOG ("Extract Viewport property: visible\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_widget_show (viewport);
    }
    else {
        SDLGUITK_ERROR("Node Viewport property unknown: ");
        SDLGUITK_ERROR2(propget(node,"name"));
    }
}

SDLGuiTK_Widget * Extract_Viewport(xmlNode * node)
{
    SDLGuiTK_Widget *viewport;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Viewport ..\n");
    viewport = SDLGuiTK_viewport_new (NULL,NULL);
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property"))
            Extract_Viewport_property(child, viewport);
        else if(namecmp(child,"child")) {
            Extract_Viewport_children(child, viewport);
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Viewport name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    return viewport;
}


static void Extract_ScrolledWindow_children(xmlNode * node, SDLGuiTK_Widget *scrolledwindow)
{
    SDLGuiTK_Widget *scrolledwindowchild = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract ScrolledWindow children ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"object"))
            scrolledwindowchild = Extract_object(child);
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node ScrolledWindow name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(scrolledwindowchild)
        SDLGuiTK_container_add (SDLGuiTK_CONTAINER (scrolledwindow), scrolledwindowchild);
}

static void Extract_ScrolledWindow_property(xmlNode * node, SDLGuiTK_Widget * scrolledwindow)
{
    SDLGUITK_LOG ("Extract ScrolledWindow property ..\n");
    if(propcmp(node, "name", "visible")) {
        SDLGUITK_LOG ("Extract ScrolledWindow property: visible\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_widget_show (scrolledwindow);
    }
    else {
        SDLGUITK_ERROR("Node ScrolledWindow property unknown: ");
        SDLGUITK_ERROR2(propget(node,"name"));
    }
}

SDLGuiTK_Widget * Extract_ScrolledWindow(xmlNode * node)
{
    SDLGuiTK_Widget *scrolledwindow;
    xmlNode * child;
    SDLGUITK_LOG ("Extract ScrolledWindow ..\n");
    scrolledwindow = SDLGuiTK_scrolled_window_new ();
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property"))
            Extract_ScrolledWindow_property(child, scrolledwindow);
        else if(namecmp(child,"child")) {
            Extract_ScrolledWindow_children(child, scrolledwindow);
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node ScrolledWindow name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    return scrolledwindow;
}



static void Extract_MenuItem_property(xmlNode * node, SDLGuiTK_Widget * menuitem)
{
    SDLGuiTK_Widget *menuitemchild;
    SDLGUITK_LOG ("Extract MenuItem property ..\n");
    if(propcmp(node, "name", "visible")) {
        SDLGUITK_LOG ("Extract MenuItem property: visible\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_widget_show (menuitem);
    }
    else if(propcmp(node, "name", "label")) {
        SDLGUITK_LOG ("Extract MenuItem property: label\n");
        menuitemchild = SDLGuiTK_label_new(contentget(node));
        SDLGuiTK_widget_show (menuitemchild);
        SDLGuiTK_container_add( SDLGuiTK_CONTAINER(menuitem), menuitemchild );
    }
    else {
        SDLGUITK_ERROR("Node MenuItem property unknown: ");
        SDLGUITK_ERROR2(propget(node,"name"));
    }
}

SDLGuiTK_Widget * Extract_MenuItem(xmlNode * node)
{
    SDLGuiTK_Widget *menuitem;
    SDLGUITK_LOG ("Extract MenuItem ..\n");
    menuitem = SDLGuiTK_menuitem_new ();
    xmlNode * child = node->children;
    while(child)
    {
        if(namecmp(child,"property"))
            Extract_MenuItem_property(child, menuitem);
        //No child in GTK API, but in SDLGuiTK
        else if(namecmp(child,"child")) {
            SDLGUITK_ERROR ("Extract child MenuItem TODO\n");
            //menuitemchild = Extract_object(child->children);
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node MenuItem name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    return menuitem;
}

static void Extract_Menu_property(xmlNode * node, SDLGuiTK_Widget * menu)
{
    SDLGUITK_LOG ("Extract Menu property ..\n");
    if(propcmp(node, "name", "visible")) {
        SDLGUITK_LOG ("Extract Menu property: visible\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_widget_show (menu);
    }
    else {
        SDLGUITK_ERROR("Node Menu property unknown: ");
        SDLGUITK_ERROR2(propget(node,"name"));
    }
}

SDLGuiTK_Widget * Extract_Menu(xmlNode * node)
{
    SDLGuiTK_Widget *menu, *menuchild;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Menu ..\n");
    menu = SDLGuiTK_menu_new ();
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property"))
            Extract_Menu_property(child, menu);
        else if(namecmp(child,"child")) {
            xmlNode * cnode = child->children;
            while(cnode) {
                if(isnode(cnode)){
                    if((menuchild = Extract_object(cnode)))
                        SDLGuiTK_menu_shell_append( SDLGuiTK_MENU (menu),
                                                    menuchild );
                }
                cnode = cnode->next;
            }
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Menu name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    return menu;
}


static void Extract_Window_children(xmlNode * node, SDLGuiTK_Widget *window)
{
    SDLGuiTK_Widget *windowchild = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Window children ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"object"))
            windowchild = Extract_object(child);
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Window name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    if(windowchild)
        SDLGuiTK_container_add (SDLGuiTK_CONTAINER (window), windowchild);
}

static void Extract_Window_property(xmlNode * node, SDLGuiTK_Widget * window)
{
    //SDLGuiTK_Widget *child;
    SDLGUITK_LOG ("Extract Window property ..\n");
    if(propcmp(node, "name", "visible")) {
        SDLGUITK_LOG ("Extract Window property: visible\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_widget_show (window);
    }
    else if(propcmp(node, "name", "title")) {
        SDLGUITK_LOG ("Extract Window property: title\n");
        SDLGuiTK_window_set_title (window, contentget(node));
    }
    else {
        SDLGUITK_ERROR("Node Window property unknown: ");
        SDLGUITK_ERROR2(propget(node,"name"));
    }
}

SDLGuiTK_Widget * Extract_Window(xmlNode * node)
{
    SDLGuiTK_Widget *window;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Window ..\n");
    window = SDLGuiTK_window_new ();
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property"))
            Extract_Window_property(child, window);
        else if(namecmp(child,"child")) {
            Extract_Window_children(child, window);
        }
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Window name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    return window;
}



static void Extract_Box_packing(xmlNode * node, SDLGuiTK_Widget *box, SDLGuiTK_Widget *boxchild)
{
    SDL_bool expand = SDL_FALSE;
    SDL_bool fill = SDL_FALSE;
    int padding = 0;
    SDLGUITK_LOG ("Extract Box packing ..\n");
    xmlNode * child = node->children;
    while(child)
    {
        if(propcmp(child, "name", "expand"))
        {
            SDLGUITK_LOG ("Extract Box_packing property: expand\n");
            if(contentcmp(child, "True"))
                expand = SDL_TRUE;
        }
        else if(propcmp(child, "name", "fill"))
        {
            SDLGUITK_LOG ("Extract Box_packing property: fill\n");
            if(contentcmp(child, "True"))
                fill = SDL_TRUE;
        }
        else if(propcmp(child, "name", "padding"))
        {
            SDLGUITK_LOG ("Extract Box_packing property: padding\n");
            padding = atoi(contentget(child));
        }
        else if(isnode(child))
        {
            SDLGUITK_ERROR("Node Box_packing property unknown: ");
            SDLGUITK_ERROR2(propget(child,"name"));
        }
        child = child->next;
    }
    if(boxchild!=NULL)
        SDLGuiTK_box_pack_start(SDLGuiTK_BOX(box), boxchild, expand, fill, padding);
    else
        SDLGUITK_ERROR("boxchild is NULL\n");
}

static void Extract_Box_children(xmlNode * node, SDLGuiTK_Widget *box)
{
    SDLGuiTK_Widget *boxchild = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Box children ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"object"))
            boxchild = Extract_object(child);
        else if(namecmp(child,"packing"))
            Extract_Box_packing(child, box, boxchild);
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Box_children name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
}

static void Extract_Box_property(xmlNode * node, SDLGuiTK_Widget * box)
{
    if(propcmp(node, "name", "visible"))
    {
        SDLGUITK_LOG ("Extract Box property: visible\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_widget_show (box);
    }
    else if(propcmp(node, "name", "orientation"))
    {
        SDLGUITK_LOG ("Extract Box property: orientation\n");
        if(contentcmp(node, "vertical"))
            SDLGuiTK_box_set_orientation (SDLGuiTK_BOX(box), SDLGUITK_BOX_V);
        else
            SDLGuiTK_box_set_orientation (SDLGuiTK_BOX(box), SDLGUITK_BOX_H);
    }
    else if(propcmp(node, "name", "homogeneous"))
    {
        SDLGUITK_LOG ("Extract Box property: homogeneous\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_box_set_homogeneous (SDLGuiTK_BOX(box), SDL_TRUE);
        else
            SDLGuiTK_box_set_homogeneous (SDLGuiTK_BOX(box), SDL_FALSE);
    }
    else if(isnode(node))
    {
        SDLGUITK_ERROR("Node Box property unknown: ");
        SDLGUITK_ERROR2(propget(node,"name"));
    }
}

SDLGuiTK_Widget * Extract_Box(xmlNode * node)
{
    SDLGuiTK_Widget *box;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Box ..\n");
    box = SDLGuiTK_hbox_new ();
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property"))
            Extract_Box_property(child, box);
        else if(namecmp(child,"child"))
            Extract_Box_children (child, box);
        else if(isnode(child))
        {
            SDLGUITK_ERROR("Node Box name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    return box;
}


static void Extract_Grid_packing(xmlNode * node, SDLGuiTK_Widget *grid,
                                 SDLGuiTK_Widget *gridchild)
{
    int left = 0;
    int top = 0;
    int width = 1;
    int height = 1;
    SDLGUITK_LOG ("Extract Grid packing ..\n");
    xmlNode * child = node->children;
    while(child)
    {
        if(propcmp(child, "name", "left_attach"))
        {
            SDLGUITK_LOG ("Extract Grid_packing property: left_attach\n");
            left = atoi(contentget(child));
        }
        else if(propcmp(child, "name", "top_attach"))
        {
            SDLGUITK_LOG ("Extract Grid_packing property: top_attach\n");
            top = atoi(contentget(child));
        }
        else if(propcmp(child, "name", "width"))
        {
            SDLGUITK_LOG ("Extract Grid_packing property: width\n");
            width = atoi(contentget(child));
        }
        else if(propcmp(child, "name", "height"))
        {
            SDLGUITK_LOG ("Extract Grid_packing property: height\n");
            height = atoi(contentget(child));
        }
        else if(isnode(child))
        {
            SDLGUITK_ERROR("Node Grid_packing property unknown: ");
            SDLGUITK_ERROR2(propget(child,"name"));
        }
        child = child->next;
    }
    if(gridchild!=NULL)
        SDLGuiTK_grid_attach(SDLGuiTK_GRID(grid), gridchild,
                             left, top, width, height);
    else
        SDLGUITK_ERROR("gridchild is NULL\n");
}

static void Extract_Grid_children(xmlNode * node, SDLGuiTK_Widget *grid)
{
    SDLGuiTK_Widget *gridchild = NULL;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Grid children ..\n");
    child = node->children;
    while(child)
    {
        if(namecmp(child,"object"))
            gridchild = Extract_object(child);
        else if(namecmp(child,"packing"))
            Extract_Grid_packing(child, grid, gridchild);
        else if (isnode(child)) {
            SDLGUITK_ERROR("Node Grid_children name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
}

static void Extract_Grid_property(xmlNode * node, SDLGuiTK_Widget * grid)
{
    if(propcmp(node, "name", "visible"))
    {
        SDLGUITK_LOG ("Extract Grid property: visible\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_widget_show (grid);
    }
    else if(propcmp(node, "name", "column_spacing"))
    {
        SDLGUITK_LOG ("Extract Grid property: column_spacing\n");
        SDLGuiTK_grid_set_column_spacing (SDLGuiTK_GRID(grid),
                                          atoi(contentget(node)));
    }
    else if(propcmp(node, "name", "row_spacing"))
    {
        SDLGUITK_LOG ("Extract Grid property: row_spacing\n");
        SDLGuiTK_grid_set_row_spacing (SDLGuiTK_GRID(grid),
                                       atoi(contentget(node)));
    }
    else if(propcmp(node, "name", "column_homogeneous"))
    {
        SDLGUITK_LOG ("Extract Grid property: column_homogeneous\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_grid_set_column_homogeneous (SDLGuiTK_GRID(grid),
                                                  SDL_TRUE);
        else
            SDLGuiTK_grid_set_column_homogeneous (SDLGuiTK_GRID(grid),
                                                  SDL_FALSE);
    }
    else if(propcmp(node, "name", "row_homogeneous"))
    {
        SDLGUITK_LOG ("Extract Grid property: row_homogeneous\n");
        if(contentcmp(node, "True"))
            SDLGuiTK_grid_set_row_homogeneous (SDLGuiTK_GRID(grid),
                                               SDL_TRUE);
        else
            SDLGuiTK_grid_set_row_homogeneous (SDLGuiTK_GRID(grid),
                                               SDL_FALSE);
    }
    else if(isnode(node))
    {
        SDLGUITK_ERROR("Node Grid property unknown: ");
        SDLGUITK_ERROR2(propget(node,"name"));
    }
}

SDLGuiTK_Widget * Extract_Grid(xmlNode * node)
{
    SDLGuiTK_Widget *grid;
    xmlNode * child;
    SDLGUITK_LOG ("Extract Grid ..\n");
    grid = SDLGuiTK_grid_new ();
    child = node->children;
    while(child)
    {
        if(namecmp(child,"property"))
            Extract_Grid_property(child, grid);
        else if(namecmp(child,"child"))
            Extract_Grid_children (child, grid);
        else if(isnode(child))
        {
            SDLGUITK_ERROR("Node Grid name unknown: ");
            SDLGUITK_ERROR2(nameget(child));
        }
        child = child->next;
    }
    return grid;
}


