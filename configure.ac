AC_PREREQ([2.60])

dnl lmiwbem version number
m4_define(lmiwbem_version_major, 0)
m4_define(lmiwbem_version_minor, 4)
m4_define(lmiwbem_version_patch, 1-pre)
m4_define(lmiwbem_version, lmiwbem_version_major.lmiwbem_version_minor.lmiwbem_version_patch)

AC_INIT([lmiwbem], [lmiwbem_version], [phatina@redhat.com])

AC_CONFIG_MACRO_DIR([m4])
m4_include([m4/python_devel.m4])
m4_include([m4/boost_base.m4])
m4_include([m4/boost_python.m4])

AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_SILENT_RULES([yes])

AC_CONFIG_HEADERS([config.h])
AC_DISABLE_STATIC
AC_PROG_CXX
AC_LANG([C++])
AC_PROG_LIBTOOL

LT_INIT

dnl Check for platform and set TOG-Pegasus platform variable
dnl TODO: other platforms
AC_CANONICAL_HOST
case $host_os in
    linux*)
        case `uname -p` in
            i*86)
                PEGASUS_PLATFORM=PEGASUS_PLATFORM_LINUX_IX86_GNU
                ;;
            x86_64)
                PEGASUS_PLATFORM=PEGASUS_PLATFORM_LINUX_X86_64_GNU
                ;;
            ppc)
                PEGASUS_PLATFORM=PEGASUS_PLATFORM_LINUX_PPC_GNU
                ;;
            ppc64*)
                PEGASUS_PLATFORM=PEGASUS_PLATFORM_LINUX_PPC64_GNU
                ;;
            s390)
                PEGASUS_PLATFORM=PEGASUS_PLATFORM_LINUX_ZSERIES_GNU
                ;;
            s390x)
                PEGASUS_PLATFORM=PEGASUS_PLATFORM_LINUX_ZSERIES64_GNU
                ;;
            arm*)
                PEGASUS_PLATFORM=PEGASUS_PLATFORM_LINUX_XSCALE_GNU
                ;;
            aarch64)
                PEGASUS_PLATFORM=PEGASUS_PLATFORM_LINUX_AARCH64_GNU
                ;;
            *)
                AC_MSG_ERROR([Your platform is not currently supported])
                ;;
        esac
        ;;
    *)
        AC_MSG_ERROR([Your platform is not currently supported])
        ;;
esac
AC_SUBST([PEGASUS_PLATFORM])

dnl Check for TOG-Pegasus Common library
SAVE_LIBS=$LIBS
LIBS="-lpegcommon"
AC_MSG_CHECKING([for libpegcommon])
AC_TRY_LINK([], [],
    [AC_MSG_RESULT([yes]); PEGASUS_COMMON_LIB=-lpegcommon],
    [AC_MSG_ERROR([TOG-Pegasus common library not found; install tog-pegasus-libs])]
)

dnl Check for TOG-Pegasus Client library
LIBS="$LIBS -lpegclient"
AC_MSG_CHECKING([for libpegclient])
AC_TRY_LINK([], [],
    [AC_MSG_RESULT([yes]); PEGASUS_CLIENT_LIB=-lpegclient],
    [AC_MSG_ERROR([TOG-Pegasus client library not found; install tog-pegasus-libs])]
)

dnl --with-listener
AC_ARG_WITH(
    [listener],
    [AS_HELP_STRING(
        [--with-listener=@<:@yes/no@:>@],
        [support CIMIndicationListener [default=yes]])],
    [with_listener=$withval])

LIBS="-lpeglistener"
AC_MSG_CHECKING([for libpeglistener])
if test x"$with_listener" != x"no"; then
    with_listener="yes"

    dnl Check for TOG-Pegasus Listener library
    AC_TRY_LINK([], [],
        [AC_MSG_RESULT([yes])
         AC_DEFINE([HAVE_PEGASUS_LISTENER], [1], [Pegasus CIMListener])
         PEGASUS_LISTENER_LIB=-lpeglistener],
        [AC_MSG_ERROR([TOG-Pegasus indication listener library not found; install tog-pegasus-libs])]
    )
else
    AC_MSG_RESULT([skipped])
fi

AC_SUBST([WITH_LISTENER], ["$with_listener"])
AM_CONDITIONAL([BUILD_WITH_LISTENER], [test x"$with_listener" = x"yes"])

