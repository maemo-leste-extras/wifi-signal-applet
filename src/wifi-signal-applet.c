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

/* wifi-signal-applet.c */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <icd/network_api.h>
#include <icd/dbus_api.h>
#include <string.h>
#include <glib.h>
#include <glib-object.h>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include <dbus/dbus-glib.h>
#include <gconf/gconf-client.h>
#include <conic/conic.h>
#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <libhildondesktop/libhildondesktop.h>

#include "icd2-stats-sig-marshal.h"
#include "utils.h"
#include "gconf-keys.h"

#define UNLIKELY_RETURN(thing) if (G_UNLIKELY (!(thing))) return;

/* Oh, this one's a fucker. This is one of the few, rare moments where I actually prefer libdbus directly to dbus-glib */
#define ICD2_STATS_SIGNAL_G_TYPES G_TYPE_STRING, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT, DBUS_TYPE_G_UCHAR_ARRAY, G_TYPE_UINT, G_TYPE_INT, G_TYPE_UINT, G_TYPE_UINT

/* Start of GObject boilerplate shit */
GType wifi_signal_applet_get_type (void);

#define WIFI_TYPE_SIGNAL_APPLET (wifi_signal_applet_get_type ())

#define WIFI_SIGNAL_APPLET(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
						WIFI_TYPE_SIGNAL_APPLET, WifiSignalApplet))

#define WIFI_SIGNAL_APPLET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), \
						WIFI_TYPE_SIGNAL_APPLET, WifiSignalAppletClass))

#define WIFI_IS_SIGNAL_APPLET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
						WIFI_TYPE_SIGNAL_APPLET))

#define WIFI_IS_SIGNAL_APPLET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
						WIFI_TYPE_SIGNAL_APPLET))

#define WIFI_SIGNAL_APPLET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), \
						WIFI_TYPE_SIGNAL_APPLET, WifiSignalAppletClass))

#define WIFI_SIGNAL_APPLET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
						WIFI_TYPE_SIGNAL_APPLET, WifiSignalAppletPrivate))

typedef struct _WifiSignalApplet WifiSignalApplet;
typedef struct _WifiSignalAppletClass WifiSignalAppletClass;
typedef struct _WifiSignalAppletPrivate WifiSignalAppletPrivate;

struct _WifiSignalApplet
{
	HDStatusPluginItem parent;

	WifiSignalAppletPrivate *priv;
};

struct _WifiSignalAppletClass
{
	HDStatusPluginItemClass parent_class;
};

struct _WifiSignalAppletPrivate
{
	DBusGConnection *sys_dbus_conn;
	DBusGProxy *icd2_proxy;
	ConIcConnection *con;
	GConfClient *gconf_client;

	gboolean iap_connected;
	gboolean is_status_area_visible;
	gint last_signal_strength;

	guint gconfnotify_id;
	guint timeout_src;
	guint mintime;
	guint maxtime;

	GdkPixbuf *pixbuf;
};

HD_DEFINE_PLUGIN_MODULE (WifiSignalApplet, wifi_signal_applet, HD_TYPE_STATUS_PLUGIN_ITEM)
/* End of GObject boilerplate shit */

static void wifi_signal_applet_on_stats_rec_cb (DBusGProxy *object G_GNUC_UNUSED, const gchar *service_type G_GNUC_UNUSED, const guint service_attributes G_GNUC_UNUSED, const gchar *service_id G_GNUC_UNUSED, const gchar *network_type G_GNUC_UNUSED, const guint network_attributes G_GNUC_UNUSED, const GArray *network_id G_GNUC_UNUSED, const guint time_active G_GNUC_UNUSED, const enum icd_nw_levels signal_strength, const guint bytes_sent G_GNUC_UNUSED, const guint bytes_recieved G_GNUC_UNUSED, WifiSignalApplet *plugin);
static void wifi_signal_applet_remove_timeout (WifiSignalApplet *plugin);
static void wifi_signal_applet_status_area_visible_cb (GObject *object, GParamSpec *pspec, gpointer user_data G_GNUC_UNUSED);
static void wifi_signal_applet_iap_connect_event_cb (ConIcConnection *con G_GNUC_UNUSED, ConIcConnectionEvent *e, WifiSignalApplet *plugin);

