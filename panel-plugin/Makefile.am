AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"xfce4-workspacebar-plugin\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	$(PLATFORM_CPPFLAGS)

#
# workspacebar plugin
#
plugin_LTLIBRARIES = \
	libworkspacebar.la

plugindir = \
	$(libdir)/xfce4/panel/plugins

libworkspacebar_la_SOURCES = \
	workspacebar.c \
	workspacebar.h \
	workspacebar-dialogs.c \
	workspacebar-dialogs.h

libworkspacebar_la_CFLAGS = \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS) \
	$(LIBWNCK_CFLAGS) \
	-DWNCK_I_KNOW_THIS_IS_UNSTABLE \
	$(PLATFORM_CFLAGS)

libworkspacebar_la_LDFLAGS = \
       -avoid-version \
       -module \
       -no-undefined \
       -export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
       $(PLATFORM_LDFLAGS)

libworkspacebar_la_LIBADD = \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBWNCK_LIBS) \
	$(LIBXFCE4PANEL_LIBS)

#
# Desktop file
#
desktopdir =								\
	$(datadir)/xfce4/panel/plugins

desktop_DATA =								\
	workspacebar.desktop

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST =								\
	workspacebar.desktop.in

CLEANFILES =								\
	$(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
