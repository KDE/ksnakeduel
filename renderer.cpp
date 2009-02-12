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

#include "renderer.h"
#include "settings.h"
#include "object.h"
#include "fontutils.h"

#include <QPainter>
#include <QPixmap>
#include <QSize>

#include <KGameTheme>
#include <KPixmapCache>
#include <KSvgRenderer>
#include <KDebug>

class RendererPrivate
{
	public:
		RendererPrivate();
		~RendererPrivate();

		QSize m_sceneSize;
		QSize m_partSize;

		KSvgRenderer m_renderer;
		KPixmapCache m_cache;

		QPixmap *m_playField;

		QString m_currentTheme;
};

const QString sizeSuffix("_%1-%2");
const QString frameSuffix("-%1");

RendererPrivate::RendererPrivate()
    : m_renderer()
    , m_cache("ktron-cache")
{
    m_cache.setCacheLimit(3 * 1024);
    m_cache.discard();
	m_playField = 0;
}

RendererPrivate::~RendererPrivate()
{
	delete m_playField;
}

Renderer::Renderer()
    : p(new RendererPrivate)
{
    loadTheme(Settings::theme());
}

Renderer::Renderer(const Renderer &)
{
}

Renderer::~Renderer()
{
}

Renderer *Renderer::self()
{
    static Renderer r;
    return &r;
}

bool Renderer::loadTheme(const QString &name)
{
    bool discardCache = !p->m_currentTheme.isEmpty();
    if (!p->m_currentTheme.isEmpty() && p->m_currentTheme == name)
        return true; //requested to load the theme that is already loaded
    KGameTheme theme;
    //try to load theme
    if (!theme.load(name))
    {
        if (!theme.loadDefault())
            return false;
    }
    p->m_currentTheme = name;

    //load graphics
    if (!p->m_renderer.load(theme.graphics()))
        return false;
    //flush cache
    if (discardCache)
        p->m_cache.discard();
    return true;
}

QPixmap Renderer::getPart(QString frameSvgName)
{
	return getPartOfSize(frameSvgName, p->m_partSize);
}

QPixmap Renderer::getPartOfSize(QString frameSvgName, QSize &partSize)
{
	QString framePixName = frameSvgName + sizeSuffix.arg(partSize.width()).arg(partSize.height());
	QPixmap pix;
	if (!p->m_cache.find(framePixName, pix))
	{
		pix = QPixmap(partSize);
		pix.fill(Qt::transparent);
		QPainter painter(&pix);
		p->m_renderer.render(&painter, frameSvgName);
		painter.end();
		p->m_cache.insert(framePixName, pix);
	}

    //return the static pixmap
    return pixmapFromCache(p, frameSvgName, partSize);
}

QPixmap Renderer::pixmapFromCache(RendererPrivate *p, const QString &svgName, const QSize &size)
{
    if (size.isEmpty())
        return QPixmap();
    QPixmap pix;
    QString pixName = svgName + sizeSuffix.arg(size.width()).arg(size.height());

    if (!p->m_cache.find(pixName, pix))
    {
        pix = QPixmap(size);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        p->m_renderer.render(&painter, svgName);
        painter.end();
        p->m_cache.insert(pixName, pix);
    }
	
    return pix;
}

QPixmap Renderer::background()
{
    QPixmap pix;
    QString pixName = "bgtile" + sizeSuffix.arg(p->m_sceneSize.width()).arg(p->m_sceneSize.height());
	if (!p->m_cache.find(pixName, pix))
	{
		pix = QPixmap(p->m_sceneSize);
		pix.fill(Qt::white);
		QPainter painter(&pix);
		
		QPixmap bgPix = getPart("bgtile");
		if (!bgPix.isNull())
		{
			pix.fill(Qt::white);
			int pw = bgPix.width();
			int ph = bgPix.height();
			for (int x = 0; x <= p->m_sceneSize.width(); x += pw) {
				for (int y = 0; y <= p->m_sceneSize.height(); y += ph) {
					painter.drawPixmap(x, y, bgPix);
				}
			}
		}
		else
		{
			pix.fill(Qt::green);
		}
		
		painter.end();
		p->m_cache.insert(pixName, pix);
	}
	
	// Tiled background
	return pix;
}

void Renderer::boardResized(int width, int height, int partWidth, int partHeight)
{
	//new metrics
	p->m_sceneSize = QSize(width, height);
	p->m_partSize = QSize(partWidth, partHeight);
}

void Renderer::resetPlayField()
{
	delete p->m_playField;
	p->m_playField = new QPixmap(p->m_sceneSize);
	//p->m_playField->fill(Qt::green);
}

void Renderer::updatePlayField(PlayField &playfield)
{
	int i, j;

	if (!p->m_playField)
	{
		resetPlayField();
	}

	QPainter painter;
	painter.begin(p->m_playField);
	
	QPixmap bgPix = background();
	painter.drawPixmap(0, 0, bgPix);

	// Draw border
	for (i = 0; i < playfield.getWidth() + 2; ++i) 
	{
		for (j = 0; j < playfield.getHeight() + 2; ++j) 
		{
			if (i == 0 || i == playfield.getWidth() + 1 || j == 0 || j == playfield.getHeight() + 1)
			{
				QPixmap part = Renderer::self()->getPart("border");
				painter.drawPixmap(calculateOffsetX(i), calculateOffsetY(j), part);
			}
		}
	}

	// Examine all pixels and draw
	for(i = 0; i < playfield.getWidth(); ++i)
	{
		for(j = 0; j < playfield.getHeight(); ++j)
		{
			if (playfield.getObjectAt(i, j)->getObjectType() != ObjectType::Object)
			{
				drawPart(painter, i, j, playfield.getObjectAt(i, j)->getSVGName());
			}
		}
	}

	painter.end();
}

int Renderer::calculateOffsetX(int x)
{
	return (x * p->m_partSize.width()) + (p->m_sceneSize.width() - (TRON_PLAYFIELD_WIDTH + 2) * p->m_partSize.width()) / 2;
}

int Renderer::calculateOffsetY(int y)
{
	return (y * p->m_partSize.height()) + (p->m_sceneSize.height() - (TRON_PLAYFIELD_HEIGHT + 2) * p->m_partSize.height()) / 2;
}

void Renderer::drawPart(QPainter & painter, int x, int y, QString svgName)
{
	//kDebug() << "Drawing part: " << svgName;

	int xOffset = calculateOffsetX(x + 1);
	int yOffset = calculateOffsetY(y + 1);

	//int type = playfield[x][y];

	QPixmap snakePart = Renderer::self()->getPart(svgName);

	painter.drawPixmap(xOffset, yOffset, snakePart);
}

QPixmap *Renderer::getPlayField()
{
	return p->m_playField;
}

QPixmap Renderer::messageBox(QString &message) {
	int w = p->m_sceneSize.width() / 2;
	int h = p->m_sceneSize.height() / 3;
	
	QSize size(w, h);
	QPixmap pixmap = getPartOfSize("display",  size);
	
	QPainter painter(&pixmap);

	int fontSize = fontUtils::fontSize(painter, message, w * 0.9, h, fontUtils::DoNotAllowWordWrap);
	
	painter.setPen(QColor(255, 255, 255, 220));
	painter.setFont(QFont("Helvetica", fontSize, QFont::Bold));
	painter.drawText(QRectF(0, 0, w, h), Qt::AlignCenter, message);
	
	painter.end();
	
	return pixmap;
}
