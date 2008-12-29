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

namespace KTronEnum
{
	enum Player {One,Two,Both,Nobody};
}

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
		/** sets the direction of player playerNr to newDirection */
		bool switchDir(int playerNr, PlayerDirections::Direction newDirection);
		PlayField *getPlayField();
		Player *getPlayer(int playerNr);

	public slots:
		/** Starts a new game. The difference to reset is, that the players
		* points are set to zero. Emits gameEnds(Nobody).
		*/
		void newGame();
		void togglePause();
		void loadSettings();
		void itemHit(int playerNumber, int x, int y);

	signals:
		void gameEnds(KTronEnum::Player loser);
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
		/** emits gameEnds(Player) and displays the winner by changing color*/
		void showWinner(KTronEnum::Player winner);
		/** retrieves the line speed */
		int lineSpeed();
		/** resizes the visual board */
		void resizeRenderer();
		/** generates new apple */
		void newApple();
		/** moves player */
		void movePlayer(int playerNr);
		// Key handling / movement
		void switchKeyOn(int, KBAction::Action);
		void switchKeyOff(int, KBAction::Action);
	
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

