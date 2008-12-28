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

#include "item.h"
#include "settings.h"
#include "renderer.h"
#include "snakepart.h"
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
	createNewPlayfield();
	reset();

	// Velocity
	setVelocity( lineSpeed() );

	// Style
	updatePixmap();
	update();

	if (Settings::gameType() == Settings::EnumGameType::PlayerVSPlayer)
	{
		setComputerplayer(KTronEnum::One, false);
		setComputerplayer(KTronEnum::Two, false);
	}
	else
	{
		setComputerplayer(KTronEnum::One, false);
		setComputerplayer(KTronEnum::Two, true);
	}
}

Tron::~Tron()
{
	delete timer;
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
	players[0]->score = 0;
	players[1]->score = 0;
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
		players[0]->score = 0;
		players[1]->score = 0;

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

void Tron::computerStart()
{
	if(isComputer(KTronEnum::Both))
	{
		reset();
		startGame();
	}
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
	players[playerNumber]->enlarge = 3;
	players[playerNumber]->score++;
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

	//apple.setCoordinates(x, y);
	apple.setType((int)(rand() % 3));
	
	pf.setObjectAt(x, y, apple);
}

void Tron::stopGame()
{
	timer->stop();
	gameEnded = true;
	players[0]->dir = PlayerDirections::Up;
	players[1]->dir = PlayerDirections::Up;
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

	if (isComputer(KTronEnum::Both))
	{
		QTimer::singleShot(1000, this, SLOT(computerStart()));
	}
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

void Tron::setComputerplayer(KTronEnum::Player player, bool flag) {
	if(player == KTronEnum::One)
	{
		players[0]->setComputer(flag);
	}
	else if (player == KTronEnum::Two)
	{
		players[1]->setComputer(flag);
	}

	if (isComputer(KTronEnum::Both))
	{
		QTimer::singleShot(1000, this, SLOT(computerStart()));
	}
}

bool Tron::isComputer(KTronEnum::Player player)
{
	if (player == KTronEnum::One)
	{
		return players[0]->computer;
	}
	else if (player == KTronEnum::Two)
	{
		return players[1]->computer;
	}
	else if (player == KTronEnum::Both)
	{
		if(players[0]->computer && players[1]->computer)
		{
			return true;
		}
	}

	return false;
}

/* *************************************************************** **
**                    moving functions										 **
** *************************************************************** */

void Tron::switchDir(int playerNr, PlayerDirections::Direction newDirection)
{
	if (playerNr != 0 && playerNr != 1)
	{
		Q_ASSERT_X(true, "Tron::switchDir", "wrong playerNr");
		return;
	}

	if (newDirection == PlayerDirections::Up && players[playerNr]->dir == PlayerDirections::Down)
		return;
	if (newDirection == PlayerDirections::Down && players[playerNr]->dir == PlayerDirections::Up)
		return;
	if (newDirection == PlayerDirections::Left && players[playerNr]->dir == PlayerDirections::Right)
		return;
	if (newDirection == PlayerDirections::Right && players[playerNr]->dir == PlayerDirections::Left)
		return;

	players[playerNr]->dir = newDirection;
}

/* *************************************************************** **
**                    			Events										 **
** *************************************************************** */

void Tron::paintEvent(QPaintEvent *e)
{
	QPainter p(this);

	p.drawPixmap(e->rect().topLeft(), *Renderer::self()->getPlayField(), e->rect());

	if(gamePaused) // if game is paused, print message
	{
		QString message = i18n("Game paused");
		QPixmap messageBox = Renderer::self()->messageBox(message);
		QPoint point(width() / 2 - messageBox.width() / 2, height() / 2 - messageBox.height() / 2);
		p.drawPixmap(point, messageBox, e->rect());
	}
	else if (gameEnded) // If game ended, print "Crash!"
	{
		QString message = QString("");
		
		if (!players[0]->alive || !players[1]->alive) {
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
	if (!players[player]->computer)
	{
		switch (action)
		{
			case KBAction::UP:
				players[player]->keyPressed = true;
				if (players[player]->dir != PlayerDirections::Up)
				{
					switchDir(player, PlayerDirections::Up);
					movePlayer(player);
				}
				break;
			case KBAction::DOWN:
				players[player]->keyPressed = true;
				if (players[player]->dir != PlayerDirections::Down)
				{
					switchDir(player, PlayerDirections::Down);
					movePlayer(player);
				}
				break;
			case KBAction::LEFT:
				players[player]->keyPressed = true;
				if (players[player]->dir != PlayerDirections::Left)
				{
					switchDir(player, PlayerDirections::Left);
					movePlayer(player);
				}
				break;
			case KBAction::RIGHT:
				players[player]->keyPressed = true;
				if (players[player]->dir != PlayerDirections::Right)
				{
					switchDir(player, PlayerDirections::Right);
					movePlayer(player);
				}
				break;
			case KBAction::ACCELERATE:
				if(!Settings::acceleratorBlocked())
				{
					//kDebug() << "Acceleration on: " << player;
					players[player]->accelerated = true;
				}
				break;
			default:
				break;
		}
	}
	// if both players press keys at the same time, start game...
	if(gameEnded && !gameBlocked)
	{
		if(players[0]->keyPressed && players[1]->keyPressed)
		{
			reset();
			startGame();
		}
	}
	// ...or continue
	else if(gamePaused)
	{
		if(players[0]->keyPressed && players[1]->keyPressed)
		{
			togglePause();
		}
	}
}

void Tron::switchKeyOff(int player, KBAction::Action action)
{
	if (!players[player]->computer)
	{
		switch (action)
		{
			case KBAction::UP:
			case KBAction::DOWN:
			case KBAction::LEFT:
			case KBAction::RIGHT:
				players[player]->keyPressed = false;
				break;
			case KBAction::ACCELERATE:
				//kDebug() << "Acceleration off: " << player;
				players[player]->accelerated = false;
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
  gameBlocked=false;
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

		if(!players[0]->alive)
		{
			stopGame();
			showWinner(KTronEnum::One);
		}
	}
	else
	{
		int i;
		for(i = 0; i < 2; ++i)
		{
			// Decide if the accelerator key was pressed...
			if (players[i]->accelerated)
			{
				movePlayer(i);
			}
		}

		if(players[0]->accelerated || players[1]->accelerated)
		{
			/* player collision check */
			if(!players[1]->alive)
			{
				int xInc=0,yInc=0;
				switch(players[1]->dir)
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
						players[0]->alive=false;
					}
				}
			}

			updatePixmap();
			update();

			// crashtest
			if(!players[0]->alive && !players[1]->alive)
			{
				stopGame();
				players[0]->score++;
				players[1]->score++;
				showWinner(KTronEnum::Both);
			}
			else
			{
				for(i = 0; i < 2; ++i)
				{
					if(!players[i]->alive)
					{
					stopGame();
					showWinner((i==0)? KTronEnum::Two : KTronEnum::One);
					players[(1 - i)]->score++;
					}
				}
			}


			if(gameEnded)
			{
				//this is for waiting 0,5s before starting next game
				gameBlocked=true;
				QTimer::singleShot(1000,this,SLOT(unblockGame()));
			return;
			}
		}

		// neue Spielerstandorte festlegen
		for (i = 0; i < 2; ++i)
		{
			if(players[i]->computer)
				think(i);
		}

		//updateDirections(0);
		update();

		for (i = 0; i < 2; ++i)
		{
			movePlayer(i);
		}

		/* player collision check */
		if(!players[1]->alive)
		{
			int xInc=0,yInc=0;
			switch(players[1]->dir)
			{
				case PlayerDirections::Left:
					xInc = -1; break;
				case PlayerDirections::Right:
					xInc = 1; break;
				case PlayerDirections::Up:
					yInc = -1; break;
				case PlayerDirections::Down:
					yInc = 1; break;
				default:
					break;
			}
			if ((players[1]->getX() + xInc) == players[0]->getX())
			{
				if ((players[1]->getY() + yInc) == players[0]->getY())
				{
					players[0]->alive = false;
				}
			}
		}

		updatePixmap();
		update();

		if(!players[0]->alive && !players[1]->alive)
		{
			stopGame();
			players[0]->score++;
			players[1]->score++;
			showWinner(KTronEnum::Both);
		}
		else
		{
			for (i = 0; i < 2; ++i)
			{
				// crashtests
				if(!players[i]->alive)
				{
					stopGame();
					showWinner((i==0)? KTronEnum::Two : KTronEnum::One);
					players[(1 - i)]->score++;
				}
			}
		}
	}

	if(gameEnded)
	{
		//this is for waiting 1s before starting next game
		gameBlocked=true;
		QTimer::singleShot(1000,this,SLOT(unblockGame()));
	}
}

/* *************************************************************** **
**                 algoritm for the computerplayer                 **
** *************************************************************** */

void Tron::think(int playerNr)
{
	intelligence.think(playerNr);
}

/**
 * Skill settings
 */

/** retrieves the line speed */
int Tron::lineSpeed() {
	KGameDifficulty::standardLevel level = KGameDifficulty::level();
	Settings::setDifficulty((int) level);

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

