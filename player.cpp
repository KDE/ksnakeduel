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

#include "player.h"

#include "tron.h"
#include "snakepart.h"
#include "settings.h"

#include <KDebug>
#include <KLocale>
#include <KUser>

Player::Player(PlayField &pf, int playerNr) : QObject()
{
	m_playField = &pf;
	m_playerNumber = playerNr;
	m_computer = false;

	// Calling setName() with an empty string will cause the defaults
	setName(QString());

	m_score = 0;
	m_dir = PlayerDirections::Up;
	m_enlarge = 0;
	m_blockSwitchDir = false;
	
	reset();
}

//
// Get / Set
//

int Player::getPlayerNumber()
{
	return m_playerNumber;
}

int Player::getX()
{
	if (m_snakeParts.isEmpty())
	{
		kDebug() << "Requested coordinate of inexistant snake";
		return 0;
	}
	
	return m_snakeParts.last().getX();
}

int Player::getY()
{
	if (m_snakeParts.isEmpty())
	{
		kDebug() << "Requested coordinate of inexistant snake";
		return 0;
	}
	
	return m_snakeParts.last().getY();
}

int Player::getScore()
{
	return m_score;
}

//
// Player name
//

QString Player::getName()
{
	if (isComputer())
	{
		return i18n("KTron");
	}
	else
	{
		return m_name;
	}
}

void Player::setName(QString name)
{
	if (name.isEmpty())
	{
		// If first player, name it after the user
		KUser thisUser = KUser();

		if (m_playerNumber == 0 && thisUser.property(KUser::FullName).isValid())
		{
			this->m_name = thisUser.property(KUser::FullName).toString();
		}
		else
		{
			this->m_name = i18n("Player %1", m_playerNumber + 1);
		}
	}
	else
	{
		this->m_name = name;
	}
}

//
// Snake growing
//

void Player::setEnlargement(int enlargement)
{
	if (enlargement < 0)
	{
		return;
	}

	m_enlarge += enlargement;
}

//
// Directions
//

PlayerDirections::Direction Player::getDirection()
{
	return m_dir;
}

void Player::setDirection(PlayerDirections::Direction direction)
{
	if (m_blockSwitchDir)
	{
		return;
	}
	else if (direction == PlayerDirections::Up && m_dir == PlayerDirections::Down)
	{
		return;
	}
	else if (direction == PlayerDirections::Down && m_dir == PlayerDirections::Up)
	{
		return;
	}
	else if (direction == PlayerDirections::Left && m_dir == PlayerDirections::Right)
	{
		return;
	}
	else if (direction == PlayerDirections::Right && m_dir == PlayerDirections::Left)
	{
		return;
	}
	
	m_dir = direction;
	m_blockSwitchDir = true;
}

//
// Live Control
//

bool Player::isAlive()
{
	return m_alive;
}

void Player::die()
{
	m_alive = false;
}

//
// Score updating
//

void Player::addScore(int increment)
{
	if (increment < 0)
	{
		return;
	}

	m_score += increment;
}

void Player::resetScore()
{
	m_score = 0;
}

//
// Start
//

void Player::setStartPosition()
{
	while (!m_snakeParts.isEmpty())
	{
		m_snakeParts.dequeue();
	}
	
	int x = (2 - getPlayerNumber()) * m_playField->getWidth() / 3;
	int y = m_playField->getHeight() / 2;
	
	SnakePart head(getPlayerNumber());
	head.setPartTop(true);
	head.setPartLeft(true);
	head.setPartRight(true);
	head.setPartType(SnakePartType::Head);
	head.generateSVGName();
	
	SnakePart tail(getPlayerNumber());
	tail.setPartBottom(true);
	tail.setPartLeft(true);
	tail.setPartRight(true);
	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		tail.setPartType(SnakePartType::Tail);
	}
	else
	{
		tail.setPartType(SnakePartType::Hole);
	}
	tail.generateSVGName();
	
	m_playField->setObjectAt(x, y, head);
	m_playField->setObjectAt(x, y + 1, tail);
	
	m_snakeParts.enqueue(tail);
	m_snakeParts.enqueue(head);
	
	// Make the computer player start some random direction
	if (isComputer())
	{
		switch ((int)(rand() % 3))
		{
			default:
			case 0:
				m_dir = PlayerDirections::Up;
				break;
			case 1:
				m_dir = PlayerDirections::Left;
				break;
			case 2:
				m_dir = PlayerDirections::Right;
				break;
		}
		
		m_blockSwitchDir = true;
	}
	else
	{
		m_dir = PlayerDirections::Up;
	}
}

//
// Movement
//

