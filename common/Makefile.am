CLEANFILES =

lib_LTLIBRARIES = libopen-app-common.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	$(LIBSOUP_CFLAGS)                                       \
	$(GNOME_APP_STORE_CFLAGS)				\
	-DCONFIGDIR=\""/etc/open-app-store\""                  \
	$(DEBUG_CFLAGS) 					\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(NULL)

AM_CFLAGS = $(WARN_CFLAGS)

libopen_app_common_la_SOURCES = 	\
	open-services.h \
	open-request.c open-request.h \
	open-result.c open-result.h \
	open-results.c open-results.h \
	open-app-config.c open-app-config.h \
	open-app-utils.c open-app-utils.h \
	open-app-install.c open-app-install.h \
	$(NULL)

libopen_app_common_la_LIBADD =	\
	$(LIBSOUP_LIBS) \
	$(GNOME_APP_STORE_LIBS)

