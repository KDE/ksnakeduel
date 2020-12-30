/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef INTELLIGENCE_H
#define INTELLIGENCE_H

#include <math.h>
#include <QRandomGenerator>

class Tron;

/**
* @short This class holds the intelligent opponent algorithm
*/
class Intelligence
{
	public:
		Intelligence();
		void think(int playerNumber);
		void referenceTron(Tron *t);
		
	private:
		/** retrieves the opponentSkill */
		int opponentSkill();
		/** changeDirection helper for think(playerNr) */
		void changeDirection(int playerNr,int dis_right,int dis_left);
		
		Tron *m_tron;
		/** The random sequence generator **/
		QRandomGenerator m_random;
		/** determines level of computerplayer */
		int m_lookForward;
};

#endif // INTELLIGENCE_H

