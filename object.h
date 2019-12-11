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

#ifndef OBJECT_H
#define OBJECT_H

#include <QString>

namespace ObjectType {
	enum Type {
		Object,
		Item,
		SnakePart,
		Obstacle
	};
}

/**
* @short This class represents a drawable object on the playfield
*/
class Object
{
	public:
		Object();
		explicit Object(ObjectType::Type t);
        int getX() const;
        int getY() const;
		void setCoordinates(int x, int y);
        QString getSVGName() const;
        ObjectType::Type getObjectType() const;

	private:
		int m_xCoordinate;
		int m_yCoordinate;
		QString m_svgName;
		ObjectType::Type m_objectType;
		
	protected:
        void setSVGName(const QString &svgName);
};

#endif // OBJECT_H
