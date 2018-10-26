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

#include "snakepart.h"

#include "ksnakeduel_debug.h"

SnakePart::SnakePart(int playerNumber) : Object(ObjectType::SnakePart)
{
	initialize();

	this->m_playerNumber = playerNumber;
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
	return this->m_playerNumber;
}

SnakePartType::Types SnakePart::getPartType() const
{
	return m_partType;
}

void SnakePart::setPartType(SnakePartType::Types type)
{
	m_partType = type;
}

bool SnakePart::getPartTop() const
{
	return m_partTop;
}

void SnakePart::setPartTop(bool value)
{
	m_partTop = value;
}

bool SnakePart::getPartBottom() const
{
	return m_partBottom;
}

void SnakePart::setPartBottom(bool value)
{
	m_partBottom = value;
}

bool SnakePart::getPartLeft() const
{
	return m_partLeft;
}

void SnakePart::setPartLeft(bool value)
{
	m_partLeft = value;
}

bool SnakePart::getPartRight() const
{
	return m_partRight;
}

void SnakePart::setPartRight(bool value)
{
	m_partRight = value;
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
	if (getPlayerNumber() == 0)
	{
		name = QStringLiteral( "tron1-" );
	}
	else if (getPlayerNumber() == 1)
	{
		name = QStringLiteral( "tron2-" );
	}

	// Heads (or tails)
	if (getPartType() == SnakePartType::Head)
	{
		if (getPartTop() && getPartLeft() && getPartRight())
		{
			name += QLatin1String( "head-north" );
		}
		else if (getPartBottom() && getPartLeft() && getPartRight())
		{
			name += QLatin1String( "head-south" );
		}
		else if (getPartTop() && getPartBottom() && getPartLeft())
		{
			name += QLatin1String( "head-west" );
		}
		else if (getPartTop() && getPartBottom() && getPartRight())
		{
			name += QLatin1String( "head-east" );
		}
	}
	else if (getPartType() == SnakePartType::Hole)
	{
		if (getPartTop() && getPartLeft() && getPartRight())
		{
			name += QLatin1String( "tail-south" );
		}
		else if (getPartBottom() && getPartLeft() && getPartRight())
		{
			name += QLatin1String( "tail-north" );
		}
		else if (getPartTop() && getPartBottom() && getPartLeft())
		{
			name += QLatin1String( "tail-east" );
		}
		else if (getPartTop() && getPartBottom() && getPartRight())
		{
			name += QLatin1String( "tail-west" );
		}
	}
	else if (getPartType() == SnakePartType::Tail)
	{
		if (getPartTop() && getPartLeft() && getPartRight())
		{
			name += QLatin1String( "tail-south2" );
		}
		else if (getPartBottom() && getPartLeft() && getPartRight())
		{
			name += QLatin1String( "tail-north2" );
		}
		else if (getPartTop() && getPartBottom() && getPartLeft())
		{
			name += QLatin1String( "tail-east2" );
		}
		else if (getPartTop() && getPartBottom() && getPartRight())
		{
			name += QLatin1String( "tail-west2" );
		}
	}
	else if (getPartType() == SnakePartType::Body)
	{
		// Bodys
		if (getPartTop() && getPartBottom())
		{
			name += QLatin1String( "body-h" );
		}
		else if (getPartLeft() && getPartRight())
		{
			name += QLatin1String( "body-v" );
		}
		else if (getPartLeft() && getPartTop())
		{
			name += QLatin1String( "body-nw" );
		}
		else if (getPartTop() && getPartRight())
		{
			name += QLatin1String( "body-ne" );
		}
		else if (getPartLeft() && getPartBottom())
		{
			name += QLatin1String( "body-sw" );
		}
		else if (getPartBottom() && getPartRight())
		{
			name += QLatin1String( "body-se" );
		}
	}

	return name;
}
