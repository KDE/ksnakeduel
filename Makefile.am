
bin_PROGRAMS = ktron
INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KIO) $(LIB_KDEGAMES)

ktron_SOURCES = kautoconfig_kfile.cpp ai.ui appearance.ui general.ui tron.cpp player.cpp ktron.cpp main.cpp

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