static void wifi_signal_applet_update_image (WifiSignalApplet *plugin, const gchar *text)
{
	GdkPixbuf   *new_pix;
	PangoLayout *layout;
	cairo_t     *cr;
	guint16     red, green, blue;
	gboolean    colour;
	const PangoFontDescription* pfd = NULL;

	gdk_pixbuf_fill (plugin->priv->pixbuf, 0x00000000);

	cr = pixbuf_cairo_create (plugin->priv->pixbuf);
	colour = he_helper_get_logical_font_colour ("DefaultTextColor", &red, &green, &blue);
	if (colour)
		cairo_set_source_rgb (cr, red, green, blue);
	else
		cairo_set_source_rgb (cr, 0, 0, 0);

	cairo_move_to (cr, 0, 0);

	layout = pango_cairo_create_layout (cr);
	pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
	if ((pfd = he_helper_get_logical_font_desc ("SmallSystemFont")))
		pango_layout_set_font_description (layout, pfd);
	pango_layout_set_text (layout, text, 2);
	pango_cairo_update_layout (cr, layout);
	pango_cairo_show_layout (cr, layout);

	g_object_unref (layout);
	new_pix = pixbuf_cairo_destroy (cr, FALSE);
	g_object_unref (new_pix);

	hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (plugin), plugin->priv->pixbuf);
}

/* Where it all falls down */
static void wifi_signal_applet_finalize (GObject *object)
{
	WifiSignalApplet *plugin = WIFI_SIGNAL_APPLET (object);

	if (plugin->priv->con)
	{
		g_object_set (plugin->priv->con, "automatic-connection-events", FALSE, NULL);
		g_signal_handlers_disconnect_by_func (plugin->priv->con, G_CALLBACK (wifi_signal_applet_iap_connect_event_cb), plugin);
		g_object_unref (plugin->priv->con);
		plugin->priv->con = NULL;
	}

	wifi_signal_applet_remove_timeout (plugin);

	if (plugin->priv->pixbuf)
	{
		//hildon_banner_show_informationf (NULL, NULL, "%u", G_OBJECT (plugin->priv->pixbuf)->ref_count);
		g_object_unref (plugin->priv->pixbuf);
		plugin->priv->pixbuf = NULL;
	}

	if (plugin->priv->icd2_proxy)
	{
		dbus_g_proxy_disconnect_signal (plugin->priv->icd2_proxy, ICD_DBUS_API_STATISTICS_SIG, G_CALLBACK (wifi_signal_applet_on_stats_rec_cb), plugin);
		g_object_unref (plugin->priv->icd2_proxy);
		plugin->priv->icd2_proxy = NULL;
	}

	if (plugin->priv->gconfnotify_id)
	{
		gconf_client_notify_remove (plugin->priv->gconf_client, plugin->priv->gconfnotify_id);
		plugin->priv->gconfnotify_id = 0;
		gconf_client_remove_dir (plugin->priv->gconf_client, WIFI_SIGNAL_APPLET_GCONF_FOLDER, NULL);
	}

	if (plugin->priv->gconf_client)
	{
		gconf_client_clear_cache (plugin->priv->gconf_client);
		g_object_unref (plugin->priv->gconf_client);
		plugin->priv->gconf_client = NULL;
	}

	/* if (plugin->priv->sys_dbus_conn)
		dbus_g_connection_unref (plugin->priv->sys_dbus_conn); */ /* Allegedly, hd_status_plugin_item_get_dbus_g_connection () returns a shared connection. */

	g_signal_handlers_disconnect_by_func (plugin, G_CALLBACK (wifi_signal_applet_status_area_visible_cb), NULL);

	if (G_OBJECT_CLASS (wifi_signal_applet_parent_class)->finalize)
		G_OBJECT_CLASS (wifi_signal_applet_parent_class)->finalize (object);
}

/* -- */

static gboolean wifi_signal_applet_send_stats_req (WifiSignalApplet *plugin)
{
	dbus_g_proxy_call_no_reply (plugin->priv->icd2_proxy, ICD_DBUS_API_STATISTICS_REQ, G_TYPE_INVALID);

	return TRUE;
}

