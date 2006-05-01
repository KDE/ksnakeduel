SUBDIRS=pics

bin_PROGRAMS = ktron
INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)

ktron_SOURCES = ai.ui appearance.ui general.ui tron.cpp player.cpp ktron.cpp main.cpp settings.kcfgc
ktron_LDFLAGS =  $(all_libraries) $(KDE_RPATH)
ktron_LDADD = $(LIB_KIO) $(LIB_KDEGAMES)
ktron_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

EXTRA_DIST = ktron.desktop

xdg_apps_DATA = ktron.desktop
kde_kcfg_DATA = ktron.kcfg

METASOURCES = AUTO

rcdir = $(kde_datadir)/ktron
rc_DATA = ktronui.rc

