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
#include "snakepart.h"
#include "settings.h"

#include <KDebug>

Player::Player(PlayField &pf, int playerNr) : QObject()
{
	playField = &pf;
	playerNumber = playerNr;
	computer = false;
	score = 0;
	dir = Directions::Up;
	enlarge = 0;
	
	reset();
}

//
// Get / Set
//

int Player::getPlayerNumber()
{
	return playerNumber;
}

int Player::getX()
{
	if (snakeParts.isEmpty())
	{
		kDebug() << "Requested coordinate of inexistant snake";
		return 0;
	}
	
	return snakeParts.last().getX();
}

int Player::getY()
{
	if (snakeParts.isEmpty())
	{
		kDebug() << "Requested coordinate of inexistant snake";
		return 0;
	}
	
	return snakeParts.last().getY();
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
	
	int x = (getPlayerNumber() + 1) * playField->getWidth() / 3;
	int y = playField->getHeight() / 2;
	
	SnakePart head(getPlayerNumber());
	head.setPartTop(true);
	head.setPartLeft(true);
	head.setPartRight(true);
	head.setPartType(SnakePartType::Head);
	head.generateSVGName();
	
	SnakePart tail(getPlayerNumber());
	tail.setPartBottom(true);
	tail.setPartLeft(true);
	tail.setPartRight(true);
	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		tail.setPartType(SnakePartType::Tail);
	}
	else
	{
		tail.setPartType(SnakePartType::Hole);
	}
	tail.generateSVGName();
	
	playField->setObjectAt(x, y, head);
	playField->setObjectAt(x, y + 1, tail);
	
	snakeParts.enqueue(tail);
	snakeParts.enqueue(head);
}

//
// Movement
//

void Player::movePlayer()
{
	int oldX = snakeParts.last().getX();
	int oldY = snakeParts.last().getY();
	SnakePart newHead(playerNumber);

	int newX = oldX;
	int newY = oldY;

	newHead.setPartType(SnakePartType::Head);

	switch (dir)
	{
		case Directions::Up:
			newY--;
			newHead.setPartTop(true);
			newHead.setPartLeft(true);
			newHead.setPartRight(true);
			break;
		case Directions::Down:
			newY++;
			newHead.setPartBottom(true);
			newHead.setPartLeft(true);
			newHead.setPartRight(true);
			break;
		case Directions::Left:
			newX--;
			newHead.setPartTop(true);
			newHead.setPartBottom(true);
			newHead.setPartLeft(true);
			break;
		case Directions::Right:
			newX++;
			newHead.setPartTop(true);
			newHead.setPartBottom(true);
			newHead.setPartRight(true);
			break;
		default:
			break;
	}
	
	if (crashed(newX, newY))
	{
		//kDebug() << "Crashed at: (" << newX << ", " << newY << ")";
		alive = false;
	}
	
	if (alive)
	{
		switch (dir)
		{
			// unset drawing flags in the moving direction
			case Directions::Up:
				snakeParts.last().setPartTop(false);
				break;
			case Directions::Down:
				snakeParts.last().setPartBottom(false);
				break;
			case Directions::Right:
				snakeParts.last().setPartRight(false);
				break;
			case Directions::Left:
				snakeParts.last().setPartLeft(false);
				break;
			default:
				break;
		}
		snakeParts.last().setPartType(SnakePartType::Body);
		snakeParts.last().generateSVGName();
		playField->setObjectAt(snakeParts.last().getX(), snakeParts.last().getY(), snakeParts.last());
		
		if (playField->getObjectAt(newX, newY)->getObjectType() == ObjectType::Item)
		{
			//kDebug() << "Boom!";
			emit fetchedItem(playerNumber, newX, newY);
		}

		newHead.generateSVGName();
		playField->setObjectAt(newX, newY, newHead);
		snakeParts.enqueue(newHead);
	}

	if (alive && enlarge == 0 && Settings::gameType() == Settings::EnumGameType::Snake)
	{
		SnakePart oldTail = snakeParts.dequeue();
		
		if (!oldTail.getPartTop())
		{
			snakeParts.head().setPartBottom(true);
		}
		else if (!oldTail.getPartBottom())
		{
			snakeParts.head().setPartTop(true);
		}
		else if (!oldTail.getPartLeft())
		{
			snakeParts.head().setPartRight(true);
		}
		else if (!oldTail.getPartRight())
		{
			snakeParts.head().setPartLeft(true);
		}
		
		snakeParts.head().setPartType(SnakePartType::Tail);
		snakeParts.head().generateSVGName();
		playField->setObjectAt(snakeParts.head().getX(), snakeParts.head().getY(), snakeParts.head());
		
		Object emptyObject = Object();
		playField->setObjectAt(oldTail.getX(), oldTail.getY(), emptyObject);
	}
	else if (enlarge > 0)
	{
		enlarge--;
	}
}

//
// Crash check
//

bool Player::crashed(int x, int y)
{
	bool flag;

	if (x < 0 || y < 0 || x >= playField->getWidth() || y >= playField->getHeight())
	{
		flag = true;
	}
	else
	{	
		ObjectType::Type objType = playField->getObjectAt(x, y)->getObjectType();

		if (objType == ObjectType::Item)
			flag = false;
		else if (objType != ObjectType::Object)
			flag = true;
		else 
			flag = false;
	}

	return flag;
}

//
// Reset
//

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

//
// Computer powered
//

void Player::setComputer(bool isComputer)
{
  computer=isComputer;
  if(computer)
    keyPressed=true;
  else keyPressed=false;
}

