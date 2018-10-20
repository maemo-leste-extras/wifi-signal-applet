/*
 * Copyright (C) 2010 Tadej Borovšak
 * Copyright (C) 2010 Thomas Perl <thp@thpinfo.com>
 * Copyright (C) 2010 Faheem Pervez <trippin1@gmail.com>. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *      
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *      
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

/* utils.c */

#include "utils.h"
#include <glib-object.h>
#include <gtk/gtk.h>

static const cairo_user_data_key_t pixbuf_key;

#if 0
/**
 * he_helper_get_logical_font_desc:
 * @name: The logical font name (see he-helper.h for possible values)
 *
 * Returns a newly-allocated string that contains the string representation
 * of the Pango font description for the given logical string. This can be
 * used in the font_desc attribute of span elements in Pango markup.
 *
 * This function should be used to get the font desc for Pango markup in
 * special use cases (e.g. GtkTreeView, mixed-content GtkLabel). If you
 * want to set a logical font directly on a widget, you can use
 * #hildon_helper_set_logical_font (from Hildon 2.2) for this.
 *
 * The return value should be freed with g_free() after use.
 **/
gchar *
he_helper_get_logical_font_desc (const gchar *name)
{
    GtkSettings *settings = gtk_settings_get_default();
    GtkStyle *style = gtk_rc_get_style_by_paths(settings,
            name, NULL, G_TYPE_NONE);

    return pango_font_description_to_string(style->font_desc);
}

/**
 * he_helper_get_logical_font_color:
 * @name: The logical font color (see he-helper.h for possible values)
 *
 * Returns a newly-allocated string that contains the color value for the
 * requested logical color. This can be used in the foreground attribute of
 * span elements in Pango markup.
 *
 * This function should be used to get the font color for Pango markup in
 * special use cases (e.g. GtkTreeView, mixed-content GtkLabel). If you
 * want to set a logical color directly on a widget, you can use
 * #hildon_helper_set_logical_color (from Hildon 2.2) for this.
 *
 * The return value should be freed with g_free() after use.
 **/
gchar *
he_helper_get_logical_font_color (const gchar *name)
{
    GdkColor color;
    GtkSettings *settings = gtk_settings_get_default();
    GtkStyle *style = gtk_rc_get_style_by_paths(settings,
            "GtkButton", "osso-logical-colors", GTK_TYPE_BUTTON);

    if (gtk_style_lookup_color(style, name, &color)) {
        return gdk_color_to_string(&color);
    } else {
        return NULL;
    }
}
#endif

const PangoFontDescription*
he_helper_get_logical_font_desc (const gchar *name)
{
	GtkSettings *settings = gtk_settings_get_default();
	GtkStyle *style = gtk_rc_get_style_by_paths(settings,
				            name, NULL, G_TYPE_NONE);

	return style->font_desc;
}

/* thp, I think you're awesome, but please spell "colour" properly ;) */
gboolean
he_helper_get_logical_font_colour (const gchar *name, guint16 *red, guint16 *green, guint16 *blue)
{
	GdkColor colour;
	GtkSettings *settings = gtk_settings_get_default();
	GtkStyle *style = gtk_rc_get_style_by_paths(settings,
				                "GtkButton", "osso-logical-colors", GTK_TYPE_BUTTON);

	if (gtk_style_lookup_color(style, name, &colour)) {
		*red = colour.red;
		*green = colour.green;
		*blue = colour.blue;

		return TRUE;
	} else {
		return FALSE;
	}
}

/**
 * pixbuf_cairo_create:
 * @pixbuf: GdkPixbuf that you wish to wrap with cairo context
 *
 * This function will initialize new cairo context with contents of @pixbuf. You
 * can then draw using returned context. When finished drawing, you must call
 * pixbuf_cairo_destroy() or your pixbuf will not be updated with new contents!
 *
 * Return value: New cairo_t context. When you're done with it, call
 * pixbuf_cairo_destroy() to update your pixbuf and free memory.
 */
