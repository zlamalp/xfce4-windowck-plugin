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
    const gulong length = 14;
    s = g_new (gchar, length);
    g_snprintf (s, length, "#%04x%04x%04x", (guint16)(rgba->red * USHRT_MAX), (guint16)(rgba->green * USHRT_MAX), (guint16)(rgba->blue * USHRT_MAX));
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

/**
 * What user sees is a combination of multiple layers.
 * This is only important when top layer is semi-transparent.
 *
 * Adapted from org.eclipse.swt/Eclipse SWT/gtk/org/eclipse/swt/widgets/Display.java
 */
static void
render_all_backgrounds(GtkStyleContext *style_ctx, cairo_t *cairo)
{
    GtkStyleContext *parent_style_ctx = gtk_style_context_get_parent (style_ctx);

    if (parent_style_ctx != NULL)
        render_all_backgrounds (parent_style_ctx, cairo);

    gtk_render_background (style_ctx, cairo, -50, -50, 100, 100);
}

/**
 * Calculates original color from RGBA with premultiplied alpha.
 *
 * NOTE: Calculating inverse gives a range of possible colors due to rounding that
 * occurs with integer calculations. However, alpha-blend formula only has the
 * multiplied component, so all of those inverses are equivalent.
 *
 * Adapted from org.eclipse.swt/Eclipse SWT/gtk/org/eclipse/swt/widgets/Display.java
 */
static int
inverse_premultiplied_color(int color, int alpha)
{
    if (alpha == 0)
        return 0;
    return (255*color + alpha-1) / alpha;
}

/**
 * Background in GTK theme can be more complex then just solid color:
 * 1) Due to 'background-image', 'background-position', 'background-repeat', etc.
 *    Example: 'tooltip' in 'Ambiance' theme uses 'background-image'.
 * 2) If background is semi-transparent, user actually sees a combination of layers.
 *    Example: 'tooltip' in 'HighContrast' theme has transparent label.
 * Both problems are solved by drawing to a temporary image and getting
 * the color of the pixel in the middle.
 *
 * Adapted from org.eclipse.swt/Eclipse SWT/gtk/org/eclipse/swt/widgets/Display.java
 */
static GdkRGBA
style_context_estimate_background_color(GtkStyleContext *context, GtkStateFlags state)
{
    cairo_surface_t *surface;
    cairo_t *cairo;
    unsigned char *data;
    int a, r, g, b;
    GdkRGBA rgba;

    // Render to a temporary image
    gtk_style_context_save (context);
    gtk_style_context_set_state (context, state);
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1, 1);
    cairo = cairo_create (surface);
    render_all_backgrounds (context, cairo);
    cairo_fill (cairo);
    cairo_surface_flush (surface);
    data = cairo_image_surface_get_data (surface);

    // CAIRO_FORMAT_ARGB32 means a-r-g-b order, 1 byte per value.
    a = data[3];
    r = data[2];
    g = data[1];
    b = data[0];

    cairo_surface_destroy (surface);
    cairo_destroy (cairo);
    gtk_style_context_restore (context);

    // NOTE: cairo uses premultiplied alpha (see CAIRO_FORMAT_ARGB32)
    rgba.alpha = a / 255.0;
    rgba.red   = inverse_premultiplied_color(r, a) / 255.0;
    rgba.green = inverse_premultiplied_color(g, a) / 255.0;
    rgba.blue  = inverse_premultiplied_color(b, a) / 255.0;

    return rgba;
}


gchar *
mix_bg_fg (GtkWidget * win, GtkStateFlags state, float alpha, float beta)
{
    GtkStyleContext *style;
    GdkRGBA fg_rgba;
    GdkRGBA bg_rgba;
    GdkRGBA rgba;
    gchar *s;

    TRACE ("entering mix_bg_fg");

    g_return_val_if_fail (win != NULL, NULL);
    g_return_val_if_fail (GTK_IS_WIDGET (win), NULL);
    g_return_val_if_fail (gtk_widget_get_realized (win), NULL);

    style = gtk_widget_get_style_context (win);
    gtk_style_context_get_color (style, state, &fg_rgba);
    bg_rgba = style_context_estimate_background_color (style, state);
    rgba = mix (&fg_rgba, &bg_rgba, alpha);
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