static void wifi_signal_applet_remove_timeout (WifiSignalApplet *plugin)
{
	if (plugin->priv->timeout_src)
	{
		g_source_destroy (g_main_context_find_source_by_id (NULL, plugin->priv->timeout_src));
		plugin->priv->timeout_src = 0;
	}
}

static void wifi_signal_applet_add_timeout (WifiSignalApplet *plugin)
{
	wifi_signal_applet_remove_timeout (plugin);
	if (plugin->priv->is_status_area_visible && plugin->priv->iap_connected)
		plugin->priv->timeout_src = hd_status_plugin_item_heartbeat_signal_add (HD_STATUS_PLUGIN_ITEM (plugin), plugin->priv->mintime, plugin->priv->maxtime, (GSourceFunc) wifi_signal_applet_send_stats_req, plugin, NULL);
	else if (!plugin->priv->iap_connected)
	{
		hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (plugin), NULL);
		plugin->priv->last_signal_strength = -1;
	}
}

static void wifi_signal_applet_on_stats_rec_cb (DBusGProxy *object G_GNUC_UNUSED, const gchar *service_type G_GNUC_UNUSED, const guint service_attributes G_GNUC_UNUSED, const gchar *service_id G_GNUC_UNUSED, const gchar *network_type G_GNUC_UNUSED, const guint network_attributes G_GNUC_UNUSED, const GArray *network_id G_GNUC_UNUSED, const guint time_active G_GNUC_UNUSED, const enum icd_nw_levels signal_strength, const guint bytes_sent G_GNUC_UNUSED, const guint bytes_recieved G_GNUC_UNUSED, WifiSignalApplet *plugin)
{
	if (signal_strength != plugin->priv->last_signal_strength)
	{
		plugin->priv->last_signal_strength = signal_strength;

		gchar signal_strength_str[3]; /* If it's any bigger, we're fucked... not to mention that it wouldn't fit anyway. No, I do not wish to g_strdup_printf () and g_free () each time for something that should be not be >2 */
		g_snprintf (signal_strength_str, sizeof (signal_strength_str), "%u", signal_strength);

		wifi_signal_applet_update_image (plugin, signal_strength_str);
	}
}

static void wifi_signal_applet_iap_connect_event_cb (ConIcConnection *con G_GNUC_UNUSED, ConIcConnectionEvent *e, WifiSignalApplet *plugin)
{
	plugin->priv->iap_connected = con_ic_connection_event_get_status (e) == CON_IC_STATUS_CONNECTED && g_str_equal (con_ic_event_get_bearer_type (CON_IC_EVENT (e)), CON_IC_BEARER_WLAN_INFRA);
	if (plugin->priv->iap_connected)
		(void) wifi_signal_applet_send_stats_req (plugin);

	wifi_signal_applet_add_timeout (plugin);
}

static void wifi_signal_applet_status_area_visible_cb (GObject *object, GParamSpec *pspec, gpointer user_data G_GNUC_UNUSED)
{
	WifiSignalApplet *plugin = WIFI_SIGNAL_APPLET (object);

	g_object_get (object, pspec->name, &plugin->priv->is_status_area_visible, NULL);
	wifi_signal_applet_add_timeout (plugin);
}

static void wifi_signal_applet_get_times_from_keys (WifiSignalApplet *plugin)
{
	GError *error = NULL;

	plugin->priv->mintime = gconf_client_get_int (plugin->priv->gconf_client, WIFI_SIGNAL_APPLET_MIN_TIME_GCONF_KEY, NULL); /* Returns 0 if fail */
	plugin->priv->maxtime = gconf_client_get_int (plugin->priv->gconf_client, WIFI_SIGNAL_APPLET_MAX_TIME_GCONF_KEY, &error);
	if (error)
	{
		plugin->priv->maxtime = MAX_TIME_DEFAULT;
		g_error_free (error);
	}
}

static void wifi_signal_applet_key_changed (GConfClient *gconf_client G_GNUC_UNUSED, guint cnxn_id G_GNUC_UNUSED, GConfEntry *entry G_GNUC_UNUSED, WifiSignalApplet *plugin)
{
	wifi_signal_applet_get_times_from_keys (plugin);
	wifi_signal_applet_add_timeout (plugin);
}

