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

SnakePart::SnakePart(int partCode) : Object()
{
	setSVGName(decodePart(partCode));
}

QString SnakePart::decodePart(int type)
{
	QString name;

	if (type & KTronEnum::ITEM1)
	{
		name = "item1";
		return name;
	}
	else if (type & KTronEnum::ITEM2)
	{
		name = "item2";
		return name;
	}
	else if (type & KTronEnum::ITEM3)
	{
		name = "item3";
		return name;
	}


	// Player
	if (type & KTronEnum::PLAYER1)
	{
		name = "tron1-";
	}
	else if (type & KTronEnum::PLAYER2)
	{
		name = "tron2-";
	}

	// Heads (or tails)
	if (type & KTronEnum::HEAD)
	{
		if ((type & KTronEnum::TOP) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "head-north";
		}
		else if ((type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "head-south";
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT))
		{
			name += "head-west";
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::RIGHT))
		{
			name += "head-east";
		}

		return name;
	}
	else if (type & KTronEnum::TAIL)
	{
		if ((type & KTronEnum::TOP) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "tail-south";
		}
		else if ((type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "tail-north";
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT))
		{
			name += "tail-east";
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::RIGHT))
		{
			name += "tail-west";
		}

		return name;
	}
	else
	{
		if ((type & KTronEnum::TOP) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "tail-south2";
			return name;
		}
		else if ((type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "tail-north2";
			return name;
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT))
		{
			name += "tail-east2";
			return name;
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::RIGHT))
		{
			name += "tail-west2";
			return name;
		}
	}

	// Bodys
	if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM))
	{
		name += "body-h";
	}
	else if ((type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
	{
		name += "body-v";
	}
	else if ((type & KTronEnum::LEFT) && (type & KTronEnum::TOP))
	{
		name += "body-nw";
	}
	else if ((type & KTronEnum::TOP) && (type & KTronEnum::RIGHT))
	{
		name += "body-ne";
	}
	else if ((type & KTronEnum::LEFT) && (type & KTronEnum::BOTTOM))
	{
		name += "body-sw";
	}
	else if ((type & KTronEnum::BOTTOM) && (type & KTronEnum::RIGHT))
	{
		name += "body-se";
	}

	return name;
}