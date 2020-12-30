/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#include "renderer.h"
#include "settings.h"
#include "object.h"

#include <QPixmap>
#include <QPixmapCache>
#include <QSvgRenderer>

#include <KFontUtils>
#include "ksnakeduel_debug.h"

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgametheme.h>

class RendererPrivate
{
	public:
		RendererPrivate();
		~RendererPrivate();

		QSize m_sceneSize;
		QSize m_partSize;

		QSvgRenderer m_renderer;

		QPixmap *m_playField;

		QString m_currentTheme;
};

const QString sizeSuffix(QStringLiteral( "_%1-%2" ));
const QString frameSuffix(QStringLiteral( "-%1" ));

RendererPrivate::RendererPrivate()
    : m_renderer()
{
	QPixmapCache::setCacheLimit(40);
	QPixmapCache::clear();
    m_playField = nullptr;
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
    delete p;
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
        QPixmapCache::clear();
    return true;
}

QPixmap Renderer::getPart(const QString &frameSvgName)
{
	return getPartOfSize(frameSvgName, p->m_partSize);
}

QPixmap Renderer::getPartOfSize(const QString &frameSvgName, const QSize &partSize)
{
	QString framePixName = frameSvgName + sizeSuffix.arg(partSize.width()).arg(partSize.height());
	QPixmap pix;
        if (!QPixmapCache::find(framePixName, &pix))
	{
		pix = QPixmap(partSize);
		pix.fill(Qt::transparent);
		QPainter painter(&pix);
		p->m_renderer.render(&painter, frameSvgName);
		painter.end();
		QPixmapCache::insert(framePixName, pix);
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

    if (!QPixmapCache::find(pixName, &pix))
    {
        pix = QPixmap(size);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        p->m_renderer.render(&painter, svgName);
        painter.end();
        QPixmapCache::insert(pixName, pix);
    }

    return pix;
}

QPixmap Renderer::background()
{
    QPixmap pix;
    QString pixName = QLatin1String( "bgtile" ) + sizeSuffix.arg(p->m_sceneSize.width()).arg(p->m_sceneSize.height());
        if (!QPixmapCache::find(pixName, &pix))
	{
		pix = QPixmap(p->m_sceneSize);
		pix.fill(Qt::white);
		QPainter painter(&pix);

		QPixmap bgPix = getPart(QStringLiteral( "bgtile" ));
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
		QPixmapCache::insert(pixName, pix);
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
				QPixmap part = Renderer::self()->getPart(QStringLiteral( "border" ));
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

void Renderer::drawPart(QPainter & painter, int x, int y, const QString &svgName)
{
	//qCDebug(KSNAKEDUEL_LOG) << "Drawing part: " << svgName;

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

QPixmap Renderer::messageBox(const QString &message) {
	int w = p->m_sceneSize.width() / 2;
	int h = p->m_sceneSize.height() / 3;

	QSize size(w, h);
	QPixmap pixmap = getPartOfSize(QStringLiteral( "display" ),  size);

	QPainter painter(&pixmap);

	const int fontSize = KFontUtils::adaptFontSize(painter, message, w * 0.9, h, 28, 1, KFontUtils::DoNotAllowWordWrap);

	painter.setPen(QColor(255, 255, 255, 220));
	painter.setFont(QFont(QStringLiteral( "Helvetica" ), fontSize, QFont::Bold));
	painter.drawText(QRectF(0, 0, w, h), Qt::AlignCenter, message);

	painter.end();

	return pixmap;
}
