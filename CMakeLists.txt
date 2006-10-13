project(ktron)

add_subdirectory( pics ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames  )


########### next target ###############

set(ktron_SRCS 
   tron.cpp 
   player.cpp 
   ktron.cpp 
   main.cpp )

kde4_automoc(ktron ${ktron_SRCS})

kde4_add_ui_files(ktron_SRCS ai.ui appearance.ui general.ui )

kde4_add_kcfg_files(ktron ktron_SRCS settings.kcfgc )

kde4_add_executable(ktron ${ktron_SRCS})

target_link_libraries(ktron  ${KDE4_KIO_LIBS} kdegames )

install(TARGETS ktron  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES ktron.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES ktron.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES ktronui.rc  DESTINATION  ${DATA_INSTALL_DIR}/ktron )

