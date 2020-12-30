/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "object.h"

/**
* @short This class represents a player with current position and several flags
*/
class Obstacle : public Object
{
	public:
		enum Type {
			Bush
		};

		Obstacle();
		Obstacle::Type getType() const;
		void setType(Obstacle::Type t);

	private:
		Type m_type;
};

#endif // OBSTACLE_H

