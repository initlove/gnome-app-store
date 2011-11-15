CLEANFILES =

lib_LTLIBRARIES = libgnome-app-common.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	$(LIBSOUP_CFLAGS)                                       \
	$(GNOME_APP_STORE_CFLAGS)				\
	-DCONFIGDIR=\""/etc/gnome-app-store\""                  \
	-DG_LOG_DOMAIN=\"GnomeAppStore\"			\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""	\
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	$(DEBUG_CFLAGS) 					\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(NULL)

AM_CFLAGS = $(WARN_CFLAGS)

libgnome_app_common_la_SOURCES = 	\
	open-services.h \
	open-request.c open-request.h \
	open-result.c open-result.h \
	open-results.c open-results.h \
	gnome-app-config.c gnome-app-config.h \
	gnome-app-utils.c gnome-app-utils.h \
	gnome-app-install.c gnome-app-install.h \
	$(NULL)

libgnome_app_common_la_LIBADD =	\
	$(LIBSOUP_LIBS) \
	$(GNOME_APP_STORE_LIBS)

