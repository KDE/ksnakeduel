/* ***************************************************************************
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

#ifndef KTRON_H
#define KTRON_H

#include <QAction>
#include <KXmlGuiWindow>
#include <KToggleAction>
#include <QKeyEvent>

#include "tron.h"

class KAccel;
class KSelectAction;
class Tron;

/**
 * @short The main window of KTron
 */
class KTron : public KXmlGuiWindow {

	Q_OBJECT

	public:
		KTron(QWidget *parent=0);

	private:
		KAccel *accel;
		Tron *tron;
		QString playerName[2];
		int playerPoints[2];
		void updateStatusbar();

	protected:
		/** calls tron->updatePixmap to draw frame in the new colors */
		void paletteChange(const QPalette &oldPalette);
		virtual void closeEvent(QCloseEvent *);
		
		/** Key hits */
		void keyPressEvent(QKeyEvent *); 	 
		void keyReleaseEvent(QKeyEvent *);

	public slots:
		void close();


	private slots:
		void loadSettings();
		/** updates players points in statusbar and checks if someone has won */
		void changeStatus(KTronEnum::Player);
		void updateScore();
		
		void showWinner(KTronEnum::Player winner);
		void showSettings();
		
		void optionsConfigureKeys();
		
		// Triggers keys
		void triggerKey0Up(bool);
		void triggerKey0Down(bool);
		void triggerKey0Left(bool);
		void triggerKey0Right(bool);
		void triggerKey0Accelerate(bool);
		void triggerKey1Up(bool);
		void triggerKey1Down(bool);
		void triggerKey1Left(bool);
		void triggerKey1Right(bool);
		void triggerKey1Accelerate(bool);
		
	private:
		QAction *player0Up;
		QAction *player0Down;
		QAction *player0Left;
		QAction *player0Right;
		KToggleAction *player0Accelerate;
		QAction *player1Up;
		QAction *player1Down;
		QAction *player1Left;
		QAction *player1Right;
		KToggleAction *player1Accelerate;
};

#endif // KTRON_H

