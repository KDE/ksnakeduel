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

#ifndef KTRON_H
#define KTRON_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ktmainwindow.h>
#include "tron.h"

class KAccel;
class KTOptDlg;

/**
* @short The main window of KTron
*/
class KTron : public KTMainWindow
{
 Q_OBJECT
public:
 KTron( const char *name=0); // Konstruktor
 ~KTron();                  // Destruktor

private:
 KAccel *accel;
 QPopupMenu *game;
 QPopupMenu *velocity;
 QPopupMenu *options;
 QPopupMenu *compPlayerMenu;
 QPopupMenu *styleMenu;
 QPopupMenu *sizeMenu;
 Tron *tron;
 QString bgPixName;
 KTOptDlg* optionsDialog;
 QString playerName[2];
 int playerPoints[2];

 /** displays the current velocity */
 void updateVelocityMenu(int);
 void updateStatusbar();
 void saveSettings();

protected:
 void readProperties(KConfig *);
 void saveProperties(KConfig *);
 /** calls tron->updatePixmap to draw frame in the new colors */
 void paletteChange(const QPalette &oldPalette);

private slots:
   void menuCallback(int);
   /** updates players points in statusbar and checks if someone has won */
   void changeStatus(Player);
   void barPositionChanged();
   void quit();
   void showWinner(Player winner);
   /** opens configure dialog */
   void configureOther();
   void takeOptions();
   void statusbarClicked(int id);
   void chooseBgPix();
};

#endif




















