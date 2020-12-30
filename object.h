/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

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
