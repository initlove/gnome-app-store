AC_INIT([gnome-app-store], 0.1)
dnl AC_CONFIG_SRCDIR([src])

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-bzip2 tar-ustar])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

#
# Enable Debug
#
AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug],
                             [turn on debugging]),,
              enable_debug=yes)

if test "$enable_debug" = "yes"; then
        DEBUG_CFLAGS="-DG_ENABLE_DEBUG"
else
        if test "x$enable_debug" = "xno"; then
                DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS"
        else
                DEBUG_CFLAGS=""
        fi
fi
AC_SUBST(DEBUG_CFLAGS)

#
# Enable Profiling
#
AC_ARG_ENABLE(profiling,
              AS_HELP_STRING([--enable-profiling],
                             [turn on profiling]),,
              enable_profiling=yes)

if test "$enable_profiling" = "yes"; then
    AC_DEFINE(ENABLE_PROFILING,1,[enable profiling])
fi

# Sets GLIB_GENMARSHAL and GLIB_MKENUMS
AM_PATH_GLIB_2_0()
G_IR_SCANNER=`$PKG_CONFIG --variable=g_ir_scanner gobject-introspection-1.0`
AC_SUBST(G_IR_SCANNER)
G_IR_COMPILER=`$PKG_CONFIG --variable=g_ir_compiler gobject-introspection-1.0`
AC_SUBST(G_IR_COMPILER)
G_IR_GENERATE=`$PKG_CONFIG --variable=g_ir_generate gobject-introspection-1.0`
AC_SUBST(G_IR_GENERATE)
GIRDIR=`$PKG_CONFIG --variable=girdir gobject-introspection-1.0`
AC_SUBST(GIRDIR)
TYPELIBDIR="$($PKG_CONFIG --variable=typelibdir gobject-introspection-1.0)"
AC_SUBST(TYPELIBDIR)

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
PKG_CHECK_MODULES(COMMON, dbus-glib-1 gobject-2.0 gio-2.0)
PKG_CHECK_MODULES(LIBXML, libxml-2.0 >= $LIBXML_REQUIRED)
PKG_CHECK_MODULES(LIBSOUP, libsoup-2.4)
PKG_CHECK_MODULES(LIBREST, rest-0.7)
PKG_CHECK_MODULES(LIBOASYNCWORKER, glib-2.0 gobject-2.0 gthread-2.0)
PKG_CHECK_MODULES(ST, clutter-1.0 gtk+-3.0 libcroco-0.6 >= 0.6.2 gnome-desktop-3.0 >= 2.90.0 x11)
PKG_CHECK_MODULES(GNOME_APP_STORE, libxml-2.0 >= $LIBXML_REQUIRED gnome-desktop-3.0 >= $GNOME_DESKTOP_REQUIRED gdk-pixbuf-2.0 >= $GDK_PIXBUF_REQUIRED gtk+-3.0 >= $GTK_REQUIRED glib-2.0 >= $GLIB_REQUIRED gio-2.0 >= $GLIB_REQUIRED gsettings-desktop-schemas >= $GSETTINGS_DESKTOP_SCHEMAS_REQUIRED clutter-1.0 >= $CLUTTER_REQUIRED)


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
pixmaps/Makefile
ui/Makefile
common/Makefile
liboasyncworker/Makefile
lib/Makefile
src/Makefile
lib/gnome-app-store-0.1.pc
po/Makefile.in
])

AC_OUTPUT
