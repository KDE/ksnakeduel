project(ksnakeduel)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")

find_package(ECM 1.3.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Qml Quick QuickWidgets Svg Test)
find_package(KF5 REQUIRED COMPONENTS 
    CoreAddons
    Config
    ItemModels
    WidgetsAddons
    WindowSystem
    Config
    DBusAddons
    I18n
    ConfigWidgets
    TextWidgets
    XmlGui
    KIO
    NotifyConfig
    NewStuff
    KDELibs4Support)

find_package(KF5KDEGames 4.9.0 REQUIRED)
find_package(Phonon4Qt5 CONFIG REQUIRED)

include_directories(BEFORE ${PHONON_INCLUDES})

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DTRANSLATION_DOMAIN="ktron")


add_subdirectory(pics)
add_subdirectory(themes)
add_subdirectory(doc)

########### next target ###############

set(ksnakeduel_SRCS
        ksnakeduel_debug.cpp
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

ki18n_wrap_ui(ksnakeduel_SRCS general.ui)

kconfig_add_kcfg_files(ksnakeduel_SRCS settings.kcfgc)

kde4_add_app_icon(ksnakeduel_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-ksnakeduel.png")

add_executable(ksnakeduel ${ksnakeduel_SRCS})

target_link_libraries(ksnakeduel KF5KDEGames KF5KDEGamesPrivate  KF5::NotifyConfig KF5::KDELibs4Support Qt5::Svg)

install(TARGETS ksnakeduel ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(PROGRAMS  ksnakeduel.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  ksnakeduel.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES  ksnakeduelui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/ksnakeduel)
install(FILES  ksnakeduel.knsrc DESTINATION ${CONFIG_INSTALL_DIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
