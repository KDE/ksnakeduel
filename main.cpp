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

#include "ktron.h"
#include <kapp.h>
#include <kimgio.h>
#include <kfiledialog.h>


// this function is for previewing wallpapers in the
// KFilePreviewDialog
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


int main(int argc, char* argv[]) { 
  KApplication a(argc,argv,"ktron");  

  // used for loading background pixmaps
  kimgioRegister();

  KFilePreviewDialog::registerPreviewModule("Wallpapers",previewWallpaper,PreviewPixmap);

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







