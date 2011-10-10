CLEANFILES =

lib_LTLIBRARIES = libgnome-app-common.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	$(GNOME_APP_STORE_CFLAGS)				\
	-DCONFIGDIR=\""/etc/gnome-app-store\""                  \
	-DG_LOG_DOMAIN=\"GnomeAppStore\"			\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""	\
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(NULL)

AM_CFLAGS = $(WARN_CFLAGS)

libgnome_app_common_la_SOURCES = 	\
	gnome-app-config.c gnome-app-config.h \
	gnome-app-item.c gnome-app-item.h \
	gnome-app-utils.c gnome-app-utils.h \
	local-app.c local-app.h \
	gnome-app-install.c gnome-app-install.h \
	$(NULL)

libgnome_app_common_la_LIBADD =	\
	$(GNOME_APP_STORE_LIBS)

