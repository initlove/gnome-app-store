BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =
bin_SCRIPTS =
libexec_PROGRAMS =
noinst_LTLIBRARIES =
noinst_PROGRAMS =

AM_CPPFLAGS =                                                   \
	-I$(top_srcdir)                                         \
	-I$(top_srcdir)/common					\
	-I$(top_srcdir)/lib					\
	$(LIBREST_CFLAGS)					\
	$(GNOME_APP_STORE_CFLAGS)                               \
	-DG_LOG_DOMAIN=\"GnomeAppStore\"                        \
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""  \
	-DPNP_IDS=\""$(PNP_IDS)"\"                              \
	$(DEBUG_CFLAGS) 					\
	$(DISABLE_DEPRECATED_CFLAGS)

noinst_PROGRAMS +=      \
	app_store	\
	$(NULL)

app_store_SOURCES =        \
	gnome-app-store-ui.c gnome-app-store-ui.h \
	gnome-app-frame-ui.c gnome-app-frame-ui.h \
	gnome-app-ui-score.c gnome-app-ui-score.h \
	gnome-app-infos-stage.c gnome-app-infos-stage.h \
	gnome-app-info-page.c gnome-app-info-page.h \
	gnome-app-info-icon.c gnome-app-info-icon.h \
	gnome-app-comment.c gnome-app-comment.h \
	app_store.c

app_store_LDADD =          \
	$(top_srcdir)/common/libopen-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	$(LIBREST_LIBS)	\
	libst-1.0.la	\
	$(GNOME_APP_STORE_LIBS)

include Makefile-st.am
