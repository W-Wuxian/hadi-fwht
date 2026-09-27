dnl ============================================================================
dnl configure.ac - Autoconf input for the FWHT library
dnl ============================================================================

AC_PREREQ([2.69])
AC_INIT([libfwht], [1.0], [hsn.hadipour@gmail.com])
AC_CONFIG_SRCDIR([include/fwht.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])

dnl ----------------------------------------------------------------------
dnl Programs
dnl ----------------------------------------------------------------------
AC_PROG_CC
AC_PROG_CXX
AM_PROG_AR
AC_PROG_INSTALL
LT_INIT

dnl ----------------------------------------------------------------------
dnl OpenMP
dnl ----------------------------------------------------------------------
AC_ARG_ENABLE([openmp],
  [AS_HELP_STRING([--disable-openmp], [Disable OpenMP support])],
  [enable_openmp=$enableval], [enable_openmp=yes])

AS_IF([test "x$enable_openmp" = "xyes"], [
  AC_OPENMP
  AS_IF([test -z "$OPENMP_CFLAGS"],
    [AC_MSG_WARN([OpenMP requested but not found; building without it])])
])
AM_CONDITIONAL([HAVE_OPENMP], [test -n "$OPENMP_CFLAGS"])

dnl ----------------------------------------------------------------------
dnl SIMD (-march=native), test manuel sans autoconf-archive
dnl ----------------------------------------------------------------------
AC_ARG_ENABLE([simd],
  [AS_HELP_STRING([--disable-simd], [Disable -march=native auto SIMD flags])],
  [enable_simd=$enableval], [enable_simd=yes])

SIMD_CFLAGS=""
AS_IF([test "x$enable_simd" = "xyes"], [
  ax_save_CFLAGS="$CFLAGS"
  CFLAGS="-march=native"
  AC_MSG_CHECKING([whether $CC supports -march=native])
  AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([[]], [[return 0;]])],
    [AC_MSG_RESULT([yes])
     SIMD_CFLAGS="-march=native"],
    [AC_MSG_RESULT([no])]
  )
  CFLAGS="$ax_save_CFLAGS"
])
AC_SUBST([SIMD_CFLAGS])

dnl ----------------------------------------------------------------------
dnl CUDA (optional, detected via nvcc)
dnl ----------------------------------------------------------------------
AC_ARG_ENABLE([cuda],
  [AS_HELP_STRING([--disable-cuda], [Disable CUDA support even if nvcc is found])],
  [enable_cuda=$enableval], [enable_cuda=yes])

AC_ARG_WITH([cuda-include],
  [AS_HELP_STRING([--with-cuda-include=DIR], [Directory containing cuda_runtime_api.h])],
  [cuda_include_override="$withval"], [cuda_include_override=""])

AC_ARG_WITH([cuda-lib],
  [AS_HELP_STRING([--with-cuda-lib=DIR], [Directory containing libcudart])],
  [cuda_lib_override="$withval"], [cuda_lib_override=""])

AC_ARG_WITH([cuda-arch],
  [AS_HELP_STRING([--with-cuda-arch=LIST], [Space-separated GPU archs, e.g. "80 86 90" (default: auto)])],
  [cuda_arch_override="$withval"], [cuda_arch_override=""])

HAVE_CUDA=no
CUDA_INCFLAGS=""
CUDA_LDFLAGS=""
NVCC_ARCH_FLAGS=""
CUDA_DEFAULT_ROOT=/usr/local/cuda

