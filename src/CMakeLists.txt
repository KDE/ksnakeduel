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

ecm_qt_declare_logging_category(ksnakeduel_SRCS
    HEADER ksnakeduel_debug.h
    IDENTIFIER KSNAKEDUEL_LOG
    CATEGORY_NAME ksnakeduel
    DESCRIPTION "KSnakeDuel"
    EXPORT KSNAKEDUEL
)

ki18n_wrap_ui(ksnakeduel_SRCS general.ui)

kconfig_add_kcfg_files(ksnakeduel_SRCS settings.kcfgc)
qt5_add_resources(ksnakeduel_SRCS ksnakeduel.qrc)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-ksnakeduel.png")
ecm_add_app_icon(ksnakeduel_SRCS ICONS ${ICONS_SRCS})

add_executable(ksnakeduel ${ksnakeduel_SRCS})

target_link_libraries(ksnakeduel
    KF5::Crash
    KF5::DBusAddons
    KF5::GuiAddons
    KF5::I18n
    KF5::XmlGui
    KF5KDEGames
    KF5KDEGamesPrivate
    Qt5::Svg
)

install(TARGETS ksnakeduel ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS  org.kde.ksnakeduel.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES  ksnakeduel.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(FILES  ksnakeduel.knsrc DESTINATION "${KDE_INSTALL_DATADIR}/knsrcfiles")
install(FILES  org.kde.ksnakeduel.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_qt_install_logging_categories(
    EXPORT KSNAKEDUEL
    FILE ksnakeduel.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
