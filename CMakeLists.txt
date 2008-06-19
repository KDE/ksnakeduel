
project(ktron)

find_package(KDE4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

include(KDE4Defaults)

include(MacroLibrary)

#include(ConvenienceLibs.cmake)

#include(ManualStuff.cmake)

#include(ConfigureChecks.cmake)

include_directories(${CMAKE_SOURCE_DIR}/libkdegames ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(pics)


########### next target ###############

set(ktron_SRCS
    tron.cpp
    player.cpp
    ktron.cpp
    main.cpp)

kde4_add_ui_files(ktron_SRCS ai.ui appearance.ui general.ui)

kde4_add_kcfg_files(ktron_SRCS settings.kcfgc)

kde4_add_executable(ktron ${ktron_SRCS})

target_link_libraries(ktron kdegames ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS ktron ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install(FILES  ktron.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  ktron.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES  ktronui.rc DESTINATION ${DATA_INSTALL_DIR}/ktron)



#original Makefile.am contents follow:

#SUBDIRS=pics
#
#bin_PROGRAMS = ktron
#INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)
#
#ktron_SOURCES = ai.ui appearance.ui general.ui tron.cpp player.cpp ktron.cpp main.cpp settings.kcfgc
#ktron_LDFLAGS =  $(all_libraries) $(KDE_RPATH)
#ktron_LDADD = $(LIB_KIO) $(LIB_KDEGAMES)
#ktron_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#
#EXTRA_DIST = ktron.desktop
#
#xdg_apps_DATA = ktron.desktop
#kde_kcfg_DATA = ktron.kcfg
#
#METASOURCES = AUTO
#
#rcdir = $(kde_datadir)/ktron
#rc_DATA = ktronui.rc
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/ktron.pot
