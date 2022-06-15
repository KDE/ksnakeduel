/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef TRON_H
#define TRON_H

#include "player.h"
#include "item.h"
#include "playfield.h"
#include "intelligence.h"

#include <QWidget>
#include <QPixmap>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QFocusEvent>


namespace KBAction
{
	enum Action {
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		ACCELERATE
	};
}

/**
* @short The playingfield
*/
class Tron : public QWidget
{
	Q_OBJECT

	public:
        explicit Tron(QWidget *parent=nullptr);
		~Tron() override;
		void updatePixmap();
		void setVelocity(int);
		void setRectSize(int newSize);
		void triggerKey(int, KBAction::Action, bool);
		bool running();
		bool paused();
		PlayField *getPlayField();
		Player *getPlayer(int playerNr);
		bool hasWinner();
		int getWinner();

	public Q_SLOTS:
		/** Starts a new game. The difference to reset is, that the players
		* points are set to zero. Emits gameEnds(Nobody).
		*/
		void newGame();
		void togglePause();
		void loadSettings();
		void itemHit(int playerNumber, int x, int y);

	Q_SIGNALS:
		void gameEnds();
		void updatedScore();
		void gameReset();
		void pauseBlocked(bool block);

	protected:
		/** Calls renderer */
		void paintEvent(QPaintEvent *) override;
		/** resets game and creates a new playingfield */
		void resizeEvent(QResizeEvent *) override;
		/** pauses game */
		void focusOutEvent(QFocusEvent *) override;

	private:
		/** The playingfield */
		PlayField pf;
		/** The players */
		Player *players[2];
		/** game status flag */
		bool gamePaused;
		/** game status flag */
		bool gameEnded;
		/**  used for waiting after game ended */
		bool gameBlocked;
		QTimer *timer;
		Item apple;
		/** Intelligence for computer */
		Intelligence intelligence;
		/** Backgroundpixmap **/
		QPixmap bgPix;
		/** time in ms between two moves */
		int velocity;
		/** size of the rects */
		int blockHeight;
		int blockWidth;
		/** counter for the number of moves, modulo 20 */
		int modMoves;

	private:
		// Functions
		/** resets the game */
		void reset();
		/** starts the game timer */
		void startGame();
		/** stops the game timer */
		void stopGame();
		/** creates a new playfield and a bufferpixmap */
		void createNewPlayfield();
		/** paints players at current player coordinates */
		void paintPlayers();
		/** emits gameEnds(Player) */
		void showWinner();
		/** retrieves the line speed */
		int lineSpeed();
		/** resizes the visual board */
		void resizeRenderer();
		/** generates new apple */
		void newApple();
		// Key handling / movement
		void switchKeyOn(int, KBAction::Action);
		void switchKeyOff(int, KBAction::Action);
		/** Check head to head collisions */
		void checkHeadToHeadCollision();
		/** Helper for the doMove() function */
		void movementHelper(bool onlyAcceleratedPlayers);
		/** Tries to generate a new obstacle */
		void newObstacle();
	
	private Q_SLOTS:
		/**
		* This is the main function of KTron.
		* It checks if an accelerator is pressed and than moves this player
		* forward. Then it checks if a crash occurred.
		* If no crash happen it moves both players forward and checks again
		* if someone crashed.
		*/
		void doMove();
		void unblockGame();
		void resetOnThemeChange();
};


#endif // TRON_H

