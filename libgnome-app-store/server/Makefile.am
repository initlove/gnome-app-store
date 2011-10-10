CLEANFILES =

lib_LTLIBRARIES = libapp-server.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	-I$(top_srcdir)/libgnome-app-store/common		\
	-I$(top_srcdir)/libgnome-app-store			\
	$(LIBSOUP_CFLAGS)					\
	$(GNOME_APP_STORE_CFLAGS)				\
	-DCONFIGDIR=\""/etc/gnome-app-store\""                  \
	-DG_LOG_DOMAIN=\"GnomeAppStore\"			\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""	\
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(NULL)

AM_CFLAGS = $(WARN_CFLAGS)

libapp_server_la_SOURCES = 	\
	app-server.c app-server.h \
	ocs-app.c ocs-app.h \
	ocs-server.c ocs-server.h	\
	app-stream-server.c app-stream-server.h	\
	$(NULL)

libapp_server_la_LIBADD =	\
	$(LIBSOUP_LIBS)	\
	$(GNOME_APP_STORE_LIBS)	\
	$(NULL)

noinst_PROGRAMS = \
	test_server

test_server_SOURCES =      \
        test_server.c

test_server_LDADD =        \
	../common/libgnome-app-common.la \
	libapp-server.la	\
        $(GNOME_APP_STORE_LIBS)

