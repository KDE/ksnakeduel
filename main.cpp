/******************************************************************************
   This file is part of the game 'KTron'

  Copyright (C) 1998-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
  Copyright (C) 2005 Benjamin C. Meyer <ben at meyerhome dot net>
  Copyright (C) 2008 Stas Verberkt <legolas at legolasweb dot nl>

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

  ****************************************************************************/  
#include <KApplication>
#include <KImageIO>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KStandardDirs>

#include "ktron.h"
#include "renderer.h"
#include "version.h"

static KLocalizedString description = ki18n("A race in hyperspace");
static KLocalizedString notice = ki18n("(c) 1998-2000, Matthias Kiefer\n\n"
"Parts of the algorithms for the computer player are from\n"
"xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>");


int main(int argc, char* argv[])
{
  KAboutData aboutData( "ktron", 0, ki18n("KTron"), 
    KTRON_VERSION, description, KAboutData::License_GPL, notice);
  aboutData.addAuthor(ki18n("Matthias Kiefer"), ki18n("Original author"), "matthias.kiefer@gmx.de");
  aboutData.addAuthor(ki18n("Benjamin Meyer"), ki18n("Various improvements"), "ben+ktron@meyerhome.net");
  aboutData.addAuthor(ki18n("Stas Verberkt"), ki18n("KDE 4 Port"), "legolas@legolasweb.nl");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication a;
  KGlobal::locale()->insertCatalog("libkdegames");
  KStandardDirs::locateLocal("appdata", "themes/");

  Renderer::self(); // Creates Renderer

  KTron *ktron = new KTron();
  ktron->show();

  return a.exec();
}

