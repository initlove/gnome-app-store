CLEANFILES =

SUBDIRS = backend

lib_LTLIBRARIES = libgnome-app-store.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	-I$(top_srcdir)/common					\
	-I$(top_srcdir)/lib/backend				\
	$(GNOME_APP_STORE_CFLAGS)				\
	-DCONFIGDIR=\""/etc/gnome-app-store\""                  \
	-DG_LOG_DOMAIN=\"GnomeAppStore\"			\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""	\
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	$(DEBUG_CFLAGS) 					\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(NULL)

AM_CFLAGS = $(WARN_CFLAGS)

libgnome_app_storedir = .
libgnome_app_store_HEADERS = \
        gnome-app-store.h

introspection_sources = 		\
	gnome-app-store.c gnome-app-store.h \
	$(NULL)

libgnome_app_store_la_SOURCES = \
	gnome-app-store.c gnome-app-store.h

libgnome_app_store_la_LIBADD =	\
	$(top_srcdir)/common/libgnome-app-common.la \
	$(top_srcdir)/lib/backend/libapp-backend.la \
	$(GNOME_APP_STORE_LIBS)

appconfigdir = /etc/gnome-app-store
appconfig_DATA = gnome-app-store.conf

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gnome-app-store-0.1.pc


-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION

GnomeAppStore-0.1.gir: libgnome-app-store.la
GnomeAppStore_0_1_gir_INCLUDES = GObject-2.0 GMenu-2.0
GnomeAppStore_0_1_gir_PACKAGES = glib-2.0 gobject-2.0 gio-2.0 libgnome-menu
GnomeAppStore_0_1_gir_CFLAGS = -I$(top_srcdir)
GnomeAppStore_0_1_gir_LIBS = libgnome-app-store.la
GnomeAppStore_0_1_gir_FILES = $(introspection_sources)
GnomeAppStore_0_1_gir_SCANNERFLAGS = --identifier-prefix=Gnome --symbol-prefix=gnome_
INTROSPECTION_SCANNER_ARGS += $(patsubst %,--c-include='libgnome-app-store/%',$(libgnome_app_store_HEADERS))
INTROSPECTION_GIRS += GnomeAppStore-0.1.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

EXTRA_DIST = \
	gnome-app-store-0.1.pc.in

