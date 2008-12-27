/********************************************************************************** 
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

  *******************************************************************************/  

#include "item.h"

#include "tron.h"

Item::Item() : Object(ObjectType::Item)
{
	setSVGName("item1");
	setOldType(KTronEnum::ITEM1);
}

//
// Getters / Setters
//

void Item::setType(int t)
{
	type = t;
	
	switch (type)
	{
		default:
		case 0:
			setSVGName("item1");
			setOldType(KTronEnum::ITEM1);
			break;
		case 1:
			setSVGName("item2");
			setOldType(KTronEnum::ITEM2);
			break;
		case 2:
			setSVGName("item3");
			setOldType(KTronEnum::ITEM3);
			break;
	}
}

int Item::getType()
{
	return type;
}

