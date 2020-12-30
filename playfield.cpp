/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/
  
#include "playfield.h"

#include "ksnakeduel_debug.h"

PlayField::PlayField()
{
	m_width = TRON_PLAYFIELD_WIDTH;
	m_height = TRON_PLAYFIELD_HEIGHT;
	
	m_playfield.resize(m_width * m_height);
	initialize();
}

void PlayField::initialize()
{	
	int i, j;
	for (i = 0; i < m_width; ++i) {
		for (j = 0; j < m_height; ++j) {
			Object newObj = Object();
			this->setObjectAt(i, j, newObj);
		}
	}
}

//
// Methods for retrieval
//

Object *PlayField::getObjectAt(int x, int y)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
		qCDebug(KSNAKEDUEL_LOG) << "Inexistent place accessed: (" << x << ", " << y << ")";

        return nullptr;
	}

	return &m_playfield[x * m_height + y];
}

int PlayField::getWidth()
{
	return m_width;
}

int PlayField::getHeight()
{
	return m_height;
}

//
// Methods for setting
//
void PlayField::setObjectAt(int x, int y, Object &o)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
		qCDebug(KSNAKEDUEL_LOG) << "Inexistent place accessed: (" << x << ", " << y << ")";

		return;
	}
	
	m_playfield[x * m_height + y] = o;
	o.setCoordinates(x, y);
}
