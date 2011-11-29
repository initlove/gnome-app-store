CLEANFILES =

lib_LTLIBRARIES = libgnome-app-store.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	-I$(top_srcdir)/common					\
	-I$(top_srcdir)/liboasyncworker				\
	$(COMMON_CFLAGS)					\
	$(LIBOASYNCWORKER_CFLAGS)				\
	$(LIBREST_CFLAGS)					\
	-DG_LOG_DOMAIN=\"GnomeAppStore\"			\
	$(NULL)

libgnome_app_store_la_SOURCES = \
	gnome-app-task.c gnome-app-task.h	\
	gnome-app-store.c gnome-app-store.h	\
	$(NULL)

libgnome_app_store_la_LIBADD =	\
	$(top_srcdir)/common/libopen-app-common.la \
	$(top_srcdir)/liboasyncworker/liboasyncworker-1.0.la \
	$(LIBOASYNCWORKER_LIBS)				\
	$(LIBREST_LIBS)				\
	$(COMMON_LIBS)			\
	$(NULL)

