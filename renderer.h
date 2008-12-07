/* ****************************************************************************
   This file is part of the game 'KTron'

  Copyright (C) 1998-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
  Copyright (C) 2005 Benjamin C. Meyer <ben at meyerhome dot net>
  Copyright (C) 2008 Stas Verberkt <legolas at legolasweb dot nl>

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

  *****************************************************************************/

#ifndef RENDERER_H
#define RENDERER_H

#include <QPainter>
#include <QString>
#include <QSize>
#include <QVector>

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

		QPixmap getPart(QString partName);
		QPixmap getPartOfSize(QString partName, QSize &partSize);
		QPixmap snakePart(int);
		QString decodePart(int);
		QPixmap background();
		QPixmap messageBox(QString &message);

		void resetPlayField();
		void drawPart(QVector< QVector<int> > &playfield, QPainter & painter, int x, int y);
		void updatePlayField(QVector< QVector<int> > &playfield);
		QPixmap *getPlayField();

		QPixmap pixmapFromCache(RendererPrivate *p, const QString &svgName, const QSize &size);

    private:
		RendererPrivate *p;
};

#endif // RENDERER_H
