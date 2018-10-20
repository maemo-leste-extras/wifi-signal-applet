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

/* gconf-keys.h */

#ifndef __GCONF_H__
#define __GCONF_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define WIFI_SIGNAL_APPLET_MIN_TIME_GCONF_KEY WIFI_SIGNAL_APPLET_GCONF_FOLDER MIN_TIME_GCONF_KEY
#define WIFI_SIGNAL_APPLET_MAX_TIME_GCONF_KEY WIFI_SIGNAL_APPLET_GCONF_FOLDER MAX_TIME_GCONF_KEY

#endif /* __GCONF_H__ */
