/* ********************************************************************************
  This file is part of the kde-game 'KTron'

  Copyright (C) 1998-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
  Copyright (C) 2005 Benjamin C. Meyer <ben at meyerhome dot net>

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

#ifndef PLAYER_H
#define PLAYER_H

#include <config.h>

namespace Directions
{
  enum Direction{ None, Up, Down, Left, Right};
}

/**
* @short This class represents a player with current position and several flags
*/
class player
{
public:
  player();
  void reset();
  void setCoordinates(int x, int y);
  void setComputer(bool computer);

  int xCoordinate,yCoordinate;
  int score;
  bool alive;
  Directions::Direction dir;
  Directions::Direction last_dir;
  bool accelerated;
  bool keyPressed;
  bool computer;
};

#endif