static void wifi_signal_applet_constructed (GObject *object)
{
	WifiSignalApplet *plugin = WIFI_SIGNAL_APPLET (object);

	if (G_OBJECT_CLASS (wifi_signal_applet_parent_class)->constructed)
		G_OBJECT_CLASS (wifi_signal_applet_parent_class)->constructed (object);

	if (plugin->priv->gconf_client)
	{
 		GError *error = NULL;

		gconf_client_add_dir (plugin->priv->gconf_client, WIFI_SIGNAL_APPLET_GCONF_FOLDER, GCONF_CLIENT_PRELOAD_NONE, &error);
		if (!error)
		{
			plugin->priv->gconfnotify_id = gconf_client_notify_add (plugin->priv->gconf_client, WIFI_SIGNAL_APPLET_GCONF_FOLDER, (GConfClientNotifyFunc) wifi_signal_applet_key_changed, plugin, NULL, &error);
			if (error)
			{
				gconf_client_remove_dir (plugin->priv->gconf_client, WIFI_SIGNAL_APPLET_GCONF_FOLDER, NULL);
				g_error_free (error);
			}
		}
		else
			g_error_free (error);
	}

	g_signal_connect (plugin, "notify::status-area-visible", G_CALLBACK (wifi_signal_applet_status_area_visible_cb), NULL);
	g_object_get (plugin, "status-area-visible", &plugin->priv->is_status_area_visible, NULL);

	dbus_g_proxy_add_signal (plugin->priv->icd2_proxy, ICD_DBUS_API_STATISTICS_SIG, ICD2_STATS_SIGNAL_G_TYPES, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (plugin->priv->icd2_proxy, ICD_DBUS_API_STATISTICS_SIG, G_CALLBACK (wifi_signal_applet_on_stats_rec_cb), plugin, NULL);

	g_signal_connect (plugin->priv->con, "connection-event", G_CALLBACK (wifi_signal_applet_iap_connect_event_cb), plugin); 
	g_object_set (plugin->priv->con, "automatic-connection-events", TRUE, NULL);
}

static void wifi_signal_applet_init (WifiSignalApplet *plugin)
{
	plugin->priv = WIFI_SIGNAL_APPLET_GET_PRIVATE (plugin);
	UNLIKELY_RETURN (plugin->priv);
	memset (plugin->priv, 0, sizeof (WifiSignalAppletPrivate));

	plugin->priv->last_signal_strength = -1;

	plugin->priv->sys_dbus_conn = hd_status_plugin_item_get_dbus_g_connection (HD_STATUS_PLUGIN_ITEM (plugin), DBUS_BUS_SYSTEM, NULL);
	UNLIKELY_RETURN (plugin->priv->sys_dbus_conn);

	plugin->priv->gconf_client = gconf_client_get_default ();
	if (G_UNLIKELY (!plugin->priv->gconf_client)) 
	{
		plugin->priv->mintime = MIN_TIME_DEFAULT;
		plugin->priv->maxtime = MAX_TIME_DEFAULT;
	}
	else
		wifi_signal_applet_get_times_from_keys (plugin);

	plugin->priv->icd2_proxy = dbus_g_proxy_new_for_name (plugin->priv->sys_dbus_conn, ICD_DBUS_API_INTERFACE, ICD_DBUS_API_PATH, ICD_DBUS_API_INTERFACE);
	UNLIKELY_RETURN (plugin->priv->icd2_proxy);

	plugin->priv->pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, 18, 18);

	plugin->priv->con = con_ic_connection_new ();
	UNLIKELY_RETURN (plugin->priv->con);

	dbus_g_object_register_marshaller (icd2_stats_marshal_VOID__STRING_UINT_STRING_STRING_UINT_BOXED_UINT_INT_UINT_UINT, G_TYPE_NONE, ICD2_STATS_SIGNAL_G_TYPES, G_TYPE_INVALID);
}

/* ---- */
static void wifi_signal_applet_class_init (WifiSignalAppletClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = (GObjectFinalizeFunc) wifi_signal_applet_finalize;
	object_class->constructed = wifi_signal_applet_constructed;

	g_type_class_add_private (klass, sizeof (WifiSignalAppletPrivate));
}

static void wifi_signal_applet_class_finalize (WifiSignalAppletClass *klass G_GNUC_UNUSED)
{
}