cairo_t *
pixbuf_cairo_create( GdkPixbuf *pixbuf )
{
    gint             width,        /* Width of both pixbuf and surface */
                     height,       /* Height of both pixbuf and surface */
                     p_stride,     /* Pixbuf stride value */
                     p_n_channels, /* RGB -> 3, RGBA -> 4 */
                     s_stride;     /* Surface stride value */
    guchar          *p_pixels,     /* Pixbuf's pixel data */
                    *s_pixels;     /* Surface's pixel data */
    cairo_surface_t *surface;      /* Temporary image surface */
    cairo_t         *cr;           /* Final context */

    g_object_ref( G_OBJECT( pixbuf ) );

    /* Inspect input pixbuf and create compatible cairo surface */
    g_object_get( G_OBJECT( pixbuf ), "width",           &width,
                                      "height",          &height,
                                      "rowstride",       &p_stride,
                                      "n-channels",      &p_n_channels,
                                      "pixels",          &p_pixels,
                                      NULL );
    surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, width, height );
    s_stride = cairo_image_surface_get_stride( surface );
    s_pixels = cairo_image_surface_get_data( surface );

    /* Copy pixel data from pixbuf to surface */
    while( height-- )
    {
        gint    i;
        guchar *p_iter = p_pixels,
               *s_iter = s_pixels;

        for( i = 0; i < width; i++ )
        {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
            /* Pixbuf:  RGB(A)
             * Surface: BGRA */
            if( p_n_channels == 3 )
            {
                s_iter[0] = p_iter[2];
                s_iter[1] = p_iter[1];
                s_iter[2] = p_iter[0];
                s_iter[3] = 0xff;
            }
            else /* p_n_channels == 4 */
            {
                gdouble alpha_factor = p_iter[3] / (gdouble)0xff;

                s_iter[0] = (guchar)( p_iter[2] * alpha_factor + .5 );
                s_iter[1] = (guchar)( p_iter[1] * alpha_factor + .5 );
                s_iter[2] = (guchar)( p_iter[0] * alpha_factor + .5 );
                s_iter[3] =           p_iter[3];
            }
#elif G_BYTE_ORDER == G_BIG_ENDIAN
            /* Pixbuf:  RGB(A)
             * Surface: ARGB */
            if( p_n_channels == 3 )
            {
                s_iter[3] = p_iter[2];
                s_iter[2] = p_iter[1];
                s_iter[1] = p_iter[0];
                s_iter[0] = 0xff;
            }
            else /* p_n_channels == 4 */
            {
                gdouble alpha_factor = p_iter[3] / (gdouble)0xff;

                s_iter[3] = (guchar)( p_iter[2] * alpha_factor + .5 );
                s_iter[2] = (guchar)( p_iter[1] * alpha_factor + .5 );
                s_iter[1] = (guchar)( p_iter[0] * alpha_factor + .5 );
                s_iter[0] =           p_iter[3];
            }
#else /* PDP endianness */
            /* Pixbuf:  RGB(A)
             * Surface: RABG */
            if( p_n_channels == 3 )
            {
                s_iter[0] = p_iter[0];
                s_iter[1] = 0xff;
                s_iter[2] = p_iter[2];
                s_iter[3] = p_iter[1];
            }
            else /* p_n_channels == 4 */
            {
                gdouble alpha_factor = p_iter[3] / (gdouble)0xff;

                s_iter[0] = (guchar)( p_iter[0] * alpha_factor + .5 );
                s_iter[1] =           p_iter[3];
                s_iter[1] = (guchar)( p_iter[2] * alpha_factor + .5 );
                s_iter[2] = (guchar)( p_iter[1] * alpha_factor + .5 );
            }
#endif
            s_iter += 4;
            p_iter += p_n_channels;
        }
        s_pixels += s_stride;
        p_pixels += p_stride;
    }

    /* Create context and set user data */
    cr = cairo_create( surface );
    cairo_surface_destroy( surface );
    cairo_set_user_data( cr, &pixbuf_key, pixbuf, g_object_unref );

    /* Return context */
    return( cr );
}

