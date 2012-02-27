CLEANFILES =

noinst_LTLIBRARIES = libwidgets.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	-I$(top_srcdir)/common					\
	$(GNOME_APP_STORE_CFLAGS)                               \
	$(LIBREST_CFLAGS)                                       \
	$(COMMON_CFLAGS)					\
	-DG_LOG_DOMAIN=\"GnomeAppStore\"			\
	$(NULL)

libwidgets_la_SOURCES = \
	gnome-app-widgets.c gnome-app-widgets.h \
	gnome-app-widget.c gnome-app-widget.h \
	gnome-app-text.c gnome-app-text.h \
	gnome-app-check-box.c gnome-app-check-box.h \
	gnome-app-button.c gnome-app-button.h \
	gnome-app-entry.c gnome-app-entry.h \
	gnome-app-texture.c gnome-app-texture.h \
	$(NULL)

libwidgets_la_LIBADD =	\
	$(top_srcdir)/common/libopen-app-common.la \
	$(LIBREST_LIBS) \
	$(GNOME_APP_STORE_LIBS)			\
	$(COMMON_LIBS)			\
	$(NULL)

