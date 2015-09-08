
AC_DEFUN([CHECK_HAVE_QTLIB], [

# Ask user for path to libmysqlclient stuff:.
AC_ARG_WITH(qtlib,
    [ --with-qtlib=yes  build with qtlib],
    [WITH_QTLIB="yes"],
    [WITH_QTLIB="no"])
)

if test "x$WITH_QTLIB" = "xyes"; then

  QTLIB_LDADD="-L"
  QTLIB_CFLAGS="-I"

fi
AC_SUBST(QTLIB_LDADD)
AC_SUBST(QTLIB_CFLAGS)


]);

