/********************************************************************************** 
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

#include "player.h"

#include "tron.h"

Player::Player()
{
	computer=false;
	score=0;
	reset();
	dir=Directions::Up;
	enlarge=0;
}

//
// Init
//

void Player::referencePlayField(PlayField &pf)
{
	playField = &pf;
}

//
// Get / Set
//

void Player::setPlayerNumber(int playerNr)
{
	playerNumber = playerNr;
}

int Player::getPlayerNumber()
{
	return playerNumber;
}

//
// Start
//

void Player::setStartPosition()
{
	while (!snakeParts.isEmpty())
	{
		snakeParts.dequeue();
	}

	setCoordinates((getPlayerNumber() + 1) * playField->getWidth() / 3, playField->getHeight() / 2);
	setCoordinatesTail(xCoordinate, yCoordinate + 1);
	
	SnakePart p1Head(getPlayerNumber(), KTronEnum::TOP | KTronEnum::LEFT | KTronEnum::RIGHT | KTronEnum::HEAD);
	SnakePart p1Tail(getPlayerNumber(), KTronEnum::BOTTOM | KTronEnum::LEFT | KTronEnum::RIGHT | KTronEnum::TAIL);
	playField->setObjectAt(xCoordinate, yCoordinate, p1Head);
	playField->setObjectAt(xCoordinate, yCoordinate + 1, p1Tail);
	
	snakeParts.enqueue(p1Tail);
	snakeParts.enqueue(p1Head);
	snakeHead = &p1Head;
}

void Player::reset()
{
 alive=true;
 accelerated=false;
 enlarge=0;
 if(computer)
   keyPressed=true;
 else
   keyPressed=false;
}

void Player::setCoordinates(int x, int y)
{
  xCoordinate=x;
  yCoordinate=y;
}

void Player::setCoordinatesTail(int x, int y)
{
  xCoordinateTail=x;
  yCoordinateTail=y;
}

void Player::setComputer(bool isComputer)
{
  computer=isComputer;
  if(computer)
    keyPressed=true;
  else keyPressed=false;
}

