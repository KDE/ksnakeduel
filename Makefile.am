
bin_PROGRAMS = ktron
INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)

ktron_SOURCES = ktoptdlg.cpp tron.cpp player.cpp ktron.cpp main.cpp
ktron_LDFLAGS =  $(all_libraries) $(KDE_RPATH)
ktron_LDADD = $(LIB_KIO) $(LIB_KDEGAMES)

EXTRA_DIST = ktron.desktop

KDE_ICON = ktron

appsdir = $(kde_appsdir)/Games/Arcade
apps_DATA = ktron.desktop

METASOURCES = AUTO

rcdir = $(kde_datadir)/ktron
rc_DATA = ktronui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ktron.pot

install-data-local:
	rm -rf $(DESTDIR)$(kde_appsdir)/Games/ktron.desktop
