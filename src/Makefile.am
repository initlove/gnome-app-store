AM_CPPFLAGS =                                                   \
	-I$(top_srcdir)                                         \
	-I$(top_srcdir)/common					\
	-I$(top_srcdir)/lib					\
	-I$(top_srcdir)/widgets					\
	$(LIBREST_CFLAGS)					\
	$(GNOME_APP_STORE_CFLAGS)                               \
	-DPIXMAPSDIR=\""$(datadir)/pixmaps/@PACKAGE@\""		\
	-DG_LOG_DOMAIN=\"GnomeAppStore\"                        \
	-DGNOMELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""  \
	-DPNP_IDS=\""$(PNP_IDS)"\"                              \
	$(DEBUG_CFLAGS) 					\
	$(DISABLE_DEPRECATED_CFLAGS)

bin_PROGRAMS = gnome-app-store

noinst_PROGRAMS =      \
	test_comments	\
	test_account	\
	test_friends	\
	test_message	\
	test_scroll	\
	$(NULL)

SOURCE_FILES =	\
	gnome-app-download.c gnome-app-download.h \
	gnome-app-stage.c gnome-app-stage.h \
	gnome-app-info-icon.c gnome-app-info-icon.h  \
	gnome-app-score.c gnome-app-score.h \
	gnome-app-login.c gnome-app-login.h \
	gnome-app-register.c gnome-app-register.h \
	gnome-app-ui-utils.c gnome-app-ui-utils.h	\
	gnome-app-frame.c gnome-app-frame.h \
	gnome-app-icon-view.c gnome-app-icon-view.h \
	gnome-app-info-page.c gnome-app-info-page.h \
	gnome-app-comment.c gnome-app-comment.h \
	gnome-app-comments.c gnome-app-comments.h \
	gnome-app-friends.c gnome-app-friends.h	\
	gnome-app-message.c gnome-app-message.h	\
	gnome-app-account.c gnome-app-account.h  \
	$(NULL)

gnome_app_store_SOURCES =        \
	$(SOURCE_FILES)		\
	app_store.c

gnome_app_store_LDADD =          \
	$(top_srcdir)/common/libcommon.la \
	$(top_srcdir)/widgets/libwidgets.la \
	$(top_srcdir)/lib/libstore.la \
	$(LIBREST_LIBS)	\
	$(GNOME_APP_STORE_LIBS)

test_comments_SOURCES = \
	$(SOURCE_FILES)		\
	test_comments.c

test_comments_LDADD =          \
	$(top_srcdir)/widgets/libwidgets.la \
	$(top_srcdir)/common/libcommon.la \
	$(top_srcdir)/lib/libstore.la \
	$(LIBREST_LIBS)	\
	$(GNOME_APP_STORE_LIBS)

test_account_SOURCES = \
	$(SOURCE_FILES)		\
	test_account.c

test_account_LDADD =          \
	$(top_srcdir)/widgets/libwidgets.la \
	$(top_srcdir)/common/libcommon.la \
	$(top_srcdir)/lib/libstore.la \
	$(LIBREST_LIBS)	\
	$(GNOME_APP_STORE_LIBS)

test_friends_SOURCES = \
	$(SOURCE_FILES)		\
	test_friends.c

test_friends_LDADD =          \
	$(top_srcdir)/widgets/libwidgets.la \
	$(top_srcdir)/common/libcommon.la \
	$(top_srcdir)/lib/libstore.la \
	$(LIBREST_LIBS)	\
	$(GNOME_APP_STORE_LIBS)

test_message_SOURCES = \
	$(SOURCE_FILES)		\
	test_message.c

test_message_LDADD =          \
	$(top_srcdir)/widgets/libwidgets.la \
	$(top_srcdir)/common/libcommon.la \
	$(top_srcdir)/lib/libstore.la \
	$(LIBREST_LIBS)	\
	$(GNOME_APP_STORE_LIBS)

test_scroll_SOURCES = \
	test_scroll.c

test_scroll_LDADD =          \
	$(top_srcdir)/widgets/libwidgets.la \
	$(LIBREST_LIBS)	\
	$(GNOME_APP_STORE_LIBS)
