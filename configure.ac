#-----------------------------------------------------------------------------
# Autoconf Requirements
#-----------------------------------------------------------------------------
AC_PREREQ(2.61)
AC_INIT([urlblock],[0.1])
AC_LANG([C])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIRS([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects silent-rules])
AM_MAINTAINER_MODE([enable])

#-----------------------------------------------------------------------------
# Compiler and Compiler Characteristics
#-----------------------------------------------------------------------------
AM_PROG_CC_C_O
AC_PROG_CC_STDC
AC_USE_SYSTEM_EXTENSIONS
AC_GNU_SOURCE

AC_PROG_RANLIB
PKG_PROG_PKG_CONFIG

AC_C_INLINE
AC_C_CONST
AC_C_RESTRICT

#-----------------------------------------------------------------------------
# Libtool
#-----------------------------------------------------------------------------
LT_INIT

#-----------------------------------------------------------------------------
# Functions, Headers and Libraries
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# End
#-----------------------------------------------------------------------------
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
AC_MSG_RESULT([
        $PACKAGE $VERSION
        ==========

        prefix: ............... ${prefix}
        sysconfdir: ........... ${sysconfdir}
        libdir: ............... ${libdir}
        includedir: ........... ${includedir}

        compiler: ............. ${CC}
        cflags: ............... ${CFLAGS}
])
