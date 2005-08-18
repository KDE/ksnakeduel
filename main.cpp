/******************************************************************************
   This file is part of the game 'KTron'

  Copyright (C) 1998-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
  Copyright (C) 2005 Benjamin C. Meyer <ben at meyerhome dot net>

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
  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.

  ****************************************************************************/  
#include <kapplication.h>
#include <kimageio.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "ktron.h"
#define KTRON_VERSION "1.1"

static const char description[] = I18N_NOOP("A race in hyperspace");
static const char notice[] = I18N_NOOP("(c) 1998-2000, Matthias Kiefer\n\n"
"Parts of the algorithms for the computer player are from\n"
"xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>");


int main(int argc, char* argv[])
{
  KAboutData aboutData( "ktron", I18N_NOOP("KTron"), 
    KTRON_VERSION, description, KAboutData::License_GPL, notice);
  aboutData.addAuthor("Matthias Kiefer",I18N_NOOP("Original author"), "matthias.kiefer@gmx.de");
  aboutData.addAuthor("Benjamin Meyer",I18N_NOOP("Various improvements"), "ben+ktron@meyerhome.net");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication a;
  KGlobal::locale()->insertCatalogue("libkdegames");
  
  // used for loading background pixmaps
  KImageIO::registerFormats();

  if(a.isRestored()){
     RESTORE(KTron)
  }
  else {
     KTron *ktron = new KTron();
     a.setMainWidget(ktron);
     ktron->show();
  }
  return a.exec();
}

