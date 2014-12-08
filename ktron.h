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

#ifndef KTRON_H
#define KTRON_H

#include <QAction>
#include <KXmlGuiWindow>
#include <QKeyEvent>

#include "tron.h"

#define ID_STATUS_BASE 40
#define MESSAGE_TIME 2000

class General;

/**
 * @short The main window of KTron
 */
class KTron : public KXmlGuiWindow {

	Q_OBJECT

	public:
		KTron(QWidget *parent=0);
		~KTron();

	private:
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
		void changeStatus();
		void updateScore();
		void showSettings();
		void showHighscores();
		void optionsConfigureKeys();
		void blockPause(bool block);
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
		Tron *m_tron;
		QAction *m_player0Up;
		QAction *m_player0Down;
		QAction *m_player0Left;
		QAction *m_player0Right;
		QAction *m_player0Accelerate;
		QAction *m_player1Up;
		QAction *m_player1Down;
		QAction *m_player1Left;
		QAction *m_player1Right;
		QAction *m_player1Accelerate;
		QAction *m_pauseButton;
		General *m_generalConfigDialog;
};

#endif // KTRON_H

