CLEANFILES =

lib_LTLIBRARIES = libopen-app-common.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	-DCONFIGDIR=\""/etc/open-app-store\""                  \
	$(LIBSOUP_CFLAGS)					\
	$(COMMON_CFLAGS)					\
	$(DEBUG_CFLAGS) 					\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(NULL)

AM_CFLAGS = $(WARN_CFLAGS)

libopen_app_common_la_SOURCES = 	\
	open-result.c open-result.h \
	open-results.c open-results.h \
	ocs-result.c ocs-result.h \
	ocs-results.c ocs-results.h \
	open-app-config.c open-app-config.h \
	open-app-utils.c open-app-utils.h \
	open-app-install.c open-app-install.h \
	$(NULL)

libopen_app_common_la_LIBADD =	\
	$(LIBSOUP_LIBS)		\
	$(COMMON_LIBS)		\
	$(NULL)
