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
// Background
#include <kio/netaccess.h>
#include <KMessageBox>

// Normal class
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
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
#include <KAction>
#include <KActionCollection>
#include <KStandardAction>
#include <KGameDifficulty>

#include "tron.h"
#include "settings.h"
#include "renderer.h"

/**
 * init-functions
 **/

Tron::Tron(QWidget *parent)
  : QWidget(parent)
{
  //pixmap=0;
  //playfield = new QVector< QVector<int> >();
  beginHint=false;
  lookForward=15;

  random.setSeed(0);

  setFocusPolicy(Qt::StrongFocus);

  gameBlocked=false;
  //rectSize=10;
  //blockWidth = width() / TRON_PLAYFIELD_WIDTH;
  //blockHeight = height() / TRON_PLAYFIELD_HEIGHT;

  timer = new QTimer(this);
  loadSettings();
  connect(timer, SIGNAL(timeout()), SLOT(doMove()));
  QTimer::singleShot(15000, this,SLOT(showBeginHint()));
}

void Tron::loadSettings(){
  //setPalette(Qt::green);

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
    setComputerplayer(KTronEnum::One, true);
    setComputerplayer(KTronEnum::Two, false);
  }
}

Tron::~Tron()
{
  //delete []  playfield;
  //delete pixmap;
  delete timer;

}

void Tron::resizeRenderer()
{
  // field size
  fieldWidth=TRON_PLAYFIELD_WIDTH;
  fieldHeight=TRON_PLAYFIELD_HEIGHT;

  // Block size
  blockWidth = width() / TRON_PLAYFIELD_WIDTH;
  blockHeight = height() / TRON_PLAYFIELD_HEIGHT;
  if (blockWidth > blockHeight)
  {
    blockWidth = blockHeight;
  }
  else
  {
    blockHeight = blockWidth;
  }

  Renderer::self()->boardResized(width(), height(), 0, 0, blockWidth, blockHeight);

  Renderer::self()->resetPlayField();
}

void Tron::createNewPlayfield()
{
  resizeRenderer();

  // start positions
  //playfield=new QVector<int>[fieldWidth];
  playfield.resize(fieldWidth);
  for(int i=0;i<fieldWidth;i++)
    playfield[i].resize(fieldHeight);

  //int min=(fieldWidth<fieldHeight) ? fieldWidth : fieldHeight;
  //lookForward=min/4;
}

void Tron::newGame()
{
  players[0].score=0;
  players[1].score=0;
  emit gameEnds(KTronEnum::Nobody);
  reset();

  QTimer::singleShot(15000,this,SLOT(showBeginHint()));
}

void Tron::reset()
{
	gamePaused = false;
	stopGame();

	players[0].reset();
	players[1].reset();

	// If playfield exists, then clean it
	// ans set start coordinates
	//if(playfield)
	//{
		int i;
		for(i = 0; i < fieldWidth; i++)
			playfield[i].fill(KTronEnum::BACKGROUND);

		// set start coordinates

		players[0].setCoordinates(fieldWidth/3, fieldHeight/2);
		players[1].setCoordinates(2*fieldWidth/3, fieldHeight/2);

		playfield[players[0].xCoordinate][players[0].yCoordinate] = KTronEnum::PLAYER1 | KTronEnum::TOP | KTronEnum::LEFT | KTronEnum::RIGHT | KTronEnum::HEAD;
		playfield[players[0].xCoordinate][players[0].yCoordinate + 1] = KTronEnum::PLAYER1 | KTronEnum::BOTTOM | KTronEnum::LEFT | KTronEnum::RIGHT;
		playfield[players[1].xCoordinate][players[1].yCoordinate] = KTronEnum::PLAYER2 | KTronEnum::TOP | KTronEnum::LEFT | KTronEnum::RIGHT | KTronEnum::HEAD;
		playfield[players[1].xCoordinate][players[1].yCoordinate + 1] = KTronEnum::PLAYER2 | KTronEnum::BOTTOM | KTronEnum::LEFT | KTronEnum::RIGHT;

		updatePixmap();
		update();
	//}

	setFocus();

	emit gameReset();
}

void Tron::computerStart()
{
	if(isComputer(KTronEnum::Both))
	{
		reset();
		startGame();
	}
}

/* *************************************************************** **
**                   	??? functions										 **
** *************************************************************** */

void Tron::startGame()
{
	gameEnded = false;
	beginHint = false;
	timer->start(velocity);
}

void Tron::stopGame()
{
	timer->stop();
	gameEnded = true;
	players[0].dir = Directions::Up;
	players[1].dir = Directions::Up;
	players[0].last_dir = Directions::Up;
	players[1].last_dir = Directions::Up;
}

void Tron::togglePause() // pause or continue game
{
  if(!gameEnded)
  {
     if(gamePaused)
     {
        gamePaused=false;
        update();
        timer->start(velocity);
     }
     else
     {
        gamePaused=true;
        timer->stop();
        update();
     }
  }
}

