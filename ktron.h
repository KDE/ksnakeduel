/* ***************************************************************************
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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  ****************************************************************************/

#ifndef KTRON_H
#define KTRON_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>
#include "tron.h"

class KAccel;
class KSelectAction;
class KToggleAction;
class KDialogBase;
class Tron;

/**
 * @short The main window of KTron
 */
class KTron : public KMainWindow {

Q_OBJECT

public:
  KTron();
  ~KTron();

private:
  KAccel *accel;
  Tron *tron;
  QString playerName[2];
  int playerPoints[2];
  KToggleAction *showStatusbar;
  KDialogBase *settings;

  void updateStatusbar();

protected:
  /** calls tron->updatePixmap to draw frame in the new colors */
  void paletteChange(const QPalette &oldPalette);

private slots:
  void readSettings();
  /** updates players points in statusbar and checks if someone has won */
  void changeStatus(Player);
  
  void showWinner(Player winner);
  void toggleStatusbar();
  void configureKeys();
  void showSettings();
  void closeSettings();
};

#endif

