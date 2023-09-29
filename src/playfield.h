/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "object.h"

#include <QList>

#define TRON_PLAYFIELD_WIDTH 50
#define TRON_PLAYFIELD_HEIGHT 30

/**
* @short This class represents the playfield
*/
class PlayField
{
	public:
		PlayField();
		void initialize();
		
		Object *getObjectAt(int x, int y);
		int getWidth();
		int getHeight();
		
		void setObjectAt(int x, int y, Object &o);
		
	private:
		QList< Object > m_playfield;
		int m_width;
		int m_height;
};

#endif // PLAYFIELD_H
