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

#ifndef RENDERER_H
#define RENDERER_H

#include "playfield.h"

#include <QPainter>
#include <QString>
#include <QSize>

class QPixmap;
class RendererPrivate;

class Renderer {
	private:
		Renderer();
		Renderer(const Renderer &);
		~Renderer();
	public:
		static Renderer *self();

		bool loadTheme(const QString &);
		void boardResized(int width, int height, int partWidth, int partHeight);

		int calculateOffsetX(int x);
		int calculateOffsetY(int y);

		QPixmap getPart(const QString &partName);
		QPixmap getPartOfSize(const QString &partName, const QSize &partSize);
		QPixmap background();
		QPixmap messageBox(const QString &message);

		void resetPlayField();
		void drawPart(QPainter & painter, int x, int y, const QString &svgName);
		void updatePlayField(PlayField &playfield);
		QPixmap *getPlayField();

		QPixmap pixmapFromCache(RendererPrivate *p, const QString &svgName, const QSize &size);

    private:
		RendererPrivate *p;
};

#endif // RENDERER_H
