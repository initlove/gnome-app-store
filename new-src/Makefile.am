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

noinst_PROGRAMS =      \
	app_store	\
	$(NULL)

app_store_SOURCES =        \
	gnome-app-info-page.c gnome-app-info-page.h \
	gnome-app-login-user.c gnome-app-login-user.h  \
	gnome-app-icon-view.c gnome-app-icon-view.h \
	gnome-app-search-entry.c gnome-app-search-entry.h \
	app_store.c

app_store_LDADD =          \
	$(top_srcdir)/common/libopen-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	$(LIBREST_LIBS)	\
	$(GNOME_APP_STORE_LIBS)
