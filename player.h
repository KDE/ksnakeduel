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

#include <QQueue>

namespace Directions
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
class Player
{
	public:
		Player();
		void referencePlayField(PlayField &pf);
		void setPlayerNumber(int playerNr);
		int getPlayerNumber();
		void reset();
		void setCoordinates(int x, int y);
		void setCoordinatesTail(int x, int y);
		void setComputer(bool computer);
		void setStartPosition();

		int xCoordinate,yCoordinate;
		int xCoordinateTail,yCoordinateTail;
		int score;
		bool alive;
		Directions::Direction dir;
		Directions::Direction last_dir;
		bool accelerated;
		bool keyPressed;
		bool computer;
		int enlarge;
		
	private:
		int playerNumber;
		QQueue<SnakePart> snakeParts;
		SnakePart *snakeHead;
		PlayField *playField;
};

#endif //PLAYER_H

