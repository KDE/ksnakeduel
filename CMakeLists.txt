project(ksnakeduel)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")
set (KF5_MIN_VERSION "5.15.0")

find_package(ECM 1.7.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Svg)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Completion
    Config
    ConfigWidgets
    CoreAddons
    Crash
    DBusAddons
    GuiAddons
    I18n
    WidgetsAddons
    XmlGui
)

find_package(KF5DocTools ${KF5_MIN_VERSION})

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

add_subdirectory(pics)
add_subdirectory(themes)
if (KF5DocTools_FOUND)
    add_subdirectory(doc)
endif()

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
    main.cpp
)

ki18n_wrap_ui(ksnakeduel_SRCS general.ui)

kconfig_add_kcfg_files(ksnakeduel_SRCS settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/*-apps-ksnakeduel.png")
ecm_add_app_icon(ksnakeduel_SRCS ICONS ${ICONS_SRCS})

add_executable(ksnakeduel ${ksnakeduel_SRCS})

target_link_libraries(ksnakeduel
    Qt5::Svg
    KF5::Crash
    KF5::DBusAddons
    KF5::GuiAddons
    KF5::I18n
    KF5::XmlGui
    KF5KDEGames
    KF5KDEGamesPrivate
)

install(TARGETS ksnakeduel ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(PROGRAMS  org.kde.ksnakeduel.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES  ksnakeduel.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(FILES  ksnakeduelui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/ksnakeduel)
install(FILES  ksnakeduel.knsrc DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
