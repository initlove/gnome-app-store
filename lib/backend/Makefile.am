CLEANFILES =

lib_LTLIBRARIES = libapp-backend.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	-I$(top_srcdir)/common					\
	$(LIBSOUP_CFLAGS)					\
	$(GNOME_APP_STORE_CFLAGS)				\
	-DCONFIGDIR=\""/etc/gnome-app-store\""                  \
	-DG_LOG_DOMAIN=\"GnomeAppStore\"			\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""	\
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	-DG_ENABLE_DEBUG					\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(NULL)

AM_CFLAGS = $(WARN_CFLAGS)

libapp_backend_la_SOURCES = 	\
	app-backend.c app-backend.h \
	ocs-app.c ocs-app.h \
	ocs-backend.c ocs-backend.h	\
	app-stream-backend.c app-stream-backend.h	\
	$(NULL)

libapp_backend_la_LIBADD =	\
	$(LIBSOUP_LIBS)	\
	$(GNOME_APP_STORE_LIBS)	\
	$(NULL)

noinst_PROGRAMS = \
	test-backend

test_backend_SOURCES =      \
        test-backend.c

test_backend_LDADD =        \
	$(top_srcdir)/common/libgnome-app-common.la \
	libapp-backend.la	\
        $(GNOME_APP_STORE_LIBS)

