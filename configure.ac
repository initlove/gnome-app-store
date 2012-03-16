AC_INIT([gnome-app-store], 0.1)
dnl AC_CONFIG_SRCDIR([src])

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-bzip2 tar-ustar])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PREFIX_DEFAULT(/usr)

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
# Enable Devel Mode
# 
AC_ARG_ENABLE(devel_mode,
              AS_HELP_STRING([--enable-devel-mode],
                             [turn on devel mode]),,
              enable_devel_mode=yes)

if test "$enable_devel_mode" = "yes"; then
    AC_DEFINE(DEVEL_MODE,1,[enable devel mode])
fi

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

AC_PROG_CC
PKG_PROG_PKG_CONFIG

dnl If you add a version number here, you *must* add an AC_SUBST line for
dnl it too, or it will never make it into the spec file!

LIBXML_REQUIRED=2.0
GLIB_REQUIRED=2.19.1
CLUTTER_REQUIRED=1.7.4

AC_SUBST(LIBXML_REQUIRED)
AC_SUBST(GLIB_REQUIRED)
AC_SUBST(CLUTTER_REQUIRED)

dnl pkg-config dependency checks
PKG_CHECK_MODULES(GIR, gjs-internals-1.0 gobject-introspection-1.0)
PKG_CHECK_MODULES(COMMON, dbus-glib-1 gobject-2.0 gio-2.0)
PKG_CHECK_MODULES(LIBJSON, json-glib-1.0)
PKG_CHECK_MODULES(LIBXML, libxml-2.0 >= $LIBXML_REQUIRED)
PKG_CHECK_MODULES(LIBSOUP, libsoup-2.4)
PKG_CHECK_MODULES(LIBREST, rest-0.7)
PKG_CHECK_MODULES(LIBOASYNCWORKER, glib-2.0 gobject-2.0 gthread-2.0)
PKG_CHECK_MODULES(GNOME_APP_STORE, libxml-2.0 >= $LIBXML_REQUIRED glib-2.0 >= $GLIB_REQUIRED gio-2.0 >= $GLIB_REQUIRED clutter-1.0 >= $CLUTTER_REQUIRED gmodule-export-2.0 gmodule-2.0)


dnl gnome-doc-utils stuff

GNOME_DOC_INIT

# i18n stuff
IT_PROG_INTLTOOL([0.41.0])

dnl Language Support
GETTEXT_PACKAGE=gnome-app-store
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
		   [The gettext translation domain])

AM_GLIB_GNU_GETTEXT

GOBJECT_INTROSPECTION_CHECK([0.9.7])

AC_CONFIG_FILES([
Makefile
pixmaps/Makefile
pixmaps/spin/Makefile
ui/Makefile
common/Makefile
liboasyncworker/Makefile
lib/Makefile
widgets/Makefile
src/Makefile
lib/gnome-app-store-0.1.pc
po/Makefile.in
])

AC_OUTPUT