dnl Check for TOG-Pegasus headers
AC_MSG_CHECKING([for TOG-Pegasus headers])
AC_TRY_COMPILE(
    [#define $PEGASUS_PLATFORM
     #include <Pegasus/Common/Config.h>],
    [], [AC_MSG_RESULT([yes])],
    [AC_MSG_RESULT([no])
     AC_MSG_ERROR([TOG-Pegasus headers not found; install tog-pegasus-devel])]
)

dnl Check if TOG-Pegasus CIMListener supports bind address.
if test x"$with_listener" = x"yes"; then
    LIBS="-lpeglistener"
    AC_MSG_CHECKING([for TOG-Pegasus CIMListener with bind address])
    AC_TRY_COMPILE(
        [#define $PEGASUS_PLATFORM
         #include <Pegasus/Listener/CIMListener.h>],
        [Pegasus::CIMListener l(Pegasus::String("0.0.0.0"), 10240);],
        [with_listener_bind_addr="yes"
         AC_MSG_RESULT([yes])
         AC_DEFINE([HAVE_PEGASUS_LISTENER_WITH_BIND_ADDRESS], [1],
                   [Pegasus CIMListener with bind address])],
        [with_listener_bind_addr="no"
         AC_MSG_RESULT([no])])
    LIBS=""
else
    with_listener_bind_addr="no"
fi

dnl Check for Pegasus verification callback capabilities
LIBS="-lpegcommon"
AC_MSG_CHECKING([for TOG-Pegasus X509 verification capabilities])
AC_TRY_LINK(
    [#define $PEGASUS_PLATFORM
     #include <Pegasus/Common/SSLContext.h>],
    [Pegasus::SSLCertificateVerifyFunctionWithData cb;],
    [with_x509_verif_cap="yes"
     AC_MSG_RESULT([yes]);
     AC_DEFINE(
        [HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA], [1],
        [TOG-Pegasus supports X509 verification])],
    [with_x509_verif_cap="no"
     AC_MSG_RESULT([no])]
)

dnl --with-slp
AC_ARG_WITH(
    [slp],
    [AS_HELP_STRING(
        [--with-slp=@<:@yes/no@:>@],
        [Support SLP])],
    [with_slp=$withval])

LIBS="-lslp"
AC_MSG_CHECKING([for SLP])
if test x"$with_slp" != x"no"; then
    with_slp="yes"

    dnl Check for OpenSLP library.
    AC_TRY_LINK([], [],
        [AC_MSG_RESULT([yes])
         AC_DEFINE([HAVE_SLP], [1], [Support SLP])
         SLP_LIB=-lslp],
        [AC_MSG_ERROR([OpenSLP library not found; install openslp])]
    )
else
    AC_MSG_RESULT([skipping])
fi

AC_SUBST([WITH_SLP], ["$with_slp"])
AM_CONDITIONAL([BUILD_WITH_SLP], [test x"$with_slp" = x"yes"])

LIBS=$SAVE_LIBS
AC_SUBST([PEGASUS_COMMON_LIB])
AC_SUBST([PEGASUS_CLIENT_LIB])
AC_SUBST([PEGASUS_LISTENER_LIB])
AC_SUBST([SLP_LIB])

dnl --with-python
AC_ARG_WITH(
    [python3],
    [AS_HELP_STRING(
        [--with-python3=@<:@yes/no@:>@],
        [use Python 3 [default=no]])],
    [with_python3=$withval])

if test x"$with_python3" = x"yes"; then
    AM_PATH_PYTHON([3.0])
else
    AM_PATH_PYTHON([2.6])
fi

dnl Check for python-devel
AX_PYTHON_DEVEL([$PYTHON_VERSION])

dnl Check for Boost::Python
AX_BOOST_BASE([1.40], [],
    [AC_MSG_ERROR([Boost not found; install boost-devel])])
AX_BOOST_PYTHON([$PYTHON_VERSION])
if test x"$ac_cv_boost_python" = x"no"; then
    AC_MSG_ERROR(Boost::Python not found; install boost-python)
fi

dnl Note that AX_BOOST_PYTHON just gives us the name of the library (BOOST_PYTHON_LIB),
dnl and not the actual linker flag, so we must add the -l prefix manually
BOOST_PYTHON_LIB=-l$BOOST_PYTHON_LIB
AC_SUBST([BOOST_PYTHON_LIB])

dnl --with-default-namespace
AC_ARG_WITH(
    [default-namespace],
    [AS_HELP_STRING(
        [--with-default-namespace=NAMESPACE],
        [Default CIM namespace])],
    [with_defns=$withval])

if test x"$with_defns" = x; then
    with_defns="root/cimv2"
fi

AC_DEFINE_UNQUOTED([DEFAULT_NAMESPACE], ["$with_defns"], [Default CIM namespace])

dnl --with-default-trust-store
AC_ARG_WITH(
    [default-trust-store],
    [AS_HELP_STRING(
        [--with-default-trust-store=DIR],
        [Default trust store])],
    [with_defts=$withval])

if test x"$with_defts" = x; then
    with_defts="/etc/pki/ca-trust/source/anchors/"
fi

AC_DEFINE_UNQUOTED([DEFAULT_TRUST_STORE], ["$with_defts"], [Default trust store])

dnl --with-doc
AC_ARG_WITH(
    [doc],
    [AS_HELP_STRING(
        [--with-doc=@<:@yes/no@:>@],
        [Build documentation])],
    [with_doc=$withval])

if test x"$with_doc" != x"yes"; then
    with_doc="no"
fi
AM_CONDITIONAL([BUILD_WITH_DOC], [test x"$with_doc" = x"yes"])

AC_OUTPUT(
    [makefile
     setup.py
     doc/makefile
     src/makefile
     src/lmiwbem/makefile
     src/lmiwbem/__init__.py
     src/lmiwbem/lmiwbem_types.py]
)

VERSION_RULE=$(printf "%$(echo ${#VERSION})s=" | tr " " "=")

AC_MSG_NOTICE([

    lmiwbem $VERSION:
    ========$VERSION_RULE

    Python version                         : $PYTHON_VERSION
    Default CIM namespace                  : $with_defns
    Default trust store                    : $with_defts
    Support SLP                            : $with_slp
    Support CIMIndicationListener          : $with_listener
    Support CIMListener bind Address       : $with_listener_bind_addr
    Support X509 Verification Capabilities : $with_x509_verif_cap

    Build documentation                    : $with_doc

    Now type 'make' to build $PACKAGE
])
