/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#include "renderer.h"

#include "object.h"
#include "ksnakeduel_debug.h"
// KDEGames
#include <KGameThemeProvider>
// KF
#include <KFontUtils>

static KGameThemeProvider *provider()
{
    auto *prov = new KGameThemeProvider(QByteArray()); // empty config key to disable internal config storage
    prov->discoverThemes(
        QStringLiteral("themes"), // theme file location
        QStringLiteral("default") // default theme file name
    );
    return prov;
}


Renderer::Renderer()
    : KGameRenderer(provider())
{
}


Renderer *Renderer::self()
{
    static Renderer r;
    return &r;
}

bool Renderer::loadTheme(const QString &name)
{
	const QByteArray identifier = name.toUtf8();
	KGameThemeProvider *provider = themeProvider();
	const QList<const KGameTheme *> themes = provider->themes();
	for (auto* theme : themes) {
	    if (theme->identifier() == identifier) {
		provider->setCurrentTheme(theme);
		return true;
	    }
	}
	return false;
}

QPixmap Renderer::getPart(const QString &frameSvgName)
{
	return getPartOfSize(frameSvgName, m_partSize);
}

QPixmap Renderer::getPartOfSize(const QString &frameSvgName, const QSize &partSize)
{
    return spritePixmap(frameSvgName, partSize);
}

QPixmap Renderer::background()
{
        if (m_background.isNull())
	{
		m_background = QPixmap(m_sceneSize);
		m_background.fill(Qt::white);
		QPainter painter(&m_background);

		QPixmap bgPix = getPart(QStringLiteral( "bgtile" ));
		if (!bgPix.isNull())
		{
			m_background.fill(Qt::white);
			int pw = bgPix.width();
			int ph = bgPix.height();
			for (int x = 0; x <= m_sceneSize.width(); x += pw) {
				for (int y = 0; y <= m_sceneSize.height(); y += ph) {
					painter.drawPixmap(x, y, bgPix);
				}
			}
		}
		else
		{
			m_background.fill(Qt::green);
		}

		painter.end();
	}

	// Tiled background
	return m_background;
}

void Renderer::boardResized(int width, int height, int partWidth, int partHeight)
{
	//new metrics
	m_sceneSize = QSize(width, height);
	m_partSize = QSize(partWidth, partHeight);

	clearPixmapCache();
}

void Renderer::clearPixmapCache()
{
	m_background = QPixmap();
	m_playField = QPixmap();
}

void Renderer::updatePlayField(PlayField &playfield)
{
	int i, j;

	if (m_playField.isNull())
	{
		if (m_sceneSize.isEmpty()) {
		    return;
		}
		m_playField = QPixmap(m_sceneSize);
	}

	QPainter painter;
	painter.begin(&m_playField);

	QPixmap bgPix = background();
	painter.drawPixmap(0, 0, bgPix);

	// Draw border
	const QPixmap part = Renderer::self()->getPart(QStringLiteral( "border" ));
	for (i = 0; i < playfield.getWidth() + 2; ++i)
	{
		for (j = 0; j < playfield.getHeight() + 2; ++j)
		{
			if (i == 0 || i == playfield.getWidth() + 1 || j == 0 || j == playfield.getHeight() + 1)
			{
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
	return (x * m_partSize.width()) + (m_sceneSize.width() - (TRON_PLAYFIELD_WIDTH + 2) * m_partSize.width()) / 2;
}

int Renderer::calculateOffsetY(int y)
{
	return (y * m_partSize.height()) + (m_sceneSize.height() - (TRON_PLAYFIELD_HEIGHT + 2) * m_partSize.height()) / 2;
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
	return &m_playField;
}

QPixmap Renderer::messageBox(const QString &message) {
	int w = m_sceneSize.width() / 2;
	int h = m_sceneSize.height() / 3;

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
