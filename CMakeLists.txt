project(ktron)

if(NOT COMPILING_KDEGAMES)
	find_package(KDE4 REQUIRED)
	include(KDE4Defaults)
	include(MacroLibrary)
	find_package(KDEGames REQUIRED)

	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	include_directories(${QDBUS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${KDEGAMES_INCLUDE_DIRS})
endif(NOT COMPILING_KDEGAMES)

add_subdirectory(pics)
add_subdirectory(themes)
add_subdirectory(doc)

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

kde4_add_app_icon(ktron_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-ktron.png")

kde4_add_executable(ktron ${ktron_SRCS})

target_link_libraries(ktron kdegames kdegamesprivate ${KDE4_KDEUI_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS})

install(TARGETS ktron ${INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(kdesnake.cmake ${CMAKE_CURRENT_BINARY_DIR}/kdesnake  @ONLY)

########### install files ###############

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/kdesnake DESTINATION ${BIN_INSTALL_DIR})
install(PROGRAMS  ktron.desktop kdesnake.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  ktron.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES  ktronui.rc DESTINATION ${DATA_INSTALL_DIR}/ktron)
install(FILES  ktron.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
