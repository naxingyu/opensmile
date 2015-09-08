
AC_DEFUN([CHECK_HAVE_OPENSMILELIB], [

# Ask user for path to libmysqlclient stuff:.
AC_ARG_WITH(opensmilelib,
    [ --with-opensmilelib=<path> prefix of opensmile build directory. e.g. /home/myname/opensmile],
    [OPENSMILELIB_PREFIX=$with_opensmilelib],
    AC_MSG_ERROR([You must call configure with the --with-opensmilelib option.
    This tells configure where to find the opensmilelib library and headers necessary for building the plugin.])
)

AC_SUBST(OPENSMILELIB_PREFIX)
OPENSMILELIB_LIBS="-L${OPENSMILELIB_PREFIX}/.libs -lopensmile"
OPENSMILELIB_CFLAGS="-I${OPENSMILELIB_PREFIX}/src"
AC_SUBST(OPENSMILELIB_LIBS)
AC_SUBST(OPENSMILELIB_CFLAGS)


]);

