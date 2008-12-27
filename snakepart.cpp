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
  
#include "snakepart.h"

#include "tron.h"

#include <KDebug>

SnakePart::SnakePart(int playerNumber, int partCode) : Object(ObjectType::SnakePart)
{
	initialize();

	this->partCode = partCode;
	this->playerNumber = playerNumber;
	
	//kDebug() << this->playerNumber << this->partCode << decodePart(this->playerNumber, this->partCode);
	
	generateFromLegacyType(this->partCode);
	setSVGName(decodePart());
	setOldType((this->playerNumber == 0 ? KTronEnum::PLAYER1 : KTronEnum::PLAYER2) | this->partCode);
}

//
// Init
//

void SnakePart::initialize()
{
	setPartType(SnakePartType::Empty);
	setPartTop(false);
	setPartBottom(false);
	setPartLeft(false);
	setPartRight(false);
}

//
// Getters / Setters
//

int SnakePart::getPlayerNumber()
{
	return this->playerNumber;
}

int SnakePart::getPartCode()
{
	return this->partCode;
}

void SnakePart::setPartCode(int partCode)
{
	initialize();
	this->partCode = partCode;
	
	//kDebug() << this->playerNumber << this->partCode << decodePart(this->playerNumber, this->partCode);
	
	generateFromLegacyType(this->partCode);
	setSVGName(decodePart());
	setOldType((this->playerNumber == 0 ? KTronEnum::PLAYER1 : KTronEnum::PLAYER2) | this->partCode);
}

SnakePartType::Types SnakePart::getPartType()
{
	return partType;
}

void SnakePart::setPartType(SnakePartType::Types type)
{
	partType = type;
}

bool SnakePart::getPartTop()
{
	return partTop;
}

void SnakePart::setPartTop(bool value)
{
	partTop = value;
}

bool SnakePart::getPartBottom()
{
	return partBottom;
}

void SnakePart::setPartBottom(bool value)
{
	partBottom = value;
}

bool SnakePart::getPartLeft()
{
	return partLeft;
}

void SnakePart::setPartLeft(bool value)
{
	partLeft = value;
}

bool SnakePart::getPartRight()
{
	return partRight;
}

void SnakePart::setPartRight(bool value)
{
	partRight = value;
}

//
// Legacy support
//
void SnakePart::generateFromLegacyType(int type)
{
	int setCount = 0;

	if (type & KTronEnum::TOP)
	{
		setPartTop(true);
		setCount++;
	}
	
	if (type & KTronEnum::BOTTOM)
	{
		setPartBottom(true);
		setCount++;
	}
	
	if (type & KTronEnum::LEFT)
	{
		setPartLeft(true);
		setCount++;
	}
	
	if (type & KTronEnum::RIGHT)
	{
		setPartRight(true);
		setCount++;
	}
	
	if (type & KTronEnum::HEAD)
	{
		setPartType(SnakePartType::Head);
	}
	else if (type & KTronEnum::TAIL)
	{
		setPartType(SnakePartType::Hole);
	}
	else if (setCount == 3)
	{
		setPartType(SnakePartType::Tail);
	}
	else
	{
		setPartType(SnakePartType::Body);
	}
}

//
// Decode type
//

void SnakePart::generateSVGName()
{
	setSVGName(decodePart());
}

QString SnakePart::decodePart()
{
	QString name;

	// Player
	if (playerNumber == 0)
	{
		name = "tron1-";
	}
	else if (playerNumber == 1)
	{
		name = "tron2-";
	}

	// Heads (or tails)
	if (getPartType() == SnakePartType::Head)
	{
		if (getPartTop() && getPartLeft() && getPartRight())
		{
			name += "head-north";
		}
		else if (getPartBottom() && getPartLeft() && getPartRight())
		{
			name += "head-south";
		}
		else if (getPartTop() && getPartBottom() && getPartLeft())
		{
			name += "head-west";
		}
		else if (getPartTop() && getPartBottom() && getPartRight())
		{
			name += "head-east";
		}
	}
	else if (getPartType() == SnakePartType::Hole)
	{
		if (getPartTop() && getPartLeft() && getPartRight())
		{
			name += "tail-south";
		}
		else if (getPartBottom() && getPartLeft() && getPartRight())
		{
			name += "tail-north";
		}
		else if (getPartTop() && getPartBottom() && getPartLeft())
		{
			name += "tail-east";
		}
		else if (getPartTop() && getPartBottom() && getPartRight())
		{
			name += "tail-west";
		}
	}
	else if (getPartType() == SnakePartType::Tail)
	{
		if (getPartTop() && getPartLeft() && getPartRight())
		{
			name += "tail-south2";
		}
		else if (getPartBottom() && getPartLeft() && getPartRight())
		{
			name += "tail-north2";
		}
		else if (getPartTop() && getPartBottom() && getPartLeft())
		{
			name += "tail-east2";
		}
		else if (getPartTop() && getPartBottom() && getPartRight())
		{
			name += "tail-west2";
		}
	}
	else if (getPartType() == SnakePartType::Body)
	{
		// Bodys
		if (getPartTop() && getPartBottom())
		{
			name += "body-h";
		}
		else if (getPartLeft() && getPartRight())
		{
			name += "body-v";
		}
		else if (getPartLeft() && getPartTop())
		{
			name += "body-nw";
		}
		else if (getPartTop() && getPartRight())
		{
			name += "body-ne";
		}
		else if (getPartLeft() && getPartBottom())
		{
			name += "body-sw";
		}
		else if (getPartBottom() && getPartRight())
		{
			name += "body-se";
		}
	}

	return name;
}