void Tron::showWinner(KTronEnum::Player player)
{
   update();

   emit gameEnds(player);

   if(isComputer(KTronEnum::Both))
   {
       QTimer::singleShot(1000,this,SLOT(computerStart()));
   }
}

/* *************************************************************** **
**                    paint functions										 **
** *************************************************************** */

void Tron::updatePixmap()
{
	Renderer::self()->updatePlayField(playfield);
}

/* *************************************************************** **
**                    config functions										 **
** *************************************************************** */

void Tron::setActionCollection(KActionCollection *a)
{
   actionCollection = a;
}

void Tron::setVelocity(int newVel)            // set new velocity
{
  velocity=(10-newVel)*15;

  if(!gameEnded && !gamePaused)
    timer->start(velocity);
}

void Tron::setComputerplayer(KTronEnum::Player player, bool flag) {
  if(player==KTronEnum::One)
    players[0].setComputer(flag);
  else if(player==KTronEnum::Two)
    players[1].setComputer(flag);

  if(isComputer(KTronEnum::Both))
      QTimer::singleShot(1000,this,SLOT(computerStart()));
}

bool Tron::isComputer(KTronEnum::Player player)
{
   if(player==KTronEnum::One)
     return players[0].computer;
   else if(player==KTronEnum::Two)
     return players[1].computer;
   else if(player==KTronEnum::Both)
   {
      if(players[0].computer && players[1].computer)
        return true;
   }

   return false;
}

/* *************************************************************** **
**                    moving functions										 **
** *************************************************************** */

bool Tron::crashed(int playerNr,int xInc, int yInc) const
{
  bool flag;
  int newX=players[playerNr].xCoordinate+xInc;
  int newY=players[playerNr].yCoordinate+yInc;

  if(newX<0 || newY <0 || newX>=fieldWidth || newY>=fieldHeight)
     flag=true;
  else if(playfield[newX][newY] != KTronEnum::BACKGROUND)
    flag=true;
  else flag=false;

  return flag;
}

void Tron::switchDir(int playerNr, Directions::Direction newDirection)
{
  if(playerNr !=0 && playerNr != 1)
  {
     Q_ASSERT_X(true, "Tron::switchDir", "wrong playerNr");
     return;
  }

    if (newDirection==Directions::Up && players[playerNr].last_dir==Directions::Down)
      return;
    if (newDirection==Directions::Down && players[playerNr].last_dir==Directions::Up)
      return;
    if (newDirection==Directions::Left && players[playerNr].last_dir==Directions::Right)
      return;
    if (newDirection==Directions::Right && players[playerNr].last_dir==Directions::Left)
      return;

  players[playerNr].dir=newDirection;
}

void Tron::updateDirections(int playerNr)
{
	if (playerNr == -1)
	{
		updateDirections(0);
		updateDirections(1);
	}
	else if (playerNr == 0 || playerNr == 1)
	{
		int x = players[playerNr].xCoordinate;
		int y = players[playerNr].yCoordinate;

		// necessary for drawing the 3d-line
		switch(players[playerNr].dir)
		{
			// unset drawing flags in the moving direction
			case Directions::Up:
			{
				playfield[x][y] &= (~KTronEnum::TOP);
				break;
			}
			case Directions::Down:
				playfield[x][y] &= (~KTronEnum::BOTTOM);
				break;
			case Directions::Right:
				playfield[x][y] &= (~KTronEnum::RIGHT);
				break;
			case Directions::Left:
				playfield[x][y] &= (~KTronEnum::LEFT);
				break;
			default:
				break;
		}
		playfield[x][y] &= (~KTronEnum::HEAD);

		players[playerNr].last_dir = players[playerNr].dir;
	}

	update();
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
		QFontMetrics fm = p.fontMetrics();
		int w = fm.width(message);
		p.drawText(width()/2-w/2,height()/2,message);
	}
	else if(gameEnded) // If game ended, print "Crash!"
	{
		QString message = i18n("Crash!");
		int w = p.fontMetrics().width(message);
		int h = p.fontMetrics().height();
		for (int i=0;i<2;i++)
		{
			if (!players[i].alive)
			{
				int x = players[i].xCoordinate*blockWidth;
				int y = players[i].yCoordinate*blockHeight;
				while(x<0) x+=blockWidth;
				while(x+w>width()) x-=blockWidth;
				while(y-h<0) y+=blockHeight;
				while(y>height()) y-=blockHeight;
				p.drawText(x,y,message);
			}
		}

		// draw begin hint
		if(beginHint)
		{
			QString hint=i18n("Press any of your direction keys to start!");
			int x=p.fontMetrics().width(hint);
			x=(width()-x)/2;
			int y=height()/2;

			p.drawText(x,y,hint);
		}
	}
}

void Tron::resizeEvent(QResizeEvent *)
{
	//createNewPlayfield();
	//reset();

	resizeRenderer();
	updatePixmap();
	update();
}