void Player::movePlayer()
{
	int oldX = m_snakeParts.last().getX();
	int oldY = m_snakeParts.last().getY();
	SnakePart newHead(m_playerNumber);

	int newX = oldX;
	int newY = oldY;

	newHead.setPartType(SnakePartType::Head);

	switch (m_dir)
	{
		case PlayerDirections::Up:
			newY--;
			newHead.setPartTop(true);
			newHead.setPartLeft(true);
			newHead.setPartRight(true);
			break;
		case PlayerDirections::Down:
			newY++;
			newHead.setPartBottom(true);
			newHead.setPartLeft(true);
			newHead.setPartRight(true);
			break;
		case PlayerDirections::Left:
			newX--;
			newHead.setPartTop(true);
			newHead.setPartBottom(true);
			newHead.setPartLeft(true);
			break;
		case PlayerDirections::Right:
			newX++;
			newHead.setPartTop(true);
			newHead.setPartBottom(true);
			newHead.setPartRight(true);
			break;
		default:
			break;
	}
	
	if (crashed(newX, newY))
	{
		//kDebug() << "Crashed at: (" << newX << ", " << newY << ")";
		m_alive = false;
	}
	
	if (m_alive)
	{
		switch (m_dir)
		{
			// unset drawing flags in the moving direction
			case PlayerDirections::Up:
				m_snakeParts.last().setPartTop(false);
				break;
			case PlayerDirections::Down:
				m_snakeParts.last().setPartBottom(false);
				break;
			case PlayerDirections::Right:
				m_snakeParts.last().setPartRight(false);
				break;
			case PlayerDirections::Left:
				m_snakeParts.last().setPartLeft(false);
				break;
			default:
				break;
		}
		m_snakeParts.last().setPartType(SnakePartType::Body);
		m_snakeParts.last().generateSVGName();
		m_playField->setObjectAt(m_snakeParts.last().getX(), m_snakeParts.last().getY(), m_snakeParts.last());
		
		if (m_playField->getObjectAt(newX, newY)->getObjectType() == ObjectType::Item)
		{
			//kDebug() << "Boom!";
			emit fetchedItem(m_playerNumber, newX, newY);
		}

		newHead.generateSVGName();
		m_playField->setObjectAt(newX, newY, newHead);
		m_snakeParts.enqueue(newHead);
	}

	if (m_alive && m_enlarge == 0 && Settings::gameType() == Settings::EnumGameType::Snake)
	{
		SnakePart oldTail = m_snakeParts.dequeue();
		
		if (!oldTail.getPartTop())
		{
			m_snakeParts.head().setPartBottom(true);
		}
		else if (!oldTail.getPartBottom())
		{
			m_snakeParts.head().setPartTop(true);
		}
		else if (!oldTail.getPartLeft())
		{
			m_snakeParts.head().setPartRight(true);
		}
		else if (!oldTail.getPartRight())
		{
			m_snakeParts.head().setPartLeft(true);
		}
		
		m_snakeParts.head().setPartType(SnakePartType::Tail);
		m_snakeParts.head().generateSVGName();
		m_playField->setObjectAt(m_snakeParts.head().getX(), m_snakeParts.head().getY(), m_snakeParts.head());
		
		Object emptyObject = Object();
		m_playField->setObjectAt(oldTail.getX(), oldTail.getY(), emptyObject);
	}
	else if (m_enlarge > 0)
	{
		m_enlarge--;
	}
	
	m_blockSwitchDir = false;
}

//
// Crash check
//

bool Player::crashed(int x, int y)
{
	if (x < 0 || y < 0 || x >= m_playField->getWidth() || y >= m_playField->getHeight())
	{
		return true;
	}
	else
	{
		ObjectType::Type objType = m_playField->getObjectAt(x, y)->getObjectType();
		return (objType != ObjectType::Item && objType != ObjectType::Object);
	}
}

//
// Reset
//

void Player::reset()
{
	m_alive = true;
	m_accelerated = false;
	m_enlarge = 0;
	m_keyPressed = m_computer;
	m_blockSwitchDir = false;
}

//
// Computer powered
//

bool Player::isComputer()
{
	return m_computer;
}

void Player::setComputer(bool isComputer)
{
	m_computer = isComputer;
	m_keyPressed = m_computer;
}

//
// Acceleration
//

bool Player::isAccelerated()
{
	return m_accelerated;
}

void Player::setAccelerated(bool value)
{
	m_accelerated = value;
}


//
// Key pressed
//

bool Player::hasKeyPressed()
{
	return m_keyPressed;
}

void Player::setKeyPressed(bool value)
{
	m_keyPressed = value;
}

