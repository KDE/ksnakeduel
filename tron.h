/* ********************************************************************************
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

  *******************************************************************************/  

#ifndef TRON_H
#define TRON_H

#include "player.h"
#include "item.h"
#include "playfield.h"
#include "intelligence.h"

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <QVector>
#include <QPaintEvent>
#include <QFocusEvent>

#define WINNING_DIFF 5

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
		Tron(QWidget *parent=0);
		~Tron();
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

	public slots:
		/** Starts a new game. The difference to reset is, that the players
		* points are set to zero. Emits gameEnds(Nobody).
		*/
		void newGame();
		void togglePause();
		void loadSettings();
		void itemHit(int playerNumber, int x, int y);

	signals:
		void gameEnds();
		void updatedScore();
		void gameReset();

	protected:
		/** Calls renderer */
		void paintEvent(QPaintEvent *);
		/** resets game and creates a new playingfield */
		void resizeEvent(QResizeEvent *);
		/** pauses game */
		void focusOutEvent(QFocusEvent *);

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
		/** Check head to head collissions */
		void checkHeadToHeadCollission();
		/** Helper for the doMove() function */
		void movementHelper(bool onlyAcceleratedPlayers);
		/** Tries to generate a new obstacle */
		void newObstacle();
	
	private slots:
		/**
		* This is the main function of KTron.
		* It checkes if an accelerator is pressed and than moves this player
		* forward. Then it checkes if a crash occurred.
		* If no crash happen it moves both players forward and checks again
		* if someone crashed.
		*/
		void doMove();
		void unblockGame();
};


#endif // TRON_H

