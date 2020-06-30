
SUBDIRS = windows container texts buttons images menus
#utils buttons

AM_CFLAGS = -I$(top_srcdir)/include $(GUI_CFLAGS)

noinst_LTLIBRARIES = libguitk.la

libguitk_la_SOURCES = xmalloc.c getopt.c getopt.h \
	main.c \
	context.c context_prot.h \
	debug.c debug.h \
	mysdl.c mysdl.h \
	myttf.c myttf.h \
	tooltips.c tooltips_prot.h \
	mycursor.c mycursor.h \
	mywm.c mywm.h \
	list.c list.h \
	theme.c theme.h \
	signal.c signal.h \
	object.c object_prot.h \
	widget.c widget_prot.h \
	misc.c misc_prot.h \
	container.c container_prot.h \
	bin.c bin_prot.h \
	viewport.c viewport_prot.h \
	adjustment.c adjustment_prot.h \
	scrollbar.c scrollbar_prot.h

libguitk_la_LDFLAGS = $(GUI_LDFLAGS)
