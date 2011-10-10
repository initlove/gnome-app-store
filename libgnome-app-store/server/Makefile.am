CLEANFILES =

lib_LTLIBRARIES = libapp-server.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	-I$(top_srcdir)/libgnome-app-store/common		\
	-I$(top_srcdir)/libgnome-app-store			\
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
	ocs-server.c ocs-server.h	\
	app-stream-server.c app-stream-server.h	\
	$(NULL)

libapp_server_la_LIBADD =	\
	$(GNOME_APP_STORE_LIBS)

noinst_PROGRAMS = \
	test_server

test_server_SOURCES =      \
        test_server.c

test_server_LDADD =        \
	../common/libgnome-app-common.la \
	libapp-server.la	\
        $(GNOME_APP_STORE_LIBS)

