/* ********************************************************************************
  This file is part of the kde-game 'KTron'

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

#ifndef PLAYER_H
#define PLAYER_H

#include "snakepart.h"
#include "playfield.h"

#include <QObject>
#include <QQueue>

namespace PlayerDirections
{
	enum Direction {
		None,
		Up,
		Down,
		Left,
		Right
	};
}

/**
* @short This class represents a player with current position and several flags
*/
class Player : public QObject
{
	Q_OBJECT

	public:
		Player(PlayField &pf, int playerNr);
		int getPlayerNumber();
		void reset();
		void setCoordinates(int x, int y);
		bool isComputer();
		void setComputer(bool computer);
		void setStartPosition();
		void movePlayer();
		bool crashed(int x, int y);
		int getX();
		int getY();
		int getScore();
		void addScore(int increment);
		void resetScore();
		void setEnlargement(int enlargement);
		PlayerDirections::Direction getDirection();
		void setDirection(PlayerDirections::Direction direction);
		bool isAlive();
		void die();
		bool isAccelerated();
		void setAccelerated(bool value);
		bool hasKeyPressed();
		void setKeyPressed(bool value);
		
	private:
		int playerNumber;
		QQueue<SnakePart> snakeParts;
		PlayField *playField;
		int score;
		int enlarge;
		PlayerDirections::Direction dir;
		bool alive;
		bool computer;
		bool accelerated;
		bool keyPressed;
		
	signals:
		void fetchedItem(int playerNumber, int x, int y);
};

#endif //PLAYER_H

