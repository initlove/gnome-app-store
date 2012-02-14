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
	test_comments	\
	test_account	\
	test_friends	\
	test_message	\
	test_scroll	\
	$(NULL)

app_store_SOURCES =        \
	gnome-app-login.c gnome-app-login.h \
	gnome-app-register.c gnome-app-register.h \
	gnome-app-ui-utils.c gnome-app-ui-utils.h	\
	gnome-app-application.c gnome-app-application.h \
	gnome-app-frame-ui.c gnome-app-frame-ui.h \
	gnome-app-score-ui.c gnome-app-score-ui.h \
	gnome-app-icon-view.c gnome-app-icon-view.h \
	gnome-app-info-page.c gnome-app-info-page.h \
	gnome-app-comment.c gnome-app-comment.h \
	gnome-app-comments.c gnome-app-comments.h \
	gnome-app-friends-ui.c gnome-app-friends-ui.h	\
	gnome-app-message-ui.c gnome-app-message-ui.h	\
	gnome-app-account-ui.c gnome-app-account-ui.h  \
	app_store.c

app_store_LDADD =          \
	$(top_srcdir)/common/libopen-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	$(LIBREST_LIBS)	\
	$(GNOME_APP_STORE_LIBS)

test_comments_SOURCES = \
	gnome-app-ui-utils.c gnome-app-ui-utils.h	\
	gnome-app-comment.c gnome-app-comment.h \
	gnome-app-comments.c gnome-app-comments.h \
	test_comments.c

test_comments_LDADD =          \
	$(top_srcdir)/common/libopen-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	$(GNOME_APP_STORE_LIBS)

test_account_SOURCES = \
	gnome-app-ui-utils.c gnome-app-ui-utils.h	\
	gnome-app-friends-ui.c gnome-app-friends-ui.h	\
	gnome-app-message-ui.c gnome-app-message-ui.h	\
	gnome-app-account-ui.c gnome-app-account-ui.h  \
	test_account.c

test_account_LDADD =          \
	$(top_srcdir)/common/libopen-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	$(GNOME_APP_STORE_LIBS)

test_friends_SOURCES = \
	gnome-app-ui-utils.c gnome-app-ui-utils.h	\
	gnome-app-friends-ui.c gnome-app-friends-ui.h	\
	test_friends.c

test_friends_LDADD =          \
	$(top_srcdir)/common/libopen-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	$(GNOME_APP_STORE_LIBS)

test_message_SOURCES = \
	gnome-app-ui-utils.c gnome-app-ui-utils.h	\
	gnome-app-message-ui.c gnome-app-message-ui.h	\
	test_message.c

test_message_LDADD =          \
	$(top_srcdir)/common/libopen-app-common.la \
	$(top_srcdir)/lib/libgnome-app-store.la \
	$(GNOME_APP_STORE_LIBS)

test_scroll_SOURCES = \
	test_scroll.c

test_scroll_LDADD =          \
	$(GNOME_APP_STORE_LIBS)
