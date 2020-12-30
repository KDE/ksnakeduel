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
#include <Kdelibs4ConfigMigrator>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStandardPaths>

#include "ktron.h"
#include "renderer.h"
#include "settings.h"
#include "ksnakeduel_version.h"


int main(int argc, char* argv[])
{
    // Fixes blurry icons with fractional scaling
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("ksnakeduel");

    Kdelibs4ConfigMigrator migrate(QStringLiteral("ksnakeduel"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("ksnakeduelrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("ksnakeduelui.rc"));
    migrate.migrate();

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
            QStringLiteral("https://kde.org/applications/games/org.kde.ksnakeduel"));
    aboutData.addAuthor(i18n("Matthias Kiefer"), i18n("Original author"), QStringLiteral("matthias.kiefer@gmx.de"));
    aboutData.addAuthor(i18n("Benjamin Meyer"), i18n("Various improvements"), QStringLiteral("ben+ktron@meyerhome.net"));
    aboutData.addAuthor(i18n("Stas Verberkt"), i18n("KDE 4 Port, interface revision and KSnake mode"), QStringLiteral("legolas@legolasweb.nl"));

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();
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

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("ksnakeduel")));

    return app.exec();
}

