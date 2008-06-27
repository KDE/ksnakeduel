project(ktron)

find_package(KDE4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include(KDE4Defaults)

include(MacroLibrary)

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KDEGAMES_INCLUDE_DIR})

add_subdirectory(pics)
add_subdirectory(themes)


########### next target ###############

set(ktron_SRCS
	renderer.cpp
    tron.cpp
    player.cpp
	item.cpp
    ktron.cpp
    main.cpp)

kde4_add_ui_files(ktron_SRCS general.ui)

kde4_add_kcfg_files(ktron_SRCS settings.kcfgc)

kde4_add_executable(ktron ${ktron_SRCS})

target_link_libraries(ktron ${KDE4_KDEUI_LIBS} kdegames ${KDE4_KNOTIFYCONFIG_LIBS} ${KDE4_KNEWSTUFF2_LIBS})

install(TARGETS ktron ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install(FILES  ktron.desktop snake.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  ktron.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES  ktronui.rc DESTINATION ${DATA_INSTALL_DIR}/ktron)
