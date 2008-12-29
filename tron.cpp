/****************************************************************************
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

  ***************************************************************************/

#include "tron.h"

// Background
#include <kio/netaccess.h>
#include <KMessageBox>

// Normal class
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QPoint>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QPixmap>
#include <QKeyEvent>
#include <QVector>
#include <QKeySequence>

#include <KDebug>
#include <KLocale>
#include <KApplication>
#include <KConfig>
#include <KColorDialog>
#include <KStandardAction>
#include <KGameDifficulty>

#include "settings.h"
#include "renderer.h"
#include "object.h"

/**
 * init-functions
 **/

Tron::Tron(QWidget *parent) : QWidget(parent)
{
	players[0] = new Player(pf, 0);
	players[1] = new Player(pf, 1);
	
	connect(players[0], SIGNAL(fetchedItem(int, int, int)), SLOT(itemHit(int, int, int)));
	connect(players[1], SIGNAL(fetchedItem(int, int, int)), SLOT(itemHit(int, int, int)));

	intelligence.referenceTron(this);

	setFocusPolicy(Qt::StrongFocus);

	gameBlocked = false;

	timer = new QTimer(this);
	loadSettings();
	connect(timer, SIGNAL(timeout()), SLOT(doMove()));
}

void Tron::loadSettings(){
	Settings::setDifficulty((int) KGameDifficulty::level());

	createNewPlayfield();
	reset();

	// Velocity
	setVelocity( lineSpeed() );

	// Style
	updatePixmap();
	update();

	// Player 0 is always human
	if (Settings::gameType() == Settings::EnumGameType::PlayerVSPlayer)
	{
		players[1]->setComputer(false);
	}
	else
	{
		players[1]->setComputer(true);
	}
}

Tron::~Tron()
{
	delete timer;
	delete players[0];
	delete players[1];
}

void Tron::resizeRenderer()
{
	// Block size
	blockWidth = width() / (pf.getWidth() + 2);
	blockHeight = height() / (pf.getHeight() + 2);
	if (blockWidth > blockHeight)
	{
		blockWidth = blockHeight;
	}
	else
	{
		blockHeight = blockWidth;
	}

	Renderer::self()->boardResized(width(), height(), blockWidth, blockHeight);

	Renderer::self()->resetPlayField();
}

void Tron::createNewPlayfield()
{
	resizeRenderer();
	
	pf.initialize();
}

void Tron::newGame()
{
	players[0]->resetScore();
	players[1]->resetScore();
	emit gameEnds(KTronEnum::Nobody);
	reset();
}

void Tron::reset()
{
	gamePaused = false;
	stopGame();

	players[0]->reset();
	players[1]->reset();

	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		players[0]->resetScore();
		players[1]->resetScore();

		setVelocity( lineSpeed() );
	}

	pf.initialize();

	// set start coordinates
	players[0]->setStartPosition();
	
	if (Settings::gameType() != Settings::EnumGameType::Snake)
	{
		players[1]->setStartPosition();
	}

	updatePixmap();
	update();

	setFocus();

	emit gameReset();
	emit updatedScore();
}

//
// Getters / Setters
//

PlayField *Tron::getPlayField()
{
	return &pf;
}

Player *Tron::getPlayer(int playerNr)
{
	if (playerNr != 0 && playerNr != 1)
	{
		kDebug() << "Inexistent player requested: " << playerNr;
		return 0;
	}

	return players[playerNr];
}
		

/* *************************************************************** **
**                   	??? functions										 **
** *************************************************************** */

void Tron::startGame()
{
	gameEnded = false;

	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		newApple();
	}

	timer->start(velocity);
}

void Tron::itemHit(int playerNumber, int, int)
{
	//kDebug() << "Got Item Hit for " << playerNumber;

	newApple();
	players[playerNumber]->setEnlargement(3);
	players[playerNumber]->addScore(1);
	if (velocity > 15)
	{
		velocity--;
		timer->stop();
		timer->start(velocity);
	}

	emit updatedScore();
}

void Tron::newApple()
{
	int x = rand() % pf.getWidth();
	int y = rand() % pf.getHeight();

	while (pf.getObjectAt(x, y)->getObjectType() != ObjectType::Object)
	{
		x = rand() % pf.getWidth();
		y = rand() % pf.getHeight();
	}

	//kDebug() << "Drawn apple at (" << x << ", " << y << ")";

	apple.setType((int)(rand() % 3));
	
	pf.setObjectAt(x, y, apple);
}

void Tron::stopGame()
{
	timer->stop();
	gameEnded = true;
}

void Tron::togglePause() // pause or continue game
{
	if (!gameEnded)
	{
		if (gamePaused)
		{
			gamePaused = false;
			update();
			timer->start(velocity);
			
			emit updatedScore();
		}
		else
		{
			gamePaused = true;
			timer->stop();
			update();
			
			emit updatedScore();
		}
	}
}

