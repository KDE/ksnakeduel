
bin_PROGRAMS = ktron
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KSYCOCA) $(LIB_KFILE)

SUBDIRS =

ktron_SOURCES = ktoptdlg.cpp tron.cpp player.cpp ktron.cpp main.cpp

EXTRA_DIST = ktron.desktop

KDE_ICON = ktron

appsdir = $(kde_appsdir)/Games
apps_DATA = ktron.desktop

METASOURCES = AUTO

rcdir = $(kde_datadir)/ktron
rc_DATA = ktronui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ktron.pot


