
bin_PROGRAMS = ktron
INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)

ktron_SOURCES = ai.ui appearance.ui general.ui tron.cpp player.cpp ktron.cpp main.cpp settings.cpp
ktron_LDFLAGS =  $(all_libraries) $(KDE_RPATH)
ktron_LDADD = $(LIB_KIO) $(LIB_KDEGAMES)

EXTRA_DIST = ktron.desktop

KDE_ICON = ktron

xdg_apps_DATA = ktron.desktop

METASOURCES = AUTO

rcdir = $(kde_datadir)/ktron
rc_DATA = ktronui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ktron.pot

# avoid running the below command in parallel
settings.cpp: settings.h
settings.cpp settings.h: $(srcdir)/ktron.kcfg $(srcdir)/settings.kcfg-codegen
	kconfig_compiler $(srcdir)/ktron.kcfg $(srcdir)/settings.kcfg-codegen

