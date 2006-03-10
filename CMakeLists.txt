kde4_header()

add_subdirectory( pics ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(ktron_SRCS 
   tron.cpp 
   player.cpp 
   ktron.cpp 
   main.cpp )

kde4_automoc(${ktron_SRCS})

kde4_add_ui_files(ktron_SRCS ai.ui appearance.ui general.ui )

kde4_add_kcfg_files(ktron_SRCS settings.kcfgc )

kde4_add_executable(ktron ${ktron_SRCS})

target_link_libraries(ktron  ${QT_AND_KDECORE_LIBS} kio )

install_targets(/bin ktron )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES ktron.desktop )
install_files( ${KCFG_INSTALL_DIR} FILES ktron.kcfg )
install_files( ${DATA_INSTALL_DIR}/ktron FILES ktronui.rc )

kde4_footer()



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
