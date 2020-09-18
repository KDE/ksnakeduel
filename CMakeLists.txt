cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

# Release Service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "20")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

# Bump KSNAKEDUEL_BASE_VERSION once new features are added
set(KSNAKEDUEL_BASE_VERSION "2.1")
set(KSNAKEDUEL_VERSION "${KSNAKEDUEL_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

project(ksnakeduel VERSION ${KSNAKEDUEL_VERSION})

set (QT_MIN_VERSION "5.10.0")
set (KF5_MIN_VERSION "5.57.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

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
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)

add_definitions(
    -DQT_NO_FOREACH
    -DQT_NO_KEYWORDS
)

add_subdirectory(pics)
add_subdirectory(themes)
if (KF5DocTools_FOUND)
    add_subdirectory(doc)
endif()

########### next target ###############

ecm_setup_version(${KSNAKEDUEL_VERSION}
    VARIABLE_PREFIX KSNAKEDUEL
    VERSION_HEADER ksnakeduel_version.h
)

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
    main.cpp
)

ecm_qt_declare_logging_category(ksnakeduel_SRCS HEADER ksnakeduel_debug.h IDENTIFIER KSNAKEDUEL_LOG CATEGORY_NAME ksnakeduel_smtp)

ki18n_wrap_ui(ksnakeduel_SRCS general.ui)

kconfig_add_kcfg_files(ksnakeduel_SRCS settings.kcfgc)
qt5_add_resources(ksnakeduel_SRCS ksnakeduel.qrc)

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
install(FILES  ksnakeduel.knsrc DESTINATION "${KDE_INSTALL_DATADIR}/knsrcfiles")
install(FILES  org.kde.ksnakeduel.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

if (${ECM_VERSION} STRGREATER "5.58.0")
    install(FILES ksnakeduel.categories  DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
else()
    install(FILES ksnakeduel.categories  DESTINATION ${KDE_INSTALL_CONFDIR})
endif()

ki18n_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
