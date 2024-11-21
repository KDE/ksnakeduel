/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/
#include <QApplication>

#include <KLocalizedString>
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStandardPaths>

#include "ktron.h"
#include "renderer.h"
#include "settings.h"
#include "ksnakeduel_version.h"

#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif

int main(int argc, char* argv[])
{
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif
    QApplication app(argc, argv);
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("ksnakeduel"));

    KAboutData aboutData( QStringLiteral("ksnakeduel"), i18n("KSnakeDuel"),
            QStringLiteral(KSNAKEDUEL_VERSION_STRING),
            i18n("A race in hyperspace"),
            KAboutLicense::GPL,
            i18n("(c) 1998-2000, Matthias Kiefer\n"
                 "(c) 2005, Benjamin Meyer\n"
                 "(c) 2008-2009, Stas Verberkt\n"
                 "\n"
                 "Parts of the algorithms for the computer player are from\n"
                 "xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>"),
            QString(),
            QStringLiteral("https://apps.kde.org/ksnakeduel"));
    aboutData.addAuthor(i18n("Matthias Kiefer"), i18n("Original author"), QStringLiteral("matthias.kiefer@gmx.de"));
    aboutData.addAuthor(i18n("Benjamin Meyer"), i18n("Various improvements"), QStringLiteral("ben+ktron@meyerhome.net"));
    aboutData.addAuthor(i18n("Stas Verberkt"), i18n("KDE 4 Port, interface revision and KSnake mode"), QStringLiteral("legolas@legolasweb.nl"));

    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("ksnakeduel")));

    KCrash::initialize();

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() <<  QStringLiteral("snake"), i18n("Start in KSnake mode")));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service;

    //KStandardDirs::locateLocal("appdata", QLatin1String( "themes/" ));

    if (parser.isSet(QStringLiteral("snake")))
    {
        Settings::setGameType(Settings::EnumGameType::Snake);
    }
    else if (Settings::gameType() == Settings::EnumGameType::Snake)
    {
        Settings::setGameType(Settings::EnumGameType::PlayerVSComputer);
    }

    Renderer::self(); // Creates Renderer

    KTron *ktron = new KTron();
    ktron->show();

    return app.exec();
}

