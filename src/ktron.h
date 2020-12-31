/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef KTRON_H
#define KTRON_H

#include <QAction>
#include <KXmlGuiWindow>
#include <QKeyEvent>

#include "tron.h"

#define ID_STATUS_BASE 40
#define MESSAGE_TIME 2000

class General;
class QLabel;

/**
 * @short The main window of KTron
 */
class KTron : public KXmlGuiWindow {

	Q_OBJECT

	public:
		explicit KTron(QWidget *parent=nullptr);
		~KTron();

	private:
		void updateStatusbar();

	protected:
		/** calls tron->updatePixmap to draw frame in the new colors */
		void paletteChange(const QPalette &oldPalette);
		void closeEvent(QCloseEvent *) override;
		/** Key hits */
		void keyPressEvent(QKeyEvent *) override;
		void keyReleaseEvent(QKeyEvent *) override;

	public Q_SLOTS:
		void close();

	private Q_SLOTS:
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
		QLabel *m_statusBarLabel[3];
};

#endif // KTRON_H

