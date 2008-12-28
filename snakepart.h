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

#ifndef SNAKEPART_H
#define SNAKEPART_H

#include "object.h"

#include <QString>

namespace SnakePartType {
	enum Types {
		Empty,
		Body,
		Head,
		Tail,
		Hole
	};
}

/**
* @short This class represents a part of a snake on the playfield
*/
class SnakePart : public Object
{
	public:
		SnakePart(int playerNumber);
		int getPlayerNumber();
		SnakePartType::Types getPartType();
		void setPartType(SnakePartType::Types type);
		bool getPartTop();
		void setPartTop(bool value);
		bool getPartBottom();
		void setPartBottom(bool value);
		bool getPartLeft();
		void setPartLeft(bool value);
		bool getPartRight();
		void setPartRight(bool value);
		void generateSVGName();

	private:
		void initialize();
		QString decodePart();
		
		int playerNumber;
		SnakePartType::Types partType;
		bool partTop;
		bool partBottom;
		bool partLeft;
		bool partRight;
};

#endif // SNAKEPART_H