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

