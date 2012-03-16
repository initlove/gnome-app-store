CLEANFILES =

lib_LTLIBRARIES = libwidgets.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	$(GNOME_APP_STORE_CFLAGS)				\
	$(XLIB_CFLAGS)						\
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""	\
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	$(DISABLE_DEPRECATED_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

introspection_sources = 		\
	gnome-app-widgets.h \
	gnome-app-widget.h \
	gnome-app-text.h \
	gnome-app-check-box.h \
	gnome-app-button.h \
	gnome-app-entry.h \
	gnome-app-texture.h \
	gnome-app-widgets.c \
	gnome-app-widget.c \
	gnome-app-text.c \
	gnome-app-check-box.c \
	gnome-app-button.c \
	gnome-app-entry.c \
	gnome-app-texture.c \
	$(NULL)

libwidgets_la_SOURCES = 	\
	$(introspection_sources)	

libwidgets_la_LIBADD =	\
	$(GNOME_APP_STORE_LIBS) \
	$(NULL)

libwidgets_la_LDFLAGS = \
	-version-info $(LT_VERSION) \
        -export-symbols-regex "^gnome_.*" \
	-no-undefined

libwidgetsdir = $(includedir)/widgets
libwidgets_HEADERS = \
        gnome-app-widgets.h      

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION

GnomeAppStoreWidgets-0.1.gir: libwidgets.la
GnomeAppStoreWidgets_0_1_gir_INCLUDES = GObject-2.0 Clutter-1.0
GnomeAppStoreWidgets_0_1_gir_PACKAGES = clutter-1.0 glib-2.0 gobject-2.0 gio-2.0
GnomeAppStoreWidgets_0_1_gir_CFLAGS = -I$(top_srcdir)
GnomeAppStoreWidgets_0_1_gir_LIBS = libwidgets.la
GnomeAppStoreWidgets_0_1_gir_FILES = $(introspection_sources) $(libwidgets_HEADERS)
GnomeAppStoreWidgets_0_1_gir_SCANNERFLAGS = --identifier-prefix=Gnome --symbol-prefix=gnome_
INTROSPECTION_SCANNER_ARGS += $(patsubst %,--c-include='widgets/%',$(libwidgets_HEADERS))
INTROSPECTION_GIRS += GnomeAppStoreWidgets-0.1.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

