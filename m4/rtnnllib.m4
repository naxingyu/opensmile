
AC_DEFUN([CHECK_HAVE_RTNNLLIB], [

# Ask user for path to nnl stuff:.
AC_ARG_WITH(rtnnllib,
    [ --with-rtnnllib=<path> prefix of rtnnllib build directory. e.g. /home/myname/rtnnl/nnl-ndim2.0/],
    [RTNNLLIB_PREFIX=$with_rtnnllib; AC_DEFINE([HAVE_RTNNLLIB],[1],[RNN-Lib])],
    [RTNNLLIB_PREFIX=""]
)

RTNNLLIB_LIBS=""
RTNNLLIB_CFLAGS=""

if test "x$RTNNLLIB_PREFIX" != "x"; then

RTNNLLIB_LIBS="-L${RTNNLLIB_PREFIX}/lib -lnn_engine -lnn_random -lxerces-c -lnetcdf"
RTNNLLIB_CFLAGS="-I${RTNNLLIB_PREFIX}/include"

fi

AC_SUBST(RTNNLLIB_PREFIX)
AC_SUBST(RTNNLLIB_LIBS)
AC_SUBST(RTNNLLIB_CFLAGS) 

]);

