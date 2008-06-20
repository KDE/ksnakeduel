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

#include "renderer.h"
#include "settings.h"
#include "tron.h"

#include <QPainter>
#include <QPixmap>

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

QString Renderer::decodePart(int type)
{
	QString name;

	// Player
	if (type & KTronEnum::PLAYER1)
	{
		name = "tron1-";
	}
	else if (type & KTronEnum::PLAYER2)
	{
		name = "tron2-";
	}

	// Heads (or tails)
	if (type & KTronEnum::HEAD)
	{
		if ((type & KTronEnum::TOP) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "head-north";
		}
		else if ((type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "head-south";
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT))
		{
			name += "head-west";
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::RIGHT))
		{
			name += "head-east";
		}

		return name;
	}
	else
	{
		if ((type & KTronEnum::TOP) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "tail-south";
			return name;
		}
		else if ((type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
		{
			name += "tail-north";
			return name;
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::LEFT))
		{
			name += "tail-east";
			return name;
		}
		else if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM) && (type & KTronEnum::RIGHT))
		{
			name += "tail-west";
			return name;
		}
	}

	// Bodys
	if ((type & KTronEnum::TOP) && (type & KTronEnum::BOTTOM))
	{
		name += "body-h";
	}
	else if ((type & KTronEnum::LEFT) && (type & KTronEnum::RIGHT))
	{
		name += "body-v";
	}
	else if ((type & KTronEnum::LEFT) && (type & KTronEnum::TOP))
	{
		name += "body-nw";
	}
	else if ((type & KTronEnum::TOP) && (type & KTronEnum::RIGHT))
	{
		name += "body-ne";
	}
	else if ((type & KTronEnum::LEFT) && (type & KTronEnum::BOTTOM))
	{
		name += "body-sw";
	}
	else if ((type & KTronEnum::BOTTOM) && (type & KTronEnum::RIGHT))
	{
		name += "body-se";
	}

	return name;
}

QPixmap Renderer::snakePart(int part)
{
    QString frameSvgName = decodePart(part);

	QString framePixName = frameSvgName + sizeSuffix.arg(p->m_partSize.width()).arg(p->m_partSize.height());
	QPixmap pix;
	if (!p->m_cache.find(framePixName, pix))
	{
		pix = QPixmap(p->m_partSize);
		pix.fill(Qt::transparent);
		QPainter painter(&pix);
		p->m_renderer.render(&painter, frameSvgName);
		painter.end();
		p->m_cache.insert(framePixName, pix);
	}

    //return the static pixmap
    return pixmapFromCache(p, frameSvgName, p->m_partSize);
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
    return pixmapFromCache(p, "ktron-background", p->m_sceneSize);
}

void Renderer::boardResized(int width, int height, int leftOffset, int topOffset, int partWidth, int partHeight)
{
    //new metrics
    p->m_sceneSize = QSize(width, height);
    p->m_partSize = QSize(partWidth, partHeight);
    
    const QString svgName("ktron-background");
    
    QString pixName = svgName + sizeSuffix.arg(width).arg(height);
    QPixmap pix;
    if (!p->m_cache.find(pixName, pix))
    {
        pix = QPixmap(p->m_sceneSize);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        p->m_renderer.render(&painter, svgName);
        
        painter.end();
        p->m_cache.insert(pixName, pix);
    }
}
