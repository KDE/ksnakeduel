/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#include "tron.h"

// Normal class
#include <QTimer>
#include <QPainter>
#include <QPoint>

#include "ksnakeduel_debug.h"
#include <KLocalizedString>
// KDEGames
#include <KgDifficulty>
#include <KgThemeProvider>

#include "settings.h"
#include "renderer.h"
#include "object.h"
#include "obstacle.h"

/**
 * init-functions
 **/

Tron::Tron(QWidget *parent) : QWidget(parent)
{
	players[0] = new Player(pf, 0);
	players[1] = new Player(pf, 1);

	connect(players[0], &Player::fetchedItem, this, &Tron::itemHit);
	connect(players[1], &Player::fetchedItem, this, &Tron::itemHit);

	intelligence.referenceTron(this);

	setFocusPolicy(Qt::StrongFocus);

	gameBlocked = false;
	gameEnded = true;

	timer = new QTimer(this);
	//loadSettings();
	connect(timer, &QTimer::timeout, this, &Tron::doMove);
	connect(Renderer::self()->themeProvider(), &KgThemeProvider::currentThemeChanged,
		this, &Tron::resetOnThemeChange);
}

void Tron::loadSettings(){
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
	//Q_EMIT gameEnds(KTronEnum::Nobody);
	Q_EMIT updatedScore();
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
	}

	setVelocity( lineSpeed() );

	modMoves = 0;

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

	Q_EMIT gameReset();
	Q_EMIT updatedScore();
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
		qCDebug(KSNAKEDUEL_LOG) << "Inexistent player requested: " << playerNr;
		return nullptr;
	}

	return players[playerNr];
}


/* *************************************************************** **
**                   	??? functions										 **
** *************************************************************** */

void Tron::startGame()
{
	gameEnded = false;
	Q_EMIT pauseBlocked(false);

	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		newApple();
	}

	timer->start(velocity);
}

void Tron::itemHit(int playerNumber, int, int)
{
	//qCDebug(KSNAKEDUEL_LOG) << "Got Item Hit for " << playerNumber;

	newApple();
	players[playerNumber]->setEnlargement(3);
	players[playerNumber]->addScore(5);
	if (velocity > 15)
	{
		velocity--;
		timer->stop();
		timer->start(velocity);
	}

	Q_EMIT updatedScore();
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

	//qCDebug(KSNAKEDUEL_LOG) << "Drawn apple at (" << x << ", " << y << ")";

	apple.setType((int)(rand() % 3));

	pf.setObjectAt(x, y, apple);
}

void Tron::newObstacle()
{
	// KSnake only
	if (Settings::gameType() != Settings::EnumGameType::Snake)
		return;

	int x = rand() % pf.getWidth();
	int y = rand() % pf.getHeight();

	// Don't render if it's at an unwanted place
	if (pf.getObjectAt(x, y)->getObjectType() != ObjectType::Object)
		return;
	else if (x == players[0]->getX() || y == players[0]->getY())
		return;

	Obstacle obst;
	pf.setObjectAt(x, y, obst);

	// Score +2
	players[0]->addScore(2);
	Q_EMIT updatedScore();
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

			Q_EMIT updatedScore();
		}
		else
		{
			gamePaused = true;
			timer->stop();
			update();

			Q_EMIT updatedScore();
		}
	}
}

