ecm_setup_version(${KSNAKEDUEL_VERSION}
    VARIABLE_PREFIX KSNAKEDUEL
    VERSION_HEADER ksnakeduel_version.h
)

add_executable(ksnakeduel)

target_sources(ksnakeduel PRIVATE
    intelligence.cpp
    intelligence.h
    item.cpp
    item.h
    ktron.cpp
    ktron.h
    main.cpp
    object.cpp
    object.h
    obstacle.cpp
    obstacle.h
    player.cpp
    player.h
    playfield.cpp
    playfield.h
    renderer.cpp
    renderer.h
    snakepart.cpp
    snakepart.h
    tron.cpp
    tron.h


    ksnakeduel.qrc
)

ecm_qt_declare_logging_category(ksnakeduel
    HEADER ksnakeduel_debug.h
    IDENTIFIER KSNAKEDUEL_LOG
    CATEGORY_NAME ksnakeduel
    DESCRIPTION "KSnakeDuel"
    EXPORT KSNAKEDUEL
)

ki18n_wrap_ui(ksnakeduel general.ui)

kconfig_add_kcfg_files(ksnakeduel settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-ksnakeduel.png")
ecm_add_app_icon(ksnakeduel ICONS ${ICONS_SRCS})

target_link_libraries(ksnakeduel
    KF5KDEGames
    KF5::CoreAddons
    KF5::Completion
    KF5::Crash
    KF5::DBusAddons
    KF5::GuiAddons
    KF5::I18n
    KF5::XmlGui
    Qt${QT_MAJOR_VERSION}::Svg
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
