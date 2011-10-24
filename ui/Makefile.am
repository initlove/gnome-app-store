BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =
bin_SCRIPTS =
libexec_PROGRAMS =
noinst_LTLIBRARIES =
noinst_PROGRAMS =

AM_CPPFLAGS =                                                   \
	-I$(top_srcdir)                                         \
	-I$(top_srcdir)/st                                      \
	-I$(top_srcdir)/common					\
	-I$(top_srcdir)/lib					\
	$(LIBSOUP_CFLAGS)					\
	$(GNOME_APP_STORE_CFLAGS)                               \
	-DG_LOG_DOMAIN=\"GnomeAppStore\"                        \
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""  \
	-DPNP_IDS=\""$(PNP_IDS)"\"                              \
	-DG_ENABLE_DEBUG					\
	$(DISABLE_DEPRECATED_CFLAGS)

noinst_PROGRAMS +=      \
	app_store	\
	app_store_dl	\
	app_store_kw	\
	$(NULL)

app_store_SOURCES =        \
	gnome-app-frame-ui.c gnome-app-frame-ui.h \
	gnome-app-stage.c gnome-app-stage.h \
	gnome-app-info-page.c gnome-app-info-page.h \
	gnome-app-info-ui.c gnome-app-info-ui.h \
	app_store.c

app_store_LDADD =          \
	$(top_srcdir)/common/libgnome-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	libst-1.0.la	\
	$(GNOME_APP_STORE_LIBS)

include Makefile-st.am

app_store_dl_SOURCES =        \
	gnome-app-frame-ui.c gnome-app-frame-ui.h \
	gnome-app-stage.c gnome-app-stage.h \
	gnome-app-info-page.c gnome-app-info-page.h \
	gnome-app-info-ui.c gnome-app-info-ui.h \
	app_store_dl.c

app_store_dl_LDADD =          \
	$(top_srcdir)/common/libgnome-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	libst-1.0.la	\
	$(GNOME_APP_STORE_LIBS)

app_store_kw_SOURCES =        \
	gnome-app-frame-ui.c gnome-app-frame-ui.h \
	gnome-app-stage.c gnome-app-stage.h \
	gnome-app-info-page.c gnome-app-info-page.h \
	gnome-app-info-ui.c gnome-app-info-ui.h \
	app_store_kw.c

app_store_kw_LDADD =          \
	$(top_srcdir)/common/libgnome-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	libst-1.0.la	\
	$(GNOME_APP_STORE_LIBS)
