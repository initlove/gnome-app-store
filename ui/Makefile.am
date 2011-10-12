AM_CPPFLAGS =                                                   \
	-I$(top_srcdir)                                         \
	-I$(top_srcdir)/libgnome-app-store                      \
	-I$(top_srcdir)/libgnome-app-store/common               \
	$(GNOME_APP_STORE_CFLAGS)                               \
	-DG_LOG_DOMAIN=\"GnomeAppStore\"                        \
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""  \
	-DPNP_IDS=\""$(PNP_IDS)"\"                              \
	$(DISABLE_DEPRECATED_CFLAGS)

noinst_PROGRAMS =               \
	app_store	\
	$(NULL)

app_store_SOURCES =        \
	gnome-app-stage.c gnome-app-stage.h \
	gnome-app-item-ui.c gnome-app-item-ui.h \
	app_store.c

app_store_LDADD =          \
	../libgnome-app-store/libgnome-app-store.la \
	$(GNOME_APP_STORE_LIBS)

