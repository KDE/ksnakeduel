
ktron_SOURCES = ktoptdlg.cpp tron.cpp player.cpp ktron.cpp main.cpp 
ktron_LDADD   = $(LIB_KIMGIO) $(LIB_KFILE)

EXTRA_DIST = ktron.desktop 

appsdir = $(kde_appsdir)/Games
apps_DATA = ktron.desktop

KDE_ICON = ktron

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
# you can add here more. This one gets installed 
bin_PROGRAMS = ktron 

ktron_METASOURCES = AUTO

# the library search path. 
ktron_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(ktron_SOURCES) && mv messages.po ../po/ktron.pot