void Tron::keyPressEvent(QKeyEvent *e)
{
	if (!players[1].computer)
	{
		if(actionCollection->action("Pl2Up")->shortcuts().contains(e->key()))
		{
			switchDir(1,Directions::Up);
			players[1].keyPressed=true;
		}
		else if(actionCollection->action("Pl2Left")->shortcuts().contains(e->key()))
		{
			switchDir(1,Directions::Left);
			players[1].keyPressed=true;
		}
		else if(actionCollection->action("Pl2Right")->shortcuts().contains(e->key()))
		{
			switchDir(1,Directions::Right);
			players[1].keyPressed=true;
		}
		else if(actionCollection->action("Pl2Down")->shortcuts().contains(e->key()))
		{
			switchDir(1,Directions::Down);
			players[1].keyPressed=true;
		}
		else if(actionCollection->action("Pl2Ac")->shortcuts().contains(e->key()))
		{
			if(!Settings::acceleratorBlocked())
				players[1].accelerated=true;
		}
	}

	if (!players[0].computer)
	{
		if(actionCollection->action("Pl1Left")->shortcuts().contains(e->key()))
		{
			switchDir(0,Directions::Left);
			players[0].keyPressed=true;
		}
		else if(actionCollection->action("Pl1Right")->shortcuts().contains(e->key()))
		{
			switchDir(0,Directions::Right);
			players[0].keyPressed=true;
		}
		else if(actionCollection->action("Pl1Up")->shortcuts().contains(e->key()))
		{
			switchDir(0,Directions::Up);
			players[0].keyPressed=true;
		}
		else if(actionCollection->action("Pl1Down")->shortcuts().contains(e->key()))
		{
			switchDir(0,Directions::Down);
			players[0].keyPressed=true;
		}
		else if(actionCollection->action("Pl1Ac")->shortcuts().contains(e->key()))
		{
			if(!Settings::acceleratorBlocked())
				players[0].accelerated=true;
		}
	}

	e->ignore();  // if key is unknown: ignore

	// if both players press keys at the same time, start game...
	if(gameEnded && !gameBlocked)
	{
		if(players[0].keyPressed && players[1].keyPressed)
		{
			reset();
			startGame();
		}
	}
	// ...or continue
	else if(gamePaused)
	{
		if(players[0].keyPressed && players[1].keyPressed)
		{
		togglePause();
		}
	}
}

