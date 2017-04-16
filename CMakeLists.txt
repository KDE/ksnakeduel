project(ksnakeduel)

if(NOT COMPILING_KDEGAMES)
	find_package(KDE4 REQUIRED)
	include(KDE4Defaults)
	include(MacroLibrary)
	find_package(KDEGames 4.9.0 REQUIRED)

	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	include_directories(${KDEGAMES_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QDBUS_INCLUDE_DIRS})
endif(NOT COMPILING_KDEGAMES)

add_subdirectory(pics)
add_subdirectory(themes)
add_subdirectory(doc)

########### next target ###############

set(ksnakeduel_SRCS
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

kde4_add_ui_files(ksnakeduel_SRCS general.ui)

kde4_add_kcfg_files(ksnakeduel_SRCS settings.kcfgc)

kde4_add_app_icon(ksnakeduel_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-ksnakeduel.png")

kde4_add_executable(ksnakeduel ${ksnakeduel_SRCS})

target_link_libraries(ksnakeduel kdegames kdegamesprivate ${KDE4_KDEUI_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS})

install(TARGETS ksnakeduel ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(PROGRAMS org.kde.ksnakeduel.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  ksnakeduel.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES  ksnakeduelui.rc DESTINATION ${DATA_INSTALL_DIR}/ktron)
install(FILES  ksnakeduel.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES  org.kde.ksnakeduel.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/metainfo/)
