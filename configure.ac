AC_INIT([gnome-app-store], 0.1)
dnl AC_CONFIG_SRCDIR([src])

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-bzip2 tar-ustar])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

LT_VERSION=2:0:0
AC_SUBST(LT_VERSION)

LT_PREREQ([2.2.6])
LT_INIT([dlopen win32-dll disable-static])

IT_PROG_INTLTOOL([0.40.6])

AC_PROG_CC
PKG_PROG_PKG_CONFIG

dnl If you add a version number here, you *must* add an AC_SUBST line for
dnl it too, or it will never make it into the spec file!

LIBXML_REQUIRED=2.0
GNOME_DESKTOP_REQUIRED=3.0
GDK_PIXBUF_REQUIRED=2.21.3
GTK_REQUIRED=3.0
GLIB_REQUIRED=2.19.1
GSETTINGS_DESKTOP_SCHEMAS_REQUIRED=0.1.4
CLUTTER_REQUIRED=1.7.4

AC_SUBST(LIBXML_REQUIRED)
AC_SUBST(GNOME_DESKTOP_REQUIRED)
AC_SUBST(GTK_REQUIRED)
AC_SUBST(GLIB_REQUIRED)
AC_SUBST(GDK_PIXBUF_REQUIRED)
AC_SUBST(CLUTTER_REQUIRED)

dnl pkg-config dependency checks

PKG_CHECK_MODULES(GNOME_APP_STORE, libxml-2.0 >= $LIBXML_REQUIRED gnome-desktop-3.0 >= $GNOME_DESKTOP_REQUIRED gdk-pixbuf-2.0 >= $GDK_PIXBUF_REQUIRED gtk+-3.0 >= $GTK_REQUIRED glib-2.0 >= $GLIB_REQUIRED gio-2.0 >= $GLIB_REQUIRED gsettings-desktop-schemas >= $GSETTINGS_DESKTOP_SCHEMAS_REQUIRED clutter-1.0 >= $CLUTTER_REQUIRED libgnome-menu)

dnl gnome-doc-utils stuff

GNOME_DOC_INIT

dnl Language Support

GETTEXT_PACKAGE=gnome-app-store-0.1
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
		   [The gettext translation domain])
AC_SUBST(GETTEXT_PACKAGE)

AM_GLIB_GNU_GETTEXT

GOBJECT_INTROSPECTION_CHECK([0.9.7])

AC_CONFIG_FILES([
Makefile
libgnome-app-store/common/Makefile
libgnome-app-store/server/Makefile
libgnome-app-store/Makefile
ui/Makefile
libgnome-app-store/gnome-app-store-0.1.pc
po/Makefile.in
])

AC_OUTPUT
