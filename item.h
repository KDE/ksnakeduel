/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef ITEM_H
#define ITEM_H

#include "object.h"

/**
* @short This class represents a player with current position and several flags
*/
class Item : public Object
{
	public:
		Item();
		void setType(int type);
		int getType() const;

	private:
		int m_type;
};

#endif // ITEM_H

