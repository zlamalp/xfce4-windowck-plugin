/*      $Id$

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2, or (at your option)
        any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., Inc., 51 Franklin Street, Fifth Floor, Boston,
        MA 02110-1301, USA.


        xfwm4    - (c) 2002-2011 Olivier Fourdan
        xfwm4-windowck-plugin - (c) 2013 Cedric leporcq
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <pango/pango-font.h>

#include <libxfce4util/libxfce4util.h>
#include "ui_style.h"

static GdkRGBA
shade (const GdkRGBA* color, float b)
{
    GdkRGBA rgba;

    rgba.red = color->red * b;
    rgba.green = color->green * b;
    rgba.blue = color->blue * b;

    return rgba;
}

static GdkRGBA
mix (const GdkRGBA* color1, const GdkRGBA* color2, float a)
{
    GdkRGBA color;

    color.red = color1->red * a + color2->red * (1 - a);
    color.green = color1->green * a + color2->green * (1 - a);
    color.blue = color1->blue * a + color2->blue * (1 - a);

    return color;
}

static gchar *
rgba_to_hex_string (const GdkRGBA * rgba)
{
    gchar *s;
    s = g_new (gchar, 14);
    g_snprintf (s, 14, "#%04x%04x%04x", (int)(rgba->red * 255), (int)(rgba->green * 255), (int)(rgba->blue * 255));
    return s;
}

gchar *
get_ui_color (GtkWidget * win, const gchar * name, GtkStateFlags state)
{
    GtkStyleContext *style;
    GdkRGBA *rgba;
    gchar *s;

    TRACE ("entering get_ui_color");

    g_return_val_if_fail (win != NULL, NULL);
    g_return_val_if_fail (GTK_IS_WIDGET (win), NULL);
    g_return_val_if_fail (gtk_widget_get_realized (win), NULL);
    g_return_val_if_fail (name != NULL, NULL);

    style = gtk_widget_get_style_context (win);
    gtk_style_context_get (style, state, name, &rgba, NULL);
    s = rgba_to_hex_string (rgba);
    gdk_rgba_free (rgba);
    TRACE ("%s[%d]=%s", name, state, s);
    return (s);
}

gchar *
mix_bg_fg (GtkWidget * win, GtkStateFlags state, float alpha, float beta)
{
    GtkStyleContext *style;
    GdkRGBA *fg_rgba, *bg_rgba;
    GdkRGBA rgba;
    gchar *s;

    TRACE ("entering mix_bg_fg");

    g_return_val_if_fail (win != NULL, NULL);
    g_return_val_if_fail (GTK_IS_WIDGET (win), NULL);
    g_return_val_if_fail (gtk_widget_get_realized (win), NULL);

    style = gtk_widget_get_style_context (win);

    gtk_style_context_get (style, state, GTK_STYLE_PROPERTY_COLOR, &fg_rgba, NULL);
    gtk_style_context_get (style, state, GTK_STYLE_PROPERTY_BACKGROUND_COLOR, &bg_rgba, NULL);
    rgba = mix (fg_rgba, bg_rgba, alpha);
    gdk_rgba_free (bg_rgba);
    gdk_rgba_free (fg_rgba);

    rgba = shade (&rgba, beta);
    s = rgba_to_hex_string (&rgba);

    TRACE ("mix_bg_fg[%d]=%s", state, s);
    return (s);
}

PangoFontDescription *
getUIPangoFontDesc (GtkWidget * win)
{
    GtkStyle *style;

    TRACE ("entering getUIPangoFontDesc");

    g_return_val_if_fail (win != NULL, NULL);
    g_return_val_if_fail (GTK_IS_WIDGET (win), NULL);
    g_return_val_if_fail (gtk_widget_get_realized (win), NULL);

    style = gtk_widget_get_style (win);
    return (style->font_desc);
}

PangoContext *
getUIPangoContext (GtkWidget * win)
{
    TRACE ("entering getUIPangoContext");

    g_return_val_if_fail (win != NULL, NULL);
    g_return_val_if_fail (GTK_IS_WIDGET (win), NULL);
    g_return_val_if_fail (gtk_widget_get_realized (win), NULL);

    return (gtk_widget_get_pango_context (win));
}