void Tron::showWinner(KTronEnum::Player player)
{
	update();

	emit gameEnds(player);
}

/* *************************************************************** **
**                    paint functions										 **
** *************************************************************** */

void Tron::updatePixmap()
{
	Renderer::self()->updatePlayField(pf);
}

/* *************************************************************** **
**                    config functions										 **
** *************************************************************** */

void Tron::setVelocity(int newVel)            // set new velocity
{
	velocity = (10 - newVel) * 15;

	if (!gameEnded && !gamePaused)
	{
		timer->start(velocity);
	}
}

/* *************************************************************** **
**                    moving functions										 **
** *************************************************************** */

bool Tron::switchDir(int playerNr, PlayerDirections::Direction newDirection)
{
	if (playerNr != 0 && playerNr != 1)
	{
		Q_ASSERT_X(true, "Tron::switchDir", "wrong playerNr");
		return false;
	}

	if (newDirection == players[playerNr]->getDirection())
		return false;
	if (newDirection == PlayerDirections::Up && players[playerNr]->getDirection() == PlayerDirections::Down)
		return false;
	if (newDirection == PlayerDirections::Down && players[playerNr]->getDirection() == PlayerDirections::Up)
		return false;
	if (newDirection == PlayerDirections::Left && players[playerNr]->getDirection() == PlayerDirections::Right)
		return false;
	if (newDirection == PlayerDirections::Right && players[playerNr]->getDirection() == PlayerDirections::Left)
		return false;

	players[playerNr]->setDirection(newDirection);
	
	return true;
}

/* *************************************************************** **
**                    			Events										 **
** *************************************************************** */

void Tron::paintEvent(QPaintEvent *e)
{
	QPainter p(this);

	p.drawPixmap(e->rect().topLeft(), *Renderer::self()->getPlayField(), e->rect());

	if (gamePaused) // if game is paused, print message
	{
		QString message = i18n("Game paused");
		QPixmap messageBox = Renderer::self()->messageBox(message);
		QPoint point(width() / 2 - messageBox.width() / 2, height() / 2 - messageBox.height() / 2);
		p.drawPixmap(point, messageBox, e->rect());
	}
	else if (gameEnded) // If game ended, print "Crash!"
	{
		QString message = QString("");
		
		if (!players[0]->isAlive() || !players[1]->isAlive()) {
			message += i18n("Crash!");
			message += '\n';
		}

		message += i18n("Press any of your direction keys to start!");

		QPixmap messageBox = Renderer::self()->messageBox(message);
		QPoint point(width() / 2 - messageBox.width() / 2, height() / 2 - messageBox.height() / 2);
		p.drawPixmap(point, messageBox, e->rect());
	}
}

void Tron::resizeEvent(QResizeEvent *)
{
	resizeRenderer();
	updatePixmap();
	update();
}

void Tron::triggerKey(int player, KBAction::Action action, bool trigger)
{
	//kDebug() << "Key hit: " << player << action << trigger;
	if (action == KBAction::ACCELERATE && !trigger)
	{
		switchKeyOff(player, action);
	}
	else
	{
		switchKeyOn(player, action);
	}
}

void Tron::switchKeyOn(int player, KBAction::Action action)
{
	if (!players[player]->isComputer())
	{
		switch (action)
		{
			case KBAction::UP:
				players[player]->setKeyPressed(true);
				if (switchDir(player, PlayerDirections::Up))
				{
					players[player]->movePlayer();
				}
				break;
			case KBAction::DOWN:
				players[player]->setKeyPressed(true);
				if (switchDir(player, PlayerDirections::Down))
				{
					players[player]->movePlayer();
				}
				break;
			case KBAction::LEFT:
				players[player]->setKeyPressed(true);
				if (switchDir(player, PlayerDirections::Left))
				{
					players[player]->movePlayer();
				}
				break;
			case KBAction::RIGHT:
				players[player]->setKeyPressed(true);
				if (switchDir(player, PlayerDirections::Right))
				{
					players[player]->movePlayer();
				}
				break;
			case KBAction::ACCELERATE:
				if(!Settings::acceleratorBlocked())
				{
					//kDebug() << "Acceleration on: " << player;
					players[player]->setAccelerated(true);
				}
				break;
			default:
				break;
		}
	}
	// if both players press keys at the same time, start game...
	if (players[0]->hasKeyPressed() && players[1]->hasKeyPressed())
	{
		// Start game
		if (gameEnded && !gameBlocked)
		{
			reset();
			startGame();
		}
		// ...or continue
		else if (gamePaused)
		{
			togglePause();
		}
	}
}

void Tron::switchKeyOff(int player, KBAction::Action action)
{
	if (!players[player]->isComputer())
	{
		switch (action)
		{
			case KBAction::UP:
			case KBAction::DOWN:
			case KBAction::LEFT:
			case KBAction::RIGHT:
				players[player]->setKeyPressed(false);
				break;
			case KBAction::ACCELERATE:
				//kDebug() << "Acceleration off: " << player;
				players[player]->setAccelerated(false);
				break;
			default:
				break;
		}
	}
}

