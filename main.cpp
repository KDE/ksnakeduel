/* ********************************************************************************
   This file is part of the game 'KTron'

  Copyright (C) 1998,1999 by Matthias Kiefer <matthias.kiefer@gmx.de>

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
#include <kimgio.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "ktron.h"
#include "version.h"

static const char *description = I18N_NOOP("KDE Game");

// this function is for previewing wallpapers in the
// KFilePreviewDialog
// KFilePreviewDialog is broken, so we don't care.
#if 0
bool previewWallpaper(const KFileInfo *,const QString filename,
											QString &, QPixmap& pix)
{
   bool flag=false;
   QPixmap temp(filename);
   if(!temp.isNull())
   {
      pix=temp;
      flag=true;
   }

   return flag;
}
#endif

int main(int argc, char* argv[])
{
  KAboutData aboutData( "ktron", I18N_NOOP("KTron"), 
    KTRON_VERSION, description, KAboutData::License_GPL, 
    "(c) 1999, Matthias Kiefer");
  aboutData.addAuthor("Matthias Kiefer",0, "matthias.kiefer@gmx.de");
  KCmdLineArgs::init( argc, argv, &aboutData );

  
  KApplication a;  

  // used for loading background pixmaps
  kimgioRegister();

#if 0
  KFilePreviewDialog::registerPreviewModule("Wallpapers",previewWallpaper,PreviewPixmap);
#endif

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







