AC_INIT([mctally],m4_esyscmd_s([./getvers.sh -v]),[dwmcrobb@me.com],[mctally],[http://www.mcplex.net])

AC_PROG_CC
AC_PROG_CXX
AC_CANONICAL_TARGET

dnl  ###  Read some generally useful macros.
builtin(include, ./dwm.m4)  dnl

dnl  Set variables for building shared library
DWM_SET_CXX_SHARED_FLAGS

dnl  Set variables for pthreads
DWM_SET_PTHREADFLAGS

OSLIBS=""
KVMLIB="-lkvm"
LIBTOOL="libtool"

case $host_os in
  freebsd[[89]]*)
    CXXFLAGS="-O2"
    LDFLAGS="-static"
    ;;
  freebsd1[[0123]]*)
    CXXFLAGS="-O2"
    LDFLAGS="-static"
    ;;
  linux*)
    CXXFLAGS="-O2"
    KVMLIB=""
    LDFLAGS="$LDFLAGS"
    AC_CHECK_LIB(procps, readproctab, [OSLIBS+=" -lprocps"], [exit 1])
    DEBARCH=`dpkg --print-architecture`
    AC_SUBST(DEBARCH)
    ;;
  darwin*)
    CC="clang++"
    CXX="clang++"
    CXXFLAGS="${CXXFLAGS} -O2 -stdlib=libc++"
    KVMLIB=""
    LDFLAGS="${LDFLAGS} -O2 -stdlib=libc++ -std=c++17"
    LIBTOOL="glibtool"
    ;;
esac

echo "host_os ${host_os}"

dnl  Check for C++17
DWM_CHECK_CPLUSPLUS_17

DWM_CHECK_NEED_LIBATOMIC
DWM_CHECK_NEED_LIBRT

AC_SUBST(KVMLIB)
AC_SUBST(LIBTOOL)
AC_SUBST(OSLIBS)

DWM_CHECK_LIBDWM
DWM_CHECK_LIBDWMCREDENCE

DWMDIR=`pkg-config --variable=prefix libDwm`
DWMINCS=`pkg-config --cflags-only-I libDwmCredence libDwm`
DWMLIBS=`pkg-config --libs libDwmCredence libDwm`
AC_SUBST(DWMINCS)
AC_SUBST(DWMLIBS)
AC_SUBST(DWMDIR)

DWM_CHECK_BOOSTASIO
DWM_CHECK_NEED_LIBIBVERBS

AC_SUBST(LDFLAGS)

DWM_CHECK_STRTOF()

DWM_PREREQ_PATH(htmlman,share/htmlman,[HTML manpage path])

DWM_SET_PKGVARS(Mcweather,[mctally-0.0.0])
DWM_GET_TAG([mctally])

AC_CHECK_PROG([MANDOC], [mandoc], [mandoc], [])

case $host_os in
  freebsd1*)  
    DWM_CHECK_SQLITE3()
    AC_SUBST(SQLITEDIR)
    ;;
esac

AC_SUBST(CXXFLAGS)

DWM_CHECK_LIBSTDCPPFS

BUILD_DOCS=""
AC_ARG_ENABLE([docs],[AS_HELP_STRING([--enable-docs],[build documentation])],
	      [BUILD_DOCS="yes"], [])
AC_SUBST(BUILD_DOCS)

AC_CONFIG_FILES([Makefile.vars packaging/debcontrol
                 packaging/fbsd_manifest packaging/mctally.pc
		 classes/include/DwmMcTallyVersion.hh])
AC_OUTPUT
