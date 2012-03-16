CLEANFILES =

noinst_LTLIBRARIES = libstore.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	-I$(top_srcdir)/common					\
	-I$(top_srcdir)/liboasyncworker				\
	$(GNOME_APP_STORE_CFLAGS)				\
	$(LIBOASYNCWORKER_CFLAGS)				\
	$(LIBREST_CFLAGS)					\
	-DPNP_IDS=\""$(PNP_IDS)"\"                              \
	-DG_LOG_DOMAIN=\"GnomeApp\"				\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(NULL)

AM_CFLAGS = $(WARN_CFLAGS)

introspection_sources =                 \
	gnome-app-proxy.c gnome-app-proxy.h	\
	gnome-app-task.c gnome-app-task.h	\
	gnome-app-store.c gnome-app-store.h	\
	$(NULL)

libstore_la_SOURCES = \
	$(introspection_sources) \
	$(NULL)

libstore_la_LIBADD =	\
	$(top_srcdir)/common/libcommon.la \
	$(top_srcdir)/liboasyncworker/liboasyncworker-1.0.la \
	$(LIBOASYNCWORKER_LIBS)				\
	$(LIBREST_LIBS)				\
	$(GNOME_APP_STORE_LIBS)			\
	$(NULL)

libstore_la_LDFLAGS = \
	-version-info $(LT_VERSION) \
	-export-symbols-regex "^gnome_.*" \
	-no-undefined

libstoredir = $(includedir)/lib
libstore_HEADERS = \
	gnome-app-store.h

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
GnomeAppStoreLib-0.1.gir: libstore.la ../common/libcommon.la
GnomeAppStoreLib_0_1_gir_INCLUDES = GObject-2.0 Rest-0.7
GnomeAppStoreLib_0_1_gir_PACKAGES = rest-0.7 glib-2.0 gobject-2.0 gio-2.0
GnomeAppStoreLib_0_1_gir_CFLAGS = -I$(top_srcdir) -I$(top_srcdir)/common
GnomeAppStoreLib_0_1_gir_LIBS = libstore.la ../common/libcommon.la
GnomeAppStoreLib_0_1_gir_FILES = $(introspection_sources) $(libstore_HEADERS)
GnomeAppStoreLib_0_1_gir_SCANNERFLAGS = --identifier-prefix=Gnome --symbol-prefix=gnome_
INTROSPECTION_SCANNER_ARGS += $(patsubst %,--c-include='lib/%',$(libstore_HEADERS))
INTROSPECTION_GIRS += GnomeAppStoreLib-0.1.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

