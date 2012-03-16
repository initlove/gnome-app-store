CLEANFILES =

lib_LTLIBRARIES = libcommon.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	-DCONFIGDIR=\""/etc/@PACKAGE@\""                  \
	-DPIXMAPSDIR=\""$(datadir)/pixmaps/@PACKAGE@\""         \
	-DUIDIR=\""$(datadir)/@PACKAGE@/ui\""                   \
	$(LIBSOUP_CFLAGS)                                       \
	$(LIBXML_CFLAGS)                                        \
	$(LIBJSON_CFLAGS)                                        \
	$(COMMON_CFLAGS)                                        \
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	$(DEBUG_CFLAGS)                                         \
	$(DISABLE_DEPRECATED_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

introspection_sources = 		\
	open-result.c open-result.h \
	open-results.c open-results.h \
	open-ocs-result.c open-ocs-result.h \
	open-ocs-results.c open-ocs-results.h \
	open-app-config.c open-app-config.h \
	open-app-utils.c open-app-utils.h \
	open-app-install.c open-app-install.h \
	$(NULL)

libcommon_la_SOURCES = 	\
	$(introspection_sources)	

libcommon_la_LIBADD =	\
	$(LIBXML_LIBS)		\
	$(LIBJSON_LIBS)		\
	$(LIBSOUP_LIBS)         \
	$(COMMON_LIBS)          \
	$(NULL)

libcommon_la_LDFLAGS = \
	-version-info $(LT_VERSION) \
        -export-symbols-regex "^open_.*" \
	-no-undefined

libcommondir = $(includedir)/common
libcommon_HEADERS = \
	open-app-utils.h

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION

GnomeAppStoreCommon-0.1.gir: libcommon.la
GnomeAppStoreCommon_0_1_gir_INCLUDES = GObject-2.0 Soup-2.4
GnomeAppStoreCommon_0_1_gir_PACKAGES = libsoup-2.4 glib-2.0 gobject-2.0 gio-2.0
GnomeAppStoreCommon_0_1_gir_CFLAGS = -I$(top_srcdir)
GnomeAppStoreCommon_0_1_gir_LIBS = libcommon.la
GnomeAppStoreCommon_0_1_gir_FILES = $(introspection_sources) $(libcommon_HEADERS)
GnomeAppStoreCommon_0_1_gir_SCANNERFLAGS = --identifier-prefix=Open --symbol-prefix=open_
INTROSPECTION_SCANNER_ARGS += $(patsubst %,--c-include='common/%',$(libcommon_HEADERS))
INTROSPECTION_GIRS += GnomeAppStoreCommon-0.1.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

