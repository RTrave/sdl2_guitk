SUBDIRS = src

AM_CFLAGS = -I$(top_srcdir)/include $(GUI_CFLAGS)

lib_LTLIBRARIES = libSDL2_guitk.la

libSDL2_guitk_la_SOURCES = sdl_guitk.c sdl_guitk.1

#release
libSDL2_guitk_la_LDFLAGS = -release 1.0 -version-info $(VERSION) \
        $(GUI_LDFLAGS) \
	src/buttons/libbuttons.la \
	src/texts/libtexts.la \
	src/windows/libwindows.la \
	src/container/libcontainer.la \
	src/images/libimages.la \
	src/menus/libmenus.la \
	src/builder/libbuilder.la \
	src/libguitk.la

EXTRA_DIST=sdl_guitk.lsm.in sdl_guitk.spec.in sdl_guitk.texinfo

#  build and install the .info pages
info_TEXINFOS = sdl_guitk.texi
sdl_guitk_TEXINFOS = gpl.texi

#  install the man pages
man_MANS=sdl_guitk.1

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(includedir)/SDL2;
	for header in $(srcdir)/include/SDL2/*.h; do \
	if test -f $$header; then \
		$(INSTALL_DATA) $$header $(DESTDIR)$(includedir)/SDL2; \
	fi \
	done

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -rf $(DESTDIR)$(includedir)/SDL2/SDL_guitk*;
	rmdir $(DESTDIR)$(includedir)/SDL2;


