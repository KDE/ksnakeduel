/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef RENDERER_H
#define RENDERER_H

#include "playfield.h"

#include <QSvgRenderer>
#include <QPainter>
#include <QString>
#include <QSize>

class QPixmap;

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

    private:
		QSize m_sceneSize;
		QSize m_partSize;

		QSvgRenderer m_renderer;

		QPixmap *m_playField = nullptr;

		QString m_currentTheme;
};

#endif // RENDERER_H
