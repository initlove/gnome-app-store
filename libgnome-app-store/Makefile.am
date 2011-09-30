CLEANFILES =

lib_LTLIBRARIES = libgnome-app-store.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	$(GNOME_APP_STORE_CFLAGS)				\
	$(XLIB_CFLAGS)						\
        -DCONFIGDIR=\""/etc/gnome-app-store\""                        	\
	-DG_LOG_DOMAIN=\"GnomeAppStore\"			\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""	\
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	$(DISABLE_DEPRECATED_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

introspection_sources = 		\
	gnome-app-config.c gnome-app-config.h \
	gnome-app-item.c gnome-app-item.h \
	gnome-app-server.c gnome-app-server.h \
	gnome-app-store.c gnome-app-store.h \
	gnome-app-stage.c gnome-app-stage.h

libgnome_app_store_la_SOURCES = 	\
	gnome-app-install.c gnome-app-install.h \
	local-app.c local-app.h 	\
	$(introspection_sources)	

libgnome_app_store_la_LIBADD =	\
	$(XLIB_LIBS)		\
	$(GNOME_APP_STORE_LIBS)

libgnome_app_store_la_LDFLAGS = \
	-version-info $(LT_VERSION) \
        -export-symbols-regex "^gnome_.*" \
	-no-undefined

noinst_PROGRAMS =               \
	app_demo	\
	test_scrolling

app_demo_SOURCES =	\
	app_demo.c

app_demo_LDADD =	\
	$(top_builddir)/src/libgnome-app-store.la \
	$(GNOME_APP_STORE_LIBS)

test_scrolling_SOURCES = 	\
	test_scrolling.c

test_scrolling_LDADD =		\
        $(top_builddir)/src/libgnome-app-store.la \
        $(GNOME_APP_STORE_LIBS)

appconfigdir = /etc/gnome-app-store
appconfig_DATA = gnome-app-store.conf

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gnome-app-store-0.1.pc

libgnome_app_storedir = $(includedir)/libgnome-app-store/src
libgnome_app_store_HEADERS = \
        gnome-app-store.h      

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION

GnomeAppStore-0.1.gir: libgnome-app-store.la
GnomeAppStore_0_1_gir_INCLUDES = GObject-2.0 Gtk-3.0 GnomeDesktop-3.0 Clutter-1.0 GMenu-2.0
GnomeAppStore_0_1_gir_PACKAGES = gdk-pixbuf-2.0 clutter-1.0 glib-2.0 gobject-2.0 gio-2.0 gtk+-3.0 gnome-desktop-3.0 gsettings-desktop-schemas libgnome-menu
GnomeAppStore_0_1_gir_CFLAGS = -DGNOME_DESKTOP_USE_UNSTABLE_API -I$(top_srcdir)
GnomeAppStore_0_1_gir_LIBS = libgnome-app-store.la
GnomeAppStore_0_1_gir_FILES = $(introspection_sources) $(libgnome_app_store_HEADERS)
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

