/* ********************************************************************************
   This file is part of the game 'KTron'

  Copyright (C) 1998-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>

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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  ***************************************************************************** */  

#include <kapp.h>
#include <kimageio.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "ktron.h"
#include "version.h"

static const char *description = I18N_NOOP("A race in the hyperspace");
static const char *notice = I18N_NOOP("(c) 1998-2000, Matthias Kiefer\n\n"
"Parts of the algorithms for the computerplayer are from\n"
"xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>");


int main(int argc, char* argv[])
{
  KAboutData aboutData( "ktron", I18N_NOOP("KTron"), 
    KTRON_VERSION, description, KAboutData::License_GPL, notice);
  aboutData.addAuthor("Matthias Kiefer",0, "matthias.kiefer@gmx.de");
  KCmdLineArgs::init( argc, argv, &aboutData );

  
  KApplication a;  

  // used for loading background pixmaps
  KImageIO::registerFormats();

  if(a.isRestored())
  {
     RESTORE(KTron)
  }
  else
  {
     KTron* ktron = new KTron;
     a.setMainWidget(ktron);
     ktron->show();
  }
  return a.exec();
}