void Tron::showWinner()
{
	update();

	Q_EMIT gameEnds();
	Q_EMIT pauseBlocked(true);
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
		QString message = QString();

		if (Settings::gameType() != Settings::EnumGameType::Snake) {
			if (hasWinner())
			{
				int winner = getWinner();
				int loser = 1 - winner;

				QString winnerName = players[winner]->getName();
				QString loserName = players[loser]->getName();
				int winnerScore = players[winner]->getScore();
				int loserScore = players[loser]->getScore();

				message += i18np("%1 has won versus %2 with %4 versus %3 point!", "%1 has won versus %2 with %4 versus %3 points!", winnerName, loserName, loserScore, winnerScore);
				message += QLatin1Char( '\n' );
			}
			else
			{
				QString name1 = players[0]->getName();
				QString name2 = players[1]->getName();
				int points1 = players[0]->getScore();
				int points2 = players[1]->getScore();

				message += i18nc("%2 = 'x points' [player %1], %4 = 'x points' [player %3]",
					"%1 (%2) versus %3 (%4)",
					name2, i18np("%1 point", "%1 points", points2),
					name1, i18np("%1 point", "%1 points", points1));
				message += QLatin1Char( '\n' );
			}
		}
		else
		{
			int points = players[0]->getScore();

			message += i18np("KSnake game ended with 1 point", "KSnake game ended with %1 points", points);
			message += QLatin1Char( '\n' );
		}

		if (Settings::gameType() == Settings::EnumGameType::PlayerVSPlayer) {
			message += i18n("The game starts when each player has pressed one of their direction keys!");
		}
		else {
			message += i18n("Press any of your direction keys to start!");
		}

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

void Tron::resetOnThemeChange()
{
	Renderer::self()->clearPixmapCache();
	updatePixmap();
	update();
}

void Tron::triggerKey(int player, KBAction::Action action, bool trigger)
{
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
	// Set key pressed
	if (!players[player]->isComputer())
	{
		switch (action)
		{
			case KBAction::UP:
			case KBAction::DOWN:
			case KBAction::LEFT:
			case KBAction::RIGHT:
				players[player]->setKeyPressed(true);
				break;
			case KBAction::ACCELERATE:
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
			if (hasWinner())
			{
				newGame();
			}

			reset();
			startGame();
		}
		// ...or continue
		else if (gamePaused)
		{
			togglePause();
		}
	}

	// Key handling for movement
	if (!players[player]->isComputer())
	{
		switch (action)
		{
			case KBAction::UP:
				players[player]->setDirection(PlayerDirections::Up);
				break;
			case KBAction::DOWN:
				players[player]->setDirection(PlayerDirections::Down);
				break;
			case KBAction::LEFT:
				players[player]->setDirection(PlayerDirections::Left);
				break;
			case KBAction::RIGHT:
				players[player]->setDirection(PlayerDirections::Right);
				break;
			case KBAction::ACCELERATE:
				if (!Settings::acceleratorBlocked())
				{
					players[player]->setAccelerated(true);
				}
				break;
			default:
				break;
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

// doMove() is called from QTimer
void Tron::doMove()
{
	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		players[0]->movePlayer();

		modMoves++;

		if (modMoves == 20)
		{
			modMoves = 0;
			newObstacle();
		}

		updatePixmap();
		update();

		if (!players[0]->isAlive())
		{
			stopGame();
			showWinner();
		}
	}
	else
	{
		if (players[0]->isAccelerated() || players[1]->isAccelerated())
		{
			movementHelper(true);
		}

		if (!gameEnded)
		{
			// Player 0 is never a computer nowadays...
			if (players[1]->isComputer())
			{
				intelligence.think(1);
			}

			movementHelper(false);
		}
	}

	if (gameEnded)
	{
		//this is for waiting 1s before starting next game
		gameBlocked = true;
		QTimer::singleShot(1000, this, &Tron::unblockGame);
	}
}

void Tron::movementHelper(bool onlyAcceleratedPlayers)
{
	if (!onlyAcceleratedPlayers || players[0]->isAccelerated())
	{
		players[0]->movePlayer();
	}

	if (!onlyAcceleratedPlayers || players[1]->isAccelerated())
	{
		players[1]->movePlayer();
	}

	/* player collision check */
	if (!players[1]->isAlive())
	{
		checkHeadToHeadCollision();
	}

	updatePixmap();
	update();

	// crashtest
	if (!players[0]->isAlive() || !players[1]->isAlive())
	{
		stopGame();

		if (!players[0]->isAlive() && !players[1]->isAlive())
		{
			// Don't award points when both players die
			//players[0]->addScore(1);
			//players[1]->addScore(1);
		}
		else if (!players[0]->isAlive())
		{
			players[1]->addScore(1);
		}
		else if (!players[1]->isAlive())
		{
			players[0]->addScore(1);
		}

		showWinner();
	}
}

void Tron::checkHeadToHeadCollision()
{
	// As player 1 and player 2 move at the same time
	// a head to head collision is possible
	// but tough movement actually is done sequential
	// we have to check back if player 1 should die when player 2 did so
	// that's where this function comes in :)

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

/**
 * Skill settings
 */

/** retrieves the line speed */
int Tron::lineSpeed() {
	switch (Kg::difficultyLevel()) {
		case KgDifficultyLevel::VeryEasy:
			return 2;
		default:
		case KgDifficultyLevel::Easy:
			return 3;
		case KgDifficultyLevel::Medium:
			return 5;
		case KgDifficultyLevel::Hard:
			return 7;
		case KgDifficultyLevel::VeryHard:
			return 8;
	}
}

bool Tron::running() {
	return !gameEnded;
}

bool Tron::paused() {
	return !gameEnded && gamePaused;
}

bool Tron::hasWinner()
{
	return getWinner() == 0 || getWinner() == 1;
}

int Tron::getWinner()
{
	const unsigned short int WINNING_DIFF = Settings::rounds();
	if (Settings::gameType() != Settings::EnumGameType::Snake)
	{
		if (players[0]->getScore() >= WINNING_DIFF && players[1]->getScore() < players[0]->getScore() - 1) {
			return 0;
		}
		else if (players[1]->getScore() >= WINNING_DIFF && players[0]->getScore() < players[1]->getScore() - 1) {
			return 1;
		}

	}

	return -1;
}



