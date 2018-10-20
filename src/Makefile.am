INCLUDES = -DG_LOG_DOMAIN=\"@PACKAGE@\"

desktoplib_LTLIBRARIES = wifi-signal-applet.la
desktoplibdir = $(HILDON_DESKTOP_LIB_DIR)

MARSHAL_SRC = icd2-stats-sig-marshal.h icd2-stats-sig-marshal.c

cpalib_LTLIBRARIES = wifi-signal-cpa.la
cpalibdir = $(CONTROL_PANEL_LIB_DIR)

wifi_signal_cpa_la_SOURCES = wifi-signal-cpa.c
wifi_signal_cpa_la_CFLAGS = $(WIFISIGNALCPA_CFLAGS)
wifi_signal_cpa_la_LDFLAGS = -module -avoid-version
wifi_signal_cpa_la_LIBADD = $(WIFISIGNALCPA_LIBS)

wifi_signal_applet_la_SOURCES = $(MARSHAL_SRC) utils.h utils.c wifi-signal-applet.c
wifi_signal_applet_la_CFLAGS = $(WIFISIGNALAPPLET_CFLAGS)
wifi_signal_applet_la_LDFLAGS = -module -avoid-version
wifi_signal_applet_la_LIBADD = $(WIFISIGNALAPPLET_LIBS)

icd2-stats-sig-marshal.h: icd2-stats-sig-marshal.list
	$(GLIBGENMARSHAL) --prefix=icd2_stats_marshal $< --header > $@

icd2-stats-sig-marshal.c: icd2-stats-sig-marshal.list icd2-stats-sig-marshal.h
	echo '#include "icd2-stats-sig-marshal.h"' > $@
	$(GLIBGENMARSHAL) --prefix=icd2_stats_marshal $< --body >> $@

noinst_HEADERS = $(wifi_signal_applet_la_SOURCES)

EXTRA_DIST = icd2-stats-sig-marshal.list
CLEANFILES = $(MARSHAL_SRC)
