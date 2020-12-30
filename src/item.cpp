/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#include "item.h"

Item::Item() : Object(ObjectType::Item)
{
	setSVGName(QStringLiteral( "item1" ));
}

//
// Getters / Setters
//

void Item::setType(int t)
{
	m_type = t;

	switch (m_type)
	{
		default:
		case 0:
			setSVGName(QStringLiteral( "item1" ));
			break;
		case 1:
			setSVGName(QStringLiteral( "item2" ));
			break;
		case 2:
			setSVGName(QStringLiteral( "item3" ));
			break;
	}
}

int Item::getType() const
{
	return m_type;
}

