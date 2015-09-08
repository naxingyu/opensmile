AC_DEFUN([CHECK_HAVE_ANDROID],
[
	AC_CANONICAL_HOST
	define('ANDROID_LDFLAGS','-lOpenSLES')
        case "${host_os}" in
            *android*)
                ANDROID_LDFLAGS="-lOpenSLES"
            ;;
	    *)
		ANDROID_LDFLAGS=""
	    ;;
        esac

	AC_SUBST(ANDROID_LDFLAGS)
])
