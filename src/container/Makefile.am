
AM_CFLAGS = -I$(top_srcdir)/include $(GUI_CFLAGS)

noinst_LTLIBRARIES = libcontainer.la

libcontainer_la_SOURCES = \
	alignment.c alignment_prot.h \
	boxchild.c boxchild.h \
	box.c box_prot.h \
	gridchild.c gridchild.h \
	grid.c grid_prot.h \
	scrolledwindow.c scrolledwindow_prot.h \
	frame.c frame_prot.h

libcontainer_la_LDFLAGS = $(GUI_LDFLAGS)
