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

if(APPLE)
    set_target_properties(ksnakeduel PROPERTIES
	    MACOSX_BUNDLE_DISPLAY_NAME "KSnakeduel"
        MACOSX_BUNDLE_BUNDLE_NAME "KSnakeduel"
       MACOSX_BUNDLE_LONG_VERSION_STRING "KSnakeduel ${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.ksnakeduel"
        MACOSX_BUNDLE_COPYRIGHT "2024 The KSnakeduel Developers")
endif()

target_link_libraries(ksnakeduel
    KDEGames6
    KF6::CoreAddons
    KF6::Completion
    KF6::Crash
    KF6::DBusAddons
    KF6::GuiAddons
    KF6::I18n
    KF6::XmlGui
    KF6::IconThemes
    Qt6::Svg
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