// if playingfield loses keyboard focus, pause game
void Tron::focusOutEvent(QFocusEvent *)
{
   if(!gameEnded && !gamePaused)
   {
      togglePause();
   }
}

/* *************************************************************** **
**     				slots										 **
** *************************************************************** */

void Tron::unblockGame()
{
	gameBlocked = false;
}

void Tron::movePlayer(int playerNr)
{
	if (playerNr == 0 || playerNr == 1)
	{
		players[playerNr]->movePlayer();
	}
}

// doMove() is called from QTimer
void Tron::doMove()
{
	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		movePlayer(0);

		updatePixmap();
		update();

		if(!players[0]->isAlive())
		{
			stopGame();
			showWinner(KTronEnum::One);
		}
	}
	else
	{
		if(players[0]->isAccelerated() || players[1]->isAccelerated())
		{
			if (players[0]->isAccelerated())
			{
				players[0]->movePlayer();
			}
			
			if (players[1]->isAccelerated())
			{
				players[1]->movePlayer();
			}
			
			/* player collision check */
			if (!players[1]->isAlive())
			{
				int xInc = 0;
				int yInc = 0;
				
				switch (players[1]->getDirection())
				{
					case PlayerDirections::Left:
						xInc = -1;
						break;
					case PlayerDirections::Right:
						xInc = 1;
						break;
					case PlayerDirections::Up:
						yInc = -1;
						break;
					case PlayerDirections::Down:
						yInc = 1;
						break;
					default:
						break;
				}
				
				if ((players[1]->getX() + xInc) == players[0]->getX())
				{
					if ((players[1]->getY() + yInc) == players[0]->getY())
					{
						players[0]->die();
					}
				}
			}

			updatePixmap();
			update();

			// crashtest
			if (!players[0]->isAlive() || !players[1]->isAlive())
			{
				stopGame();
				
				if (!players[0]->isAlive() && !players[1]->isAlive())
				{
					players[0]->addScore(1);
					players[1]->addScore(1);
					showWinner(KTronEnum::Both);
				}
				else if (!players[0]->isAlive())
				{
					showWinner(KTronEnum::Two);
					players[1]->addScore(1);
				}
				else if (!players[1]->isAlive())
				{
					showWinner(KTronEnum::One);
					players[0]->addScore(1);
				}
			}

			if (gameEnded)
			{
				//this is for waiting 0,5s before starting next game
				gameBlocked = true;
				QTimer::singleShot(1000, this, SLOT(unblockGame()));
				return;
			}
		}

		// Player 0 is never a computer nowadays...
		if (players[1]->isComputer())
		{
			intelligence.think(1);
		}

		players[0]->movePlayer();
		players[1]->movePlayer();

		/* player collision check */
		if (!players[1]->isAlive())
		{
			int xInc = 0;
			int yInc = 0;
			
			switch (players[1]->getDirection())
			{
				case PlayerDirections::Left:
					xInc = -1;
					break;
				case PlayerDirections::Right:
					xInc = 1;
					break;
				case PlayerDirections::Up:
					yInc = -1;
					break;
				case PlayerDirections::Down:
					yInc = 1;
					break;
				default:
					break;
			}
			
			if ((players[1]->getX() + xInc) == players[0]->getX())
			{
				if ((players[1]->getY() + yInc) == players[0]->getY())
				{
					players[0]->die();
				}
			}
		}

		updatePixmap();
		update();

		// crashtest
		if (!players[0]->isAlive() || !players[1]->isAlive())
		{
			stopGame();
			
			if (!players[0]->isAlive() && !players[1]->isAlive())
			{
				players[0]->addScore(1);
				players[1]->addScore(1);
				showWinner(KTronEnum::Both);
			}
			else if (!players[0]->isAlive())
			{
				showWinner(KTronEnum::Two);
				players[1]->addScore(1);
			}
			else if (!players[1]->isAlive())
			{
				showWinner(KTronEnum::One);
				players[0]->addScore(1);
			}
		}
	}

	if (gameEnded)
	{
		//this is for waiting 1s before starting next game
		gameBlocked = true;
		QTimer::singleShot(1000, this, SLOT(unblockGame()));
	}
}

/**
 * Skill settings
 */

/** retrieves the line speed */
int Tron::lineSpeed() {
	KGameDifficulty::standardLevel level = KGameDifficulty::level();

	switch (level) {
		case KGameDifficulty::VeryEasy:
			return 2;
		default:
		case KGameDifficulty::Easy:
			return 3;
		case KGameDifficulty::Medium:
			return 5;
		case KGameDifficulty::Hard:
			return 7;
		case KGameDifficulty::VeryHard:
			return 8;
	}
}

bool Tron::running() {
	return !gameEnded;
}

bool Tron::paused() {
	return !gameEnded && gamePaused;
}


#include "tron.moc"

