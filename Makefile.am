
bin_PROGRAMS = ktron
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KDEUI) $(LIB_KIMGIO) $(LIB_KFILE)

SUBDIRS =

ktron_SOURCES = ktoptdlg.cpp tron.cpp player.cpp ktron.cpp main.cpp

EXTRA_DIST = ktron.desktop

KDE_ICON = ktron

appsdir = $(kde_appsdir)/Games
apps_DATA = ktron.desktop

METASOURCES = AUTO


messages:
	$(XGETTEXT) -C -kI18N_NOOP -ki18n -x $(includedir)/kde.pot *.cpp && mv messages.po $(podir)/ktron.pot


