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
		explicit SnakePart(int playerNumber);
		int getPlayerNumber();
        SnakePartType::Types getPartType() const;
		void setPartType(SnakePartType::Types type);
        bool getPartTop() const;
		void setPartTop(bool value);
        bool getPartBottom() const;
		void setPartBottom(bool value);
        bool getPartLeft() const;
		void setPartLeft(bool value);
        bool getPartRight() const;
		void setPartRight(bool value);
		void generateSVGName();

	private:
		void initialize();
		QString decodePart();
		
		int m_playerNumber;
		SnakePartType::Types m_partType;
		bool m_partTop;
		bool m_partBottom;
		bool m_partLeft;
		bool m_partRight;
};

#endif // SNAKEPART_H