/**
 * pixbuf_cairo_destroy:
 * @cr: Cairo context that you wish to destroy
 * @create_new_pixbuf: If TRUE, new pixbuf will be created and returned. If
 *                     FALSE, input pixbuf will be updated in place.
 *
 * This function will destroy cairo context, created with pixbuf_cairo_create().
 *
 * Return value: New or updated GdkPixbuf. You own a new reference on return
 * value, so you need to call g_object_unref() on returned pixbuf when you don't
 * need it anymore.
 */
GdkPixbuf *
pixbuf_cairo_destroy( cairo_t  *cr,
                         gboolean  create_new_pixbuf )
{
    gint             width,        /* Width of both pixbuf and surface */
                     height,       /* Height of both pixbuf and surface */
                     p_stride,     /* Pixbuf stride value */
                     p_n_channels, /* RGB -> 3, RGBA -> 4 */
                     s_stride;     /* Surface stride value */
    guchar          *p_pixels,     /* Pixbuf's pixel data */
                    *s_pixels;     /* Surface's pixel data */
    cairo_surface_t *surface;      /* Temporary image surface */
    GdkPixbuf       *pixbuf,       /* Pixbuf to be returned */
                    *tmp_pix;      /* Temporary storage */

    /* Obtain pixbuf to be returned */
    tmp_pix = cairo_get_user_data( cr, &pixbuf_key );
    if( create_new_pixbuf )
        pixbuf = gdk_pixbuf_copy( tmp_pix );
    else
        pixbuf = g_object_ref( G_OBJECT( tmp_pix ) );

    /* Obtain surface from where pixel values will be copied */
    surface = cairo_get_target( cr );

    /* Inspect pixbuf and surface */
    g_object_get( G_OBJECT( pixbuf ), "width",           &width,
                                      "height",          &height,
                                      "rowstride",       &p_stride,
                                      "n-channels",      &p_n_channels,
                                      "pixels",          &p_pixels,
                                      NULL );
    s_stride = cairo_image_surface_get_stride( surface );
    s_pixels = cairo_image_surface_get_data( surface );

    /* Copy pixel data from surface to pixbuf */
    while( height-- )
    {
        gint    i;
        guchar *p_iter = p_pixels,
               *s_iter = s_pixels;

        for( i = 0; i < width; i++ )
        {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
            /* Pixbuf:  RGB(A)
             * Surface: BGRA */
            gdouble alpha_factor = (gdouble)0xff / s_iter[3];

            p_iter[0] = (guchar)( s_iter[2] * alpha_factor + .5 );
            p_iter[1] = (guchar)( s_iter[1] * alpha_factor + .5 );
            p_iter[2] = (guchar)( s_iter[0] * alpha_factor + .5 );
            if( p_n_channels == 4 )
                p_iter[3] = s_iter[3];
#elif G_BYTE_ORDER == G_BIG_ENDIAN
            /* Pixbuf:  RGB(A)
             * Surface: ARGB */
            gdouble alpha_factor = (gdouble)0xff / s_iter[0];

            p_iter[0] = (guchar)( s_iter[1] * alpha_factor + .5 );
            p_iter[1] = (guchar)( s_iter[2] * alpha_factor + .5 );
            p_iter[2] = (guchar)( s_iter[3] * alpha_factor + .5 );
            if( p_n_channels == 4 )
                p_iter[3] = s_iter[0];
#else /* PDP endianness */
            /* Pixbuf:  RGB(A)
             * Surface: RABG */
            gdouble alpha_factor = (gdouble)0xff / s_iter[1];

            p_iter[0] = (guchar)( s_iter[0] * alpha_factor + .5 );
            p_iter[1] = (guchar)( s_iter[3] * alpha_factor + .5 );
            p_iter[2] = (guchar)( s_iter[2] * alpha_factor + .5 );
            if( p_n_channels == 4 )
                p_iter[3] = s_iter[1];
#endif
            s_iter += 4;
            p_iter += p_n_channels;
        }
        s_pixels += s_stride;
        p_pixels += p_stride;
    }

    /* Destroy context */
    cairo_destroy( cr );

    /* Return pixbuf */
    return( pixbuf );
}
