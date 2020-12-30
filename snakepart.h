/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

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
