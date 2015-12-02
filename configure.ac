dnl Process this file with autoconf to produce a configure script.

dnl AC_INIT(sdl_guitk.c)
dnl Every other copy of the package version number gets its value from here
Adnl M_INIT_AUTOMAKE(sdl_guitk, 0:2:0)
AC_INIT([sdl2_guitk], [0:1:0])
AC_CONFIG_SRCDIR([sdl_guitk.c])
AM_INIT_AUTOMAKE

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER(config.h)

AC_SUBST(VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC(gcc cc)

dnl autoconf-2.57 libtool-1.4.3
AC_PROG_LIBTOOL

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h sys/param.h sys/time.h time.h sys/mkdev.h sys/sysmacros.h string.h memory.h fcntl.h dirent.h sys/ndir.h ndir.h alloca.h locale.h )


dnl jm_CHECK_TYPE_STRUCT_UTIMBUF
AC_HEADER_MAJOR
AC_FUNC_ALLOCA
AC_STRUCT_TM
AC_STRUCT_ST_BLOCKS
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_FUNCS(mkfifo)
AC_CHECK_FUNC(mknod)

dnl AC_CHECK_HEADERS(unicode.h)
dnl AC_ARG_ENABLE(unicode, [  --enable-unicode=[no/yes]    use UNICODE (default=yes)],, enable_unicode=yes )
dnl if test "x$enable_unicode" = "xyes"; then

dnl AC_CHECK_HEADERS( [unicode.h], [], export tunicode=1)
dnl if test "x$tunicode" = "x1"; then
dnl    echo " Humm ... unicode not found but request!";
dnl    echo " fatal (try --enable-unicode=no) ... exit"; exit;
dnl else
dnl    GUI_LDFLAGS=" -lunicode $GUI_LDFLAGS"
dnl fi

dnl fi

dnl Checks for typedefs, structures, and compiler characteristics.
AC_PATH_PROG(sdl_config, sdl2-config)
if test "x$ac_cv_path_sdl_config" = "x"; then
   echo " libsdl2 devel package needed to compile SDL Display..";
   echo " fatal ... exit"; exit;
else
   GUI_CFLAGS="`$ac_cv_path_sdl_config --cflags` $GUI_CFLAGS"
   GUI_LDFLAGS="`$ac_cv_path_sdl_config --libs` $GUI_LDFLAGS"
fi

dnl HAVE_SDL_SDL_TTF_H
AC_CHECK_HEADERS( [SDL2/SDL_ttf.h], [], export tsdlttf=1)
if test "x$tsdlttf" = "x1"; then
   echo " Humm ... ttf needed for now! (freetype later ?) ";
   echo " fatal ... exit"; exit;
else
   GUI_LDFLAGS=" -lSDL2_ttf $GUI_LDFLAGS"
fi

dnl HAVE_SDL_SDL_IMAGE_H
AC_ARG_ENABLE(sdlimage, [  --enable-sdlimage=[no/yes]    use SDL_Image (default=yes)],, enable_sdlimage=yes )
if test "x$enable_sdlimage" = "xyes"; then

AC_CHECK_HEADERS( [SDL2/SDL_image.h], [], export tsdlimage=1)
if test "x$tsdlimage" = "x1"; then
   echo " Humm ... SDL_image.h not found!";
   echo " fatal (try --enable-sdlimage=no) ... exit"; exit;
else
   GUI_LDFLAGS=" -lSDL2_image $GUI_LDFLAGS"
fi

fi

dnl HAVE_GL_GL_H
AC_ARG_ENABLE(opengl, [  --enable-opengl=[no/yes]     use OpenGL (default=yes)],, enable_opengl=yes )
if test "x$enable_opengl" = "xyes"; then

AC_CHECK_HEADERS( [GL/gl.h], [], export tgl=1 )
if test "x$tgl" = "x1"; then
   echo " Humm ... OpenGL headers not found!";
   echo " fatal (try --enable-opengl=no) ... exit"; exit;
else
   GUI_LDFLAGS=" -lGL $GUI_LDFLAGS"
fi

fi

AC_SUBST(GUI_CFLAGS)
AC_SUBST(GUI_LDFLAGS)


dnl Checks to enable debugging informations and library (debug.h)
AC_ARG_ENABLE(debug, [  --enable-debug=[no/min/yes]  set debug level (default=no)],, enable_debug=no )
if test "x$enable_debug" = "xyes"; then
  CFLAGS="-Wall -DDEBUG_LEVEL=2 $CFLAGS"
else
  if test "x$enable_debug" = "xmin"; then
    CFLAGS="-Wall -DDEBUG_LEVEL=1 $CFLAGS"
  else
    CFLAGS="-DDEBUG_LEVEL=0 $CFLAGS"
  fi
fi


dnl Checks for library functions.

AC_OUTPUT(
src/buttons/Makefile
src/texts/Makefile
src/container/Makefile
src/windows/Makefile
src/images/Makefile
src/menus/Makefile
src/Makefile
Makefile sdl_guitk.lsm sdl_guitk.spec)
