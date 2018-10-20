/*
 * wifi-signal-applet: Shows the Wi-Fi signal in the status area
 *
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
 
/* wifi-signal-cpa.c */

#include <stdlib.h>
#include <libintl.h>
#include <glib.h>
#include <libosso.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <hildon-cp-plugin/hildon-cp-plugin-interface.h>

#include "gconf-keys.h"

static GtkWidget *dialog;

static GConfClient *gconf_client;
  
static GtkWidget *mintime_entry;
static GtkWidget *maxtime_entry;

static GtkWidget *min_caption;
static GtkWidget *max_caption;

static GtkWidget *maxtimeaddlabel;

static HildonSizeType default_size = HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT;

static void gtk_entry_set_text_printf (GtkEntry *entry, const gchar *format, ...) G_GNUC_PRINTF (2, 3);

static void gtk_entry_set_text_printf (GtkEntry *entry, const gchar *format, ...)
{
	va_list args;
	gchar *text;

	va_start (args, format);
	text = g_strdup_vprintf (format, args);
	va_end (args);
	
	gtk_entry_set_text (entry, text);
	
	g_free (text);
}

osso_return_t execute (osso_context_t *osso G_GNUC_UNUSED, gpointer user_data, gboolean user_activated G_GNUC_UNUSED)
{
	GtkWidget *content_area;

	gconf_client = gconf_client_get_default ();

	dialog = gtk_dialog_new_with_buttons ("Wi-Fi Signal Applet Settings", GTK_WINDOW (user_data), GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR, dgettext ("hildon-libs", "wdgt_bd_save"), GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

	mintime_entry = hildon_entry_new (default_size);
	maxtime_entry = hildon_entry_new (default_size);
	hildon_gtk_entry_set_input_mode (GTK_ENTRY (mintime_entry), HILDON_GTK_INPUT_MODE_NUMERIC);
	hildon_gtk_entry_set_input_mode (GTK_ENTRY (maxtime_entry), HILDON_GTK_INPUT_MODE_NUMERIC);

	gtk_entry_set_text_printf (GTK_ENTRY (mintime_entry), "%d", gconf_client_get_int (gconf_client, WIFI_SIGNAL_APPLET_MIN_TIME_GCONF_KEY, NULL));
	gtk_entry_set_text_printf (GTK_ENTRY (maxtime_entry), "%d", gconf_client_get_int (gconf_client, WIFI_SIGNAL_APPLET_MAX_TIME_GCONF_KEY, NULL));

	min_caption = hildon_caption_new (NULL, "Min. time before updating", mintime_entry, NULL, HILDON_CAPTION_MANDATORY);
	max_caption = hildon_caption_new (NULL, "Max. time before updating", maxtime_entry, NULL, HILDON_CAPTION_MANDATORY);

	maxtimeaddlabel = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (maxtimeaddlabel), "<small>It's advisable to, naturally, have Max. time larger than Min. Time.</small>");
	gtk_misc_set_alignment (GTK_MISC (maxtimeaddlabel), 0, 0);

	gtk_box_pack_start (GTK_BOX (content_area), min_caption, TRUE, TRUE, HILDON_MARGIN_DEFAULT);
	gtk_box_pack_start (GTK_BOX (content_area), max_caption, TRUE, TRUE, HILDON_MARGIN_DEFAULT);
	gtk_container_add (GTK_CONTAINER (content_area), maxtimeaddlabel);

	gtk_widget_show_all (content_area);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK);
	{
		gint mintime = atoi (gtk_entry_get_text (GTK_ENTRY (mintime_entry)));
		gint maxtime = atoi (gtk_entry_get_text (GTK_ENTRY (maxtime_entry)));

		gconf_client_set_int (gconf_client, WIFI_SIGNAL_APPLET_MIN_TIME_GCONF_KEY, mintime, NULL);
		gconf_client_set_int (gconf_client, WIFI_SIGNAL_APPLET_MAX_TIME_GCONF_KEY, maxtime, NULL);
	}

	gtk_widget_destroy (dialog);

	g_object_unref (gconf_client);

	return OSSO_OK;
}

osso_return_t save_state (osso_context_t *osso G_GNUC_UNUSED, gpointer data G_GNUC_UNUSED)
{
	return OSSO_OK;
}
