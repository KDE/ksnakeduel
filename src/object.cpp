/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/
  
#include "object.h"

Object::Object()
{
	m_objectType = ObjectType::Object;
}

Object::Object(ObjectType::Type t)
{
	m_objectType = t;
}

QString Object::getSVGName() const
{
	return m_svgName;
}

void Object::setSVGName(const QString &name) {
	m_svgName = name;
}

int Object::getX() const
{
	return m_xCoordinate;
}

int Object::getY() const
{
	return m_yCoordinate;
}

void Object::setCoordinates(int x, int y)
{
	m_xCoordinate = x;
	m_yCoordinate = y;
}

ObjectType::Type Object::getObjectType() const
{
	return m_objectType;
}
