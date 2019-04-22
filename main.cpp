/**********************************************************************************
  This file is part of the game 'KTron'

  Copyright (C) 1998-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
  Copyright (C) 2005 Benjamin C. Meyer <ben at meyerhome dot net>
  Copyright (C) 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  *******************************************************************************/
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
#include "version.h"

static const char description[] = I18N_NOOP("A race in hyperspace");
static const char notice[] = I18N_NOOP("(c) 1998-2000, Matthias Kiefer\n"
"(c) 2005, Benjamin Meyer\n"
"(c) 2008-2009, Stas Verberkt\n"
"\n"
"Parts of the algorithms for the computer player are from\n"
"xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>");


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("ksnakeduel");

    Kdelibs4ConfigMigrator migrate(QStringLiteral("ksnakeduel"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("ksnakeduelrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("ksnakeduelui.rc"));
    migrate.migrate();

    KAboutData aboutData( QStringLiteral("ksnakeduel"), i18n("KSnakeDuel"),
            QStringLiteral(KTRON_VERSION), i18n(description), KAboutLicense::GPL, i18n(notice));
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

