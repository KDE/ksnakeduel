/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#include "obstacle.h"

Obstacle::Obstacle() : Object(ObjectType::Obstacle)
{
	setSVGName(QStringLiteral( "bush" ));
}

//
// Getters / Setters
//

void Obstacle::setType(Obstacle::Type t)
{
	m_type = t;

	switch (m_type)
	{
		default:
		case Obstacle::Bush:
			setSVGName(QStringLiteral( "bush" ));
			break;
	}
}

Obstacle::Type Obstacle::getType() const
{
	return m_type;
}

