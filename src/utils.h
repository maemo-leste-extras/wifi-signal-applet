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

/* utils.h */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <glib.h>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include <gdk/gdk.h>

G_BEGIN_DECLS

const PangoFontDescription* he_helper_get_logical_font_desc (const gchar *name);

gboolean he_helper_get_logical_font_colour (const gchar *name, guint16 *red, guint16 *green, guint16 *blue);

cairo_t* pixbuf_cairo_create (GdkPixbuf *pixbuf);

GdkPixbuf* pixbuf_cairo_destroy (cairo_t *cr, gboolean create_new_pixbuf);

G_END_DECLS

#endif /* __UTILS_H__ */