AS_IF([test "x$enable_cuda" = "xyes"], [
  AC_CHECK_PROG([NVCC], [nvcc], [nvcc], [no])
  AS_IF([test "x$NVCC" != "xno"], [
    HAVE_CUDA=yes

    dnl -- includes --------------------------------------------------
    AS_IF([test -n "$cuda_include_override"], [
      CUDA_INCFLAGS="-I$cuda_include_override"
      AC_MSG_NOTICE([Using user-specified CUDA include dir: $cuda_include_override])
    ], [
      AC_MSG_CHECKING([for CUDA headers in $CUDA_DEFAULT_ROOT/include])
      AS_IF([test -f "$CUDA_DEFAULT_ROOT/include/cuda_runtime_api.h"], [
        CUDA_INCFLAGS="-I$CUDA_DEFAULT_ROOT/include"
        AC_MSG_RESULT([yes])
      ], [
        AC_MSG_RESULT([no])
        AC_MSG_WARN([nvcc was found but CUDA headers are not in $CUDA_DEFAULT_ROOT/include])
        AC_MSG_WARN([Re-run configure with: --with-cuda-include=DIR --with-cuda-lib=DIR])
      ])
    ])

    dnl -- libs ------------------------------------------------------
    AS_IF([test -n "$cuda_lib_override"], [
      CUDA_LDFLAGS="-L$cuda_lib_override -lcudart"
      AC_MSG_NOTICE([Using user-specified CUDA lib dir: $cuda_lib_override])
    ], [test -n "$CUDA_INCFLAGS"], [
      AS_IF([test -d "$CUDA_DEFAULT_ROOT/lib64"], [
        CUDA_LDFLAGS="-L$CUDA_DEFAULT_ROOT/lib64 -lcudart"
      ], [test -d "$CUDA_DEFAULT_ROOT/lib"], [
        CUDA_LDFLAGS="-L$CUDA_DEFAULT_ROOT/lib -lcudart"
      ], [
        AC_MSG_WARN([Could not locate libcudart under $CUDA_DEFAULT_ROOT; use --with-cuda-lib=DIR])
      ])
    ])

    dnl -- target GPU architectures (-gencode) ------------------------
    dnl Sans ceci, nvcc ne genere que du PTX generique pour son
    dnl architecture virtuelle par defaut, que le pilote doit alors
    dnl recompiler (JIT) au lancement -> erreur "unsupported toolchain"
    dnl si le pilote ne supporte pas cette version de PTX. On genere
    dnl explicitement du code natif (cubin) pour une liste
    dnl d'architectures reelles, evitant le JIT dans le cas courant.
    AS_IF([test -n "$cuda_arch_override"], [
      CUDA_ARCH_LIST="$cuda_arch_override"
      AC_MSG_NOTICE([Using user-specified CUDA architectures: $CUDA_ARCH_LIST])
    ], [
      NVCC_VERSION_STR=`$NVCC --version 2>/dev/null | sed -n 's/.*release \([[0-9]]*\.[[0-9]]*\).*/\1/p'`
      NVCC_MAJOR=`echo "$NVCC_VERSION_STR" | cut -d. -f1`
      AC_MSG_NOTICE([nvcc reports CUDA release: ${NVCC_VERSION_STR:-unknown} (major: ${NVCC_MAJOR:-unknown})])
      dnl CUDA 12+ a abandonne le support de compute_70 et plus ancien (Volta)
      AS_IF([test -n "$NVCC_MAJOR" && test "$NVCC_MAJOR" -ge 12 2>/dev/null], [
        CUDA_ARCH_LIST="80 86 89 90"
      ], [
        CUDA_ARCH_LIST="70 75 80 86 89 90"
      ])
    ])
    AC_MSG_NOTICE([CUDA target architectures: $CUDA_ARCH_LIST])

    for arch in $CUDA_ARCH_LIST; do
      NVCC_ARCH_FLAGS="$NVCC_ARCH_FLAGS -gencode arch=compute_$arch,code=sm_$arch"
    done
    dnl PTX de secours pour la derniere archi de la liste (compatibilite
    dnl ascendante avec des GPU plus recents que ceux explicitement listes)
    cuda_arch_last=`echo $CUDA_ARCH_LIST | awk '{print $NF}'`
    NVCC_ARCH_FLAGS="$NVCC_ARCH_FLAGS -gencode arch=compute_$cuda_arch_last,code=compute_$cuda_arch_last"
    AC_MSG_NOTICE([NVCC_ARCH_FLAGS: $NVCC_ARCH_FLAGS])
  ])
])
AM_CONDITIONAL([HAVE_CUDA], [test "x$HAVE_CUDA" = "xyes"])
AS_IF([test "x$HAVE_CUDA" = "xyes"], [AC_DEFINE([USE_CUDA], [1], [CUDA backend enabled])])
AC_SUBST([CUDA_INCFLAGS])
AC_SUBST([CUDA_LDFLAGS])
AC_SUBST([NVCC_ARCH_FLAGS])

dnl ----------------------------------------------------------------------
dnl Run the test suite as part of 'make all'?
dnl ----------------------------------------------------------------------
AC_ARG_ENABLE([tests],
  [AS_HELP_STRING([--enable-tests], [Run test suite as part of 'make all'])],
  [enable_tests=$enableval], [enable_tests=no])
AM_CONDITIONAL([RUN_TESTS], [test "x$enable_tests" = "xyes"])

dnl ----------------------------------------------------------------------
dnl Headers / functions used by the library
dnl ----------------------------------------------------------------------
AC_CHECK_HEADERS([stdint.h stdlib.h string.h pthread.h])
AC_CHECK_LIB([m], [sqrt])
AC_CHECK_LIB([pthread], [pthread_create])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([])
AC_MSG_NOTICE([Configuration summary:])
AC_MSG_NOTICE([  CC:              $CC])
AC_MSG_NOTICE([  CXX:             $CXX])
AC_MSG_NOTICE([  OpenMP:          ${OPENMP_CFLAGS:-disabled}])
AC_MSG_NOTICE([  SIMD:            ${SIMD_CFLAGS:-disabled}])
AC_MSG_NOTICE([  CUDA:            $HAVE_CUDA])
AC_MSG_NOTICE([  CUDA_INCFLAGS:   ${CUDA_INCFLAGS:-none}])
AC_MSG_NOTICE([  CUDA_LDFLAGS:    ${CUDA_LDFLAGS:-none}])
AC_MSG_NOTICE([  NVCC_ARCH_FLAGS: ${NVCC_ARCH_FLAGS:-none}])
AC_MSG_NOTICE([])
