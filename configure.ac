AC_INIT(m4_esyscmd([dpkg-parsechangelog | grep ^Source:.* | cut -d ' ' -f 2 | tr -d '\n']), m4_esyscmd([dpkg-parsechangelog | grep ^Version:.* | cut -d ' ' -f 2 | tr -d '\n']), m4_esyscmd([dpkg-parsechangelog | grep ^Maintainer:.* | awk '{print $2 " "$3}' | tr -d '\n']))
AC_CONFIG_SRCDIR([src/wifi-signal-applet.c])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AM_PROG_CC_C_O
AC_HEADER_STDC
AM_PATH_GLIB_2_0([2.20.3])
AM_PATH_GTK_2_0([2.14.7])
AC_PROG_INSTALL
AC_DISABLE_STATIC
AM_PROG_LIBTOOL
AC_PREFIX_DEFAULT(/usr)

# AC_PREFIX_DEFAULT does not get expanded until too late so we need to do this to use prefix in this script --conny 
if test "x$prefix" = "xNONE" ; then
	prefix='/usr'
fi

PKG_CHECK_MODULES([WIFISIGNALAPPLET], [glib-2.0 cairo pango gobject-2.0 icd2 dbus-glib-1 gconf-2.0 conic gtk+-2.0 hildon-1 libhildondesktop-1])
AC_SUBST([WIFISIGNALAPPLET_CFLAGS])
AC_SUBST([WIFISIGNALAPPLET_LIBS])

PKG_CHECK_MODULES([WIFISIGNALCPA], [glib-2.0 libosso gconf-2.0 gtk+-2.0 hildon-1 hildon-control-panel])
AC_SUBST([WIFISIGNALCPA_CFLAGS])
AC_SUBST([WIFISIGNALCPA_LIBS])

HILDON_DESKTOP_LIB_DIR=`pkg-config libhildondesktop-1 --variable=hildondesktoplibdir`
AC_SUBST([HILDON_DESKTOP_LIB_DIR])

HILDON_HOME_DESKTOP_DIR=`pkg-config libhildondesktop-1 --variable=hildonstatusmenudesktopentrydir`
AC_SUBST(HILDON_HOME_DESKTOP_DIR)

CONTROL_PANEL_LIB_DIR=`pkg-config hildon-control-panel --variable=pluginlibdir`
AC_SUBST([CONTROL_PANEL_LIB_DIR])

CONTROL_PANEL_DESKTOP_DIR=`pkg-config hildon-control-panel --variable=plugindesktopentrydir`
AC_SUBST([CONTROL_PANEL_DESKTOP_DIR])

AC_PATH_PROG([GLIBGENMARSHAL], [`$PKG_CONFIG glib-2.0 --variable=glib_genmarshal`])
AC_SUBST([GLIBGENMARSHAL])

WIFI_SIGNAL_APPLET_GCONF_FOLDER="/apps/wifi-signal-applet"
AC_DEFINE_UNQUOTED(WIFI_SIGNAL_APPLET_GCONF_FOLDER, "$WIFI_SIGNAL_APPLET_GCONF_FOLDER", [GConf folder in which Wi-Fi Signal Applet will store its settings])
AC_SUBST([WIFI_SIGNAL_APPLET_GCONF_FOLDER])

MIN_TIME_GCONF_KEY="/min-time"
AC_DEFINE_UNQUOTED(MIN_TIME_GCONF_KEY, "$MIN_TIME_GCONF_KEY", [GConf key in which the setting for the minimum time before calling the timeout CB is stored])
AC_SUBST([MIN_TIME_GCONF_KEY])

MIN_TIME_DEFAULT=0
AC_DEFINE_UNQUOTED(MIN_TIME_DEFAULT, $MIN_TIME_DEFAULT, [Self-explanatory])
AC_SUBST([MIN_TIME_DEFAULT])

MAX_TIME_GCONF_KEY="/max-time"
AC_DEFINE_UNQUOTED(MAX_TIME_GCONF_KEY, "$MAX_TIME_GCONF_KEY", [GConf key in which the setting for the maximum time before calling the timeout CB is stored])
AC_SUBST([MAX_TIME_GCONF_KEY])

MAX_TIME_DEFAULT=2
AC_DEFINE_UNQUOTED(MAX_TIME_DEFAULT, $MAX_TIME_DEFAULT, [Self-explanatory])
AC_SUBST([MAX_TIME_DEFAULT])

CFLAGS="$CFLAGS -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS -Wall -Wmissing-prototypes -Wmissing-declarations -Wno-format"

AC_OUTPUT([debian/schemas debian/postinst debian/prerm debian/postrm data/Makefile src/Makefile Makefile])
