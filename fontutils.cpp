/*
    Copyright 2005-2006 by Albert Astals Cid <aacid@kde.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "fontutils.h"

#include <QPainter>

int fontUtils::fontSize(QPainter &p, const QString &str, 
                        int w, int h, Flags flags)
{
    int size;
    QRect rect;
    bool done = false;

    size = 28;

    while (!done && size > 1) {
        QFont f = p.font();
        f.setPointSize(size);
        p.setFont(f);
        int qtFlags = Qt::AlignCenter | Qt::TextWordWrap;
        if (flags & DoNotAllowWordWrap) {
            qtFlags = qtFlags & ~Qt::TextWordWrap;
        }
        rect = p.boundingRect(QRect(0, 0, w, h), qtFlags, str);
        
        if (rect.width() == 0 || rect.height() == 0) {
            return -1;
        } else if (rect.width() > w || rect.height() > h) {
            size = qMin(w * size / rect.width(), h * size / rect.height());
        }
        else done = true;
    }

    return size;
}