void Tron::keyReleaseEvent(QKeyEvent * e)
{
  if(!players[1].computer)
  {
      if(actionCollection->action("Pl2Ac")->shortcuts().contains(e->key()))
		{
			players[1].accelerated=false;
			return;
  		}
      else if(actionCollection->action("Pl2Left")->shortcuts().contains(e->key()))
		{
			players[1].keyPressed=false;
			return;
		}
      else if(actionCollection->action("Pl2Right")->shortcuts().contains(e->key()))
		{
	  		players[1].keyPressed=false;
	  		return;
		}
      else if(actionCollection->action("Pl2Up")->shortcuts().contains(e->key()))
		{
	  		players[1].keyPressed=false;
	  		return;
		}
      else if(actionCollection->action("Pl2Down")->shortcuts().contains(e->key()))
		{
		  players[1].keyPressed=false;
		  return;
		}
  }

   if(!players[0].computer)
   {
      if(actionCollection->action("Pl1Left")->shortcuts().contains(e->key()))
		{
	 		players[0].keyPressed=false;
	 		return;
		}
      else if(actionCollection->action("Pl1Right")->shortcuts().contains(e->key()))
		{
	  		players[0].keyPressed=false;
	  		return;
		}
      else if(actionCollection->action("Pl1Up")->shortcuts().contains(e->key()))
		{
	 		players[0].keyPressed=false;
	  		return;
		}
      else if(actionCollection->action("Pl1Down")->shortcuts().contains(e->key()))
		{
	 		players[0].keyPressed=false;
	 		return;
		}
      else if(actionCollection->action("Pl1Ac")->shortcuts().contains(e->key()))
		{
	  		players[0].accelerated=false;
	  		return;
		}
 	}

   e->ignore();  // if pressed key is unknown, ignore it

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

void Tron::showBeginHint()
{
   if(gameEnded)
   {
      // show only at the beginning of a game
      if(players[0].score==0 && players[1].score==0)
      {
         beginHint=true;
         update();
      }
   }
}

// doMove() is called from QTimer
void Tron::doMove()
{
	int i;
	for(i=0;i<2;i++)
	{
		// Decide if the accelerator key was pressed...
		if(players[i].accelerated)
		{
			updateDirections(i);

			int newType; // determine type of rect to set
			if(i==0)
			{
				newType = (KTronEnum::PLAYER1 | KTronEnum::HEAD);
			}
			else
			{
				newType = (KTronEnum::PLAYER2 | KTronEnum::HEAD);
			}
			switch(players[i].dir)
			{
				case Directions::Up:
					if(crashed(i,0,-1))
						players[i].alive=false;
					else
					{
						players[i].yCoordinate--;
						newType|=(KTronEnum::TOP | KTronEnum::LEFT | KTronEnum::RIGHT);
					}
					break;
				case Directions::Down:
					if(crashed(i,0,1))
						players[i].alive=false;
					else
					{
						players[i].yCoordinate++;
						newType |= (KTronEnum::BOTTOM | KTronEnum::LEFT | KTronEnum::RIGHT);
					}
					break;
				case Directions::Left:
					if(crashed(i,-1,0))
						players[i].alive=false;
					else
					{
						players[i].xCoordinate--;
						newType |= (KTronEnum::LEFT | KTronEnum::TOP | KTronEnum::BOTTOM);
					}
					break;
				case Directions::Right:
					if(crashed(i,1,0))
						players[i].alive=false;
					else
					{
						players[i].xCoordinate++;
						newType |= (KTronEnum::RIGHT | KTronEnum::TOP | KTronEnum::BOTTOM);
					}
					break;
				default:
					break;
			}
			if(players[i].alive)
			{
				playfield[players[i].xCoordinate][players[i].yCoordinate]=newType;
			}
			else
			{
				switch (players[i].last_dir)
				{
					case Directions::Up:
						playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::TOP;
						break;
					case Directions::Down:
						playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::BOTTOM;
						break;
					case Directions::Right:
						playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::RIGHT;
						break;
					case Directions::Left:
						playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::LEFT;
						break;
					default:
						break;
				}
				playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::HEAD;
			}
		}
	}

	if(players[0].accelerated || players[1].accelerated)
	{
		/* player collision check */
		if(!players[1].alive)
		{
			int xInc=0,yInc=0;
			switch(players[1].dir)
			{
				case Directions::Left:
				xInc = -1;
				break;
				case Directions::Right:
				xInc = 1;
				break;
				case Directions::Up:
				yInc = -1;
				break;
				case Directions::Down:
				yInc = 1;
				break;
				default:
				break;
			}
			if ((players[1].xCoordinate+xInc) == players[0].xCoordinate)
			if ((players[1].yCoordinate+yInc) == players[0].yCoordinate)
			{
				players[0].alive=false;
			}
		}

		updatePixmap();
		update();

		// crashtest
		if(!players[0].alive && !players[1].alive)
		{
			stopGame();
			players[0].score++;
			players[1].score++;
			showWinner(KTronEnum::Both);
		}
		else
		{
			for(i=0;i<2;i++)
			{
				if(!players[i].alive)
				{
				stopGame();
				showWinner((i==0)? KTronEnum::Two : KTronEnum::One);
				players[i].score++;
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
	for(i=0;i<2;i++)
	{
		if(players[i].computer)
			think(i);
	}

	updateDirections();

	for(i=0;i<2;i++)
	{
		int newType;
		if(i==0)
			newType = (KTronEnum::PLAYER1 | KTronEnum::HEAD);
		else
			newType = (KTronEnum::PLAYER2 | KTronEnum::HEAD);

		switch(players[i].dir)
		{
			case Directions::Up:
				if(crashed(i,0,-1))
				players[i].alive=false;
				else
				{
				players[i].yCoordinate--;
				newType |= (KTronEnum::TOP | KTronEnum::RIGHT | KTronEnum::LEFT);
				}
			break;
			case Directions::Down:
				if(crashed(i,0,1))
				players[i].alive=false;
				else
				{
				players[i].yCoordinate++;
				newType |= (KTronEnum::BOTTOM | KTronEnum::RIGHT | KTronEnum::LEFT);
				}
			break;
			case Directions::Left:
				if(crashed(i,-1,0))
				players[i].alive=false;
				else
				{
				players[i].xCoordinate--;
				newType |= (KTronEnum::LEFT | KTronEnum::TOP | KTronEnum::BOTTOM);
				}
			break;
			case Directions::Right:
				if(crashed(i,1,0))
				players[i].alive=false;
				else
				{
				players[i].xCoordinate++;
				newType |= (KTronEnum::RIGHT | KTronEnum::TOP | KTronEnum::BOTTOM);
				}
			break;
			default:
			break;

		}
		if(players[i].alive)
		{
			playfield[players[i].xCoordinate][players[i].yCoordinate]=newType;
		}
		else
		{
			switch (players[i].last_dir)
			{
				case Directions::Up:
					playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::TOP;
					break;
				case Directions::Down:
					playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::BOTTOM;
					break;
				case Directions::Right:
					playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::RIGHT;
					break;
				case Directions::Left:
					playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::LEFT;
					break;
				default:
					break;
			}
			playfield[players[i].xCoordinate][players[i].yCoordinate] |= KTronEnum::HEAD;
		}
	}

	/* player collision check */
	if(!players[1].alive)
	{
		int xInc=0,yInc=0;
		switch(players[1].dir)
		{
			case Directions::Left:
				xInc = -1; break;
			case Directions::Right:
				xInc = 1; break;
			case Directions::Up:
				yInc = -1; break;
			case Directions::Down:
				yInc = 1; break;
			default:
				break;
		}
		if ((players[1].xCoordinate+xInc) == players[0].xCoordinate)
			if ((players[1].yCoordinate+yInc) == players[0].yCoordinate)
			{
				players[0].alive=false;
			}
	}

	updatePixmap();
	update();

	if(!players[0].alive && !players[1].alive)
	{
		stopGame();
		players[0].score++;
		players[1].score++;
		showWinner(KTronEnum::Both);
	}
	else
		for(i=0;i<2;i++)
		{
			// crashtests
			if(!players[i].alive)
			{
				stopGame();
				showWinner((i==0)? KTronEnum::Two : KTronEnum::One);
				players[i].score++;
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

// This part is partly ported from
// xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>
void Tron::think(int playerNr)
{
if(opponentSkill() != 1)
{
  int opponent=(playerNr==1)? 0 : 1;

  // determines left and right side
  Directions::Direction sides[2];
  // increments for moving to the different sides
  int flags[6]={0,0,0,0,0,0};
  int index[2];
  // distances to barrier
  int dis_forward,  dis_left, dis_right;

  dis_forward = dis_left = dis_right = 1;


  	switch (players[playerNr].dir)
   	{
  			case Directions::Left:
    			//forward flags
    			flags[0] = -1;
    			flags[1] = 0;

    			//left flags
    			flags[2] = 0;
    			flags[3] = 1;

    			// right flags
    			flags[4] = 0;
    			flags[5] = -1;

    			//turns to either side
    			sides[0] = Directions::Down;
    			sides[1] = Directions::Up;
    			break;
  			case Directions::Right:
    			flags[0] = 1;
    			flags[1] = 0;
    			flags[2] = 0;
			   flags[3] = -1;
    			flags[4] = 0;
    			flags[5] = 1;
    			sides[0] = Directions::Up;
    			sides[1] = Directions::Down;
    			break;
  			case Directions::Up:
    			flags[0] = 0;
    			flags[1] = -1;
    			flags[2] = -1;
    			flags[3] = 0;
    			flags[4] = 1;
    			flags[5] = 0;
    			sides[0] = Directions::Left;
    			sides[1] = Directions::Right;
    			break;
  			case Directions::Down:
    			flags[0] = 0;
    			flags[1] = 1;
    			flags[2] = 1;
    			flags[3] = 0;
    			flags[4] = -1;
    			flags[5] = 0;
    			sides[0] = Directions::Right;
    			sides[1] = Directions::Left;
    			break;
			default:
			break;

  		}

  		// check forward
  		index[0] = players[playerNr].xCoordinate+flags[0];
  		index[1] = players[playerNr].yCoordinate+flags[1];
  		while (index[0] < fieldWidth && index[0] >= 0 &&
	 		index[1] < fieldHeight && index[1] >= 0 &&
	 		playfield[index[0]][index[1]] == KTronEnum::BACKGROUND)
	 	{
    		dis_forward++;
    		index[0] += flags[0];
    		index[1] += flags[1];
  		}

    	// check left
    	index[0] = players[playerNr].xCoordinate+flags[2];
    	index[1] = players[playerNr].yCoordinate+flags[3];
    while (index[0] < fieldWidth && index[0] >= 0 &&
	   index[1] < fieldHeight && index[1] >= 0 &&
	   playfield[index[0]][index[1]] == KTronEnum::BACKGROUND) {
      dis_left++;
      index[0] += flags[2];
      index[1] += flags[3];
    }

    // check right
    index[0] = players[playerNr].xCoordinate+flags[4];
    index[1] = players[playerNr].yCoordinate+flags[5];
    while (index[0] < fieldWidth && index[0] >= 0 &&
	   index[1] <  fieldHeight && index[1] >= 0 &&
	   playfield[index[0]][index[1]] == KTronEnum::BACKGROUND) {
      dis_right++;
      index[0] += flags[4];
      index[1] += flags[5];
    }

  	// distances to opponent
  	int hor_dis=0; // negative is opponent to the right
  	int vert_dis=0; // negative is opponent to the bottom
  	hor_dis=players[playerNr].xCoordinate-players[opponent].xCoordinate;
  	vert_dis=players[playerNr].yCoordinate-players[opponent].yCoordinate;

  	int opForwardDis=0; // negative is to the back
  	int opSideDis=0;  // negative is to the left
  	bool opMovesOppositeDir=false;
  	bool opMovesSameDir=false;
  	bool opMovesRight=false;
  	bool opMovesLeft=false;

  	switch(players[playerNr].dir)
  	{
  	   case Directions::Up:
  	      opForwardDis=vert_dis;
  	      opSideDis=-hor_dis;
  	      if(players[opponent].dir==Directions::Down)
  	         opMovesOppositeDir=true;
  	      else if(players[opponent].dir==Directions::Up)
  	         opMovesSameDir=true;
  	      else if(players[opponent].dir==Directions::Left)
  	         opMovesLeft=true;
  	      else if(players[opponent].dir==Directions::Right)
  	         opMovesRight=true;
  	      break;
  	   case Directions::Down:
  	      opForwardDis=-vert_dis;
  	      opSideDis=hor_dis;
  	      if(players[opponent].dir==Directions::Up)
  	         opMovesOppositeDir=true;
  	      else if(players[opponent].dir==Directions::Down)
  	         opMovesSameDir=true;
  	      else if(players[opponent].dir==Directions::Left)
  	         opMovesRight=true;
  	      else if(players[opponent].dir==Directions::Right)
  	         opMovesLeft=true;
  	      break;
  	   case Directions::Left:
  	      opForwardDis=hor_dis;
  	      opSideDis=vert_dis;
  	      if(players[opponent].dir==Directions::Right)
  	         opMovesOppositeDir=true;
  	      else if(players[opponent].dir==Directions::Left)
  	         opMovesSameDir=true;
  	      else if(players[opponent].dir==Directions::Down)
  	         opMovesLeft=true;
  	      else if(players[opponent].dir==Directions::Up)
  	         opMovesRight=true;
  	      break;
  	   case Directions::Right:
  	      opForwardDis=-hor_dis;
  	      opSideDis=-vert_dis;
  	      if(players[opponent].dir==Directions::Left)
  	         opMovesOppositeDir=true;
  	      else if(players[opponent].dir==Directions::Right)
  	         opMovesSameDir=true;
  	      else if(players[opponent].dir==Directions::Up)
  	         opMovesLeft=true;
  	      else if(players[opponent].dir==Directions::Down)
  	         opMovesRight=true;
  	      break;
  	    default:
  	      break;

  	}

  	int doPercentage = 100;
  	switch(opponentSkill())
  	{
  	  case 1:
  	        // Never reached
  		break;

  	  case 2:
  		doPercentage=5;
  		break;
  		
  	  case 3:
  		doPercentage=90;
  		break;
  	}

  	// if opponent moves the opposite direction as we
   if(opMovesOppositeDir)
   {
      // if opponent is in front
      if(opForwardDis>0)
      {
         // opponent is to the right and we have the chance to block the way
         if(opSideDis>0 && opSideDis < opForwardDis && opSideDis < dis_right && opForwardDis < lookForward)
         {
            if ((int)random.getLong(100) <= doPercentage || dis_forward==1)
               switchDir(playerNr,sides[1]); // turn right
         }
         // opponent is to the left and we have the chance to block the way
         else if(opSideDis<0 && -opSideDis < opForwardDis && -opSideDis < dis_left && opForwardDis < lookForward)
         {
            if ((int)random.getLong(100) <= doPercentage || dis_forward==1)
               switchDir(playerNr,sides[0]); // turn left
         }
         // if we can do nothing, go forward
         else if(dis_forward < lookForward)
         {
      		dis_forward = 100 - 100/dis_forward;

    			if(!(dis_left == 1 && dis_right == 1))
      			if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
         			changeDirection(playerNr,dis_right,dis_left);
         }
      }
      // opponent is in back of us and moves away: do nothing
      else if(dis_forward < lookForward)
      {
      	dis_forward = 100 - 100/dis_forward;

    		if(!(dis_left == 1 && dis_right == 1))
      		if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        			changeDirection(playerNr,dis_right,dis_left);
      }
   } // end  if(opMovesOppositeDir)

   else if(opMovesSameDir)
   {
      // if opponent is to the back
      if(opForwardDis < 0)
      {
        	// opponent is to the right and we have the chance to block the way
         if(opSideDis>0 && opSideDis < -opForwardDis && opSideDis < dis_right)
         {
            if ((int)random.getLong(100) <= doPercentage || dis_forward==1)
               switchDir(playerNr,sides[1]); // turn right
         }
         // opponent is to the left and we have the chance to block the way
         else if(opSideDis<0 && -opSideDis < -opForwardDis && -opSideDis < dis_left)
         {
            if ((int)random.getLong(100) <= doPercentage || dis_forward==1)
               switchDir(playerNr,sides[0]); // turn left
         }
         // if we can do nothing, go forward
         else if(dis_forward < lookForward)
         {
      		dis_forward = 100 - 100/dis_forward;

    			if(!(dis_left == 1 && dis_right == 1))
      			if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
         			changeDirection(playerNr,dis_right,dis_left);
         }
      }
      // opponent is in front of us and moves away
      else if(dis_forward < lookForward)
      {
      	dis_forward = 100 - 100/dis_forward;

    		if(!(dis_left == 1 && dis_right == 1))
      		if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        			changeDirection(playerNr,dis_right,dis_left);
      }
   } // end if(opMovesSameDir)

   else if(opMovesRight)
   {
      // opponent is in front of us
      if(opForwardDis>0)
      {
          // opponent is to the left
          if(opSideDis < 0 && -opSideDis < opForwardDis && -opSideDis < dis_left)
          {
             if(opForwardDis < lookForward && dis_left > lookForward)
             {
	          	 if ((int)random.getLong(100) <= doPercentage/2 || dis_forward==1)
               	changeDirection(playerNr,dis_right,dis_left);
	          }
	          else if(dis_forward < lookForward)
      		 {
      			dis_forward = 100 - 100/dis_forward;

    				if(!(dis_left == 1 && dis_right == 1))
      				if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        					changeDirection(playerNr,dis_right,dis_left);
      		 }
          }
          // op is to the right and moves away, but maybe we can block him
          else if(opSideDis>=0 && opSideDis < dis_right)
          {
             if(opForwardDis < lookForward && dis_right>lookForward)
             {
            	if ((int)random.getLong(100) <= doPercentage/2 || dis_forward==1)
               	switchDir(playerNr,sides[1]); // turn right
             }
	          else if(dis_forward < lookForward)
      		 {
      			dis_forward = 100 - 100/dis_forward;

    				if(!(dis_left == 1 && dis_right == 1))
      				if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        					changeDirection(playerNr,dis_right,dis_left);
      		 }
          }
          else if(dis_forward < lookForward)
      	 {
      		dis_forward = 100 - 100/dis_forward;

    			if(!(dis_left == 1 && dis_right == 1))
      			if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        				changeDirection(playerNr,dis_right,dis_left);
      	 }
      }
      // opponent is in the back of us
      else
      {
         // opponent is right from us and we already blocked him
         if(opSideDis>0 && opForwardDis < lookForward && opSideDis < dis_right)
         {
            if ((int)random.getLong(100) <= doPercentage/2 || dis_forward==1)
               	changeDirection(playerNr,dis_right,dis_left);
         }
      	else if(dis_forward<lookForward)
      	{
      		dis_forward = 100 - 100/dis_forward;

    			if(!(dis_left == 1 && dis_right == 1))
      			if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        				changeDirection(playerNr,dis_right,dis_left);
      	}
      }
   } // end if(opMovesRight)

   else if(opMovesLeft)
   {
      // opponent is in front of us
      if(opForwardDis>0)
      {
          // opponent is to the right, moves towards us and could block us
          if(opSideDis > 0 && opSideDis < opForwardDis && opSideDis < dis_right)
          {
             if(opForwardDis < lookForward && dis_right>lookForward)
             {
          	 	if ((int)random.getLong(100) <= doPercentage/2 || dis_forward==1)
               	changeDirection(playerNr,dis_right,dis_left);
             }
          	 else if(dis_forward < lookForward)
      		 {
      			dis_forward = 100 - 100/dis_forward;

    				if(!(dis_left == 1 && dis_right == 1))
      				if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        					changeDirection(playerNr,dis_right,dis_left);
      		 }
          }
          // op is to the left and moves away, but maybe we can block him
          else if(opSideDis<=0 && opSideDis < dis_left)
          {
             if(opForwardDis < lookForward && dis_left>lookForward)
             {
             	if ((int)random.getLong(100) <= doPercentage/2 || dis_forward==1)
						switchDir(playerNr,sides[0]); // turn left
				 }
	       	 else if(dis_forward < lookForward)
      		 {
      			dis_forward = 100 - 100/dis_forward;

    				if(!(dis_left == 1 && dis_right == 1))
      				if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        					changeDirection(playerNr,dis_right,dis_left);
      		 }

          }
      	else if(dis_forward < lookForward)
      	{
      		dis_forward = 100 - 100/dis_forward;

    			if(!(dis_left == 1 && dis_right == 1))
      			if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        				changeDirection(playerNr,dis_right,dis_left);
      	}
      }
      // opponent is in the back of us
      else //if(opForwardDis<=0)
      {
         // opponent is left from us and we already blocked him
         if(opSideDis<0 && opForwardDis < lookForward && -opSideDis < dis_left)
         {
            if ((int)random.getLong(100) <= doPercentage/2 || dis_forward==1)
               	changeDirection(playerNr,dis_right,dis_left);
         }
      	else if(dis_forward<lookForward)
      	{
      		dis_forward = 100 - 100/dis_forward;

    			if(!(dis_left == 1 && dis_right == 1))
      			if ((int)random.getLong(100) >= dis_forward || dis_forward == 1)
        				changeDirection(playerNr,dis_right,dis_left);
      	}
      }
   } // end if(opMovesLeft)

}
// This part is completely ported from
// xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>
else // Settings::skill() == Settings::EnumSkill::Easy
{
  Directions::Direction sides[2];
  int flags[6] = {0,0,0,0,0,0};
  int index[2];
  int dis_forward,  dis_left, dis_right;

  dis_forward = dis_left = dis_right = 1;

  switch (players[playerNr].dir) {
  case Directions::Left:

    //forward flags
    flags[0] = -1;
    flags[1] = 0;

    //left flags
    flags[2] = 0;
    flags[3] = 1;

    // right flags
    flags[4] = 0;
    flags[5] = -1;

    //turns to either side
    sides[0] = Directions::Down;
    sides[1] = Directions::Up;
    break;
  case Directions::Right:
    flags[0] = 1;
    flags[1] = 0;
    flags[2] = 0;
    flags[3] = -1;
    flags[4] = 0;
    flags[5] = 1;
    sides[0] = Directions::Up;
    sides[1] = Directions::Down;
    break;
  case Directions::Up:
    flags[0] = 0;
    flags[1] = -1;
    flags[2] = -1;
    flags[3] = 0;
    flags[4] = 1;
    flags[5] = 0;
    sides[0] = Directions::Left;
    sides[1] = Directions::Right;
    break;
  case Directions::Down:
    flags[0] = 0;
    flags[1] = 1;
    flags[2] = 1;
    flags[3] = 0;
    flags[4] = -1;
    flags[5] = 0;
    sides[0] = Directions::Right;
    sides[1] = Directions::Left;
    break;
  default:
    break;
  }

  // check forward
  index[0] = players[playerNr].xCoordinate+flags[0];
  index[1] = players[playerNr].yCoordinate+flags[1];
  while (index[0] < fieldWidth && index[0] >= 0 &&
	 index[1] < fieldHeight && index[1] >= 0 &&
	 playfield[index[0]][index[1]] == KTronEnum::BACKGROUND) {
    dis_forward++;
    index[0] += flags[0];
    index[1] += flags[1];
  }

  if (dis_forward < lookForward)
    {
      dis_forward = 100 - 100/dis_forward;

    // check left
    index[0] = players[playerNr].xCoordinate+flags[2];
    index[1] = players[playerNr].yCoordinate+flags[3];
    while (index[0] < fieldWidth && index[0] >= 0 &&
	   index[1] < fieldHeight && index[1] >= 0 &&
	   playfield[index[0]][index[1]] == KTronEnum::BACKGROUND) {
      dis_left++;
      index[0] += flags[2];
      index[1] += flags[3];
    }

    // check right
    index[0] = players[playerNr].xCoordinate+flags[4];
    index[1] = players[playerNr].yCoordinate+flags[5];
    while (index[0] < fieldWidth && index[0] >= 0 &&
	   index[1] <  fieldHeight && index[1] >= 0 &&
	   playfield[index[0]][index[1]] == KTronEnum::BACKGROUND) {
      dis_right++;
      index[0] += flags[4];
      index[1] += flags[5];
    }
    if(!(dis_left == 1 && dis_right == 1))
      if ((int)random.getLong(100) >= dis_forward || dis_forward == 0) {

	// change direction
	if ((int)random.getLong(100) <= (100*dis_left)/(dis_left+dis_right))
	  if (dis_left != 1)

	    // turn to the left
	    switchDir(playerNr,sides[0]);
	  else

	    // turn to the right
	    switchDir(playerNr,sides[1]);
	else
	  if (dis_right != 1)

	    // turn to the right
	    switchDir(playerNr,sides[1]);
	  else

	    // turn to the left
	    switchDir(playerNr,sides[0]);
      }
  }
 }
}

void Tron::changeDirection(int playerNr,int dis_right,int dis_left)
{
   Directions::Direction currentDir=players[playerNr].dir;
   Directions::Direction sides[2];
   switch (currentDir)
   {
  		case Directions::Left:
    		//turns to either side
    		sides[0] = Directions::Down;
    		sides[1] = Directions::Up;
    		break;
  		case Directions::Right:
    		sides[0] = Directions::Up;
    		sides[1] = Directions::Down;
    		break;
  		case Directions::Up:
    		sides[0] = Directions::Left;
    		sides[1] = Directions::Right;
    		break;
  		case Directions::Down:
    		sides[0] = Directions::Right;
    		sides[1] = Directions::Left;
    		break;
		default:
		break;

  	}

   if(!(dis_left == 1 && dis_right == 1))
   {
			// change direction
			if ((int)random.getLong(100) <= (100*dis_left)/(dis_left+dis_right))
			{
	  			if (dis_left != 1)
		    		// turn to the left
		    		switchDir(playerNr,sides[0]);
	  			else
	   	 		// turn to the right
	    			switchDir(playerNr,sides[1]);
	    	}
			else
			{
	  				if (dis_right != 1)
	  					// turn to the right
	    				switchDir(playerNr,sides[1]);
	  				else
	    				// turn to the left
	    				switchDir(playerNr,sides[0]);
          }
    }
}

/**
 * Skill settings
 */

/** retrieves the opponentSkill */
int Tron::opponentSkill() {
	KGameDifficulty::standardLevel level = KGameDifficulty::level();
	Settings::setDifficulty((int) level);

	switch (level) {
		case KGameDifficulty::VeryEasy:
			return 1;
		default:
		case KGameDifficulty::Easy:
			return 1;
		case KGameDifficulty::Medium:
			return 2;
		case KGameDifficulty::Hard:
			return 3;
		case KGameDifficulty::VeryHard:
			return 3;
	}
}

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


#include "tron.moc"

