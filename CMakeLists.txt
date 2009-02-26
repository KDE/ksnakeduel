project(ktron)

add_subdirectory(pics)
add_subdirectory(themes)


########### next target ###############

set(ktron_SRCS
	fontutils.cpp
	renderer.cpp
	object.cpp
	snakepart.cpp
	item.cpp
	obstacle.cpp
	playfield.cpp
	player.cpp
	intelligence.cpp
	tron.cpp
	ktron.cpp
	main.cpp)

kde4_add_ui_files(ktron_SRCS general.ui)

kde4_add_kcfg_files(ktron_SRCS settings.kcfgc)

kde4_add_executable(ktron ${ktron_SRCS})

target_link_libraries(ktron kdegames ${KDE4_KDEUI_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} ${KDE4_KNEWSTUFF2_LIBS})

install(TARGETS ktron ${INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(kdesnake.cmake ${CMAKE_CURRENT_BINARY_DIR}/kdesnake  @ONLY)

########### install files ###############

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/kdesnake DESTINATION ${BIN_INSTALL_DIR})
install(FILES  ktron.desktop kdesnake.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  ktron.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES  ktronui.rc DESTINATION ${DATA_INSTALL_DIR}/ktron)
install(FILES  ktron.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
