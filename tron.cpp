/* ********************************************************************************
  This file is part of the game 'KTron'

  Copyright (C) 1998-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>

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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  ***************************************************************************** */

#include <qtimer.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kaccel.h>
#include <klocale.h>
#include <kconfig.h>
#include <kcolordialog.h>

#include "tron.h"

#define FRAMESIZE 2

/* *************************************************************** **
**                    init-functions										 **
** *************************************************************** */

Tron::Tron(QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  pixmap=0;
  playfield=0;
  changeWinnerColor=false;
  blockAccelerator=false;
  beginHint=false;
  lookForward=15;

  random.setSeed(0);

  timer=new QTimer(this,"timer");
  connect(timer,SIGNAL(timeout()),SLOT(doMove()));

  colors[0]=QColor("black");
  colors[1]=QColor("red");
  colors[2]=QColor("blue");
  setPalette(colors[0]);

  setFocusPolicy(QWidget::StrongFocus);
  setBackgroundMode(NoBackground);

  gameBlocked=false;
  style=OLine;
  rectSize=7;
  _skill=Medium;

  QTimer::singleShot(15000,this,SLOT(showBeginHint()));
}

Tron::~Tron()
{
  if(playfield)
    {
      delete []  playfield;
    }
  if(pixmap)
    delete pixmap;
}

void Tron::createNewPlayfield()
{
  if(playfield)
    {
      delete [] playfield;
    }

  if(pixmap)
    delete pixmap;

  // field size

  fieldWidth=(width()-2*FRAMESIZE)/rectSize;
  fieldHeight=(height()-2*FRAMESIZE)/rectSize;

  // start positions
  playfield=new QMemArray<int>[fieldWidth];
  for(int i=0;i<fieldWidth;i++)
    playfield[i].resize(fieldHeight);

  pixmap=new QPixmap(size());
     pixmap->fill(colors[0]);

  //int min=(fieldWidth<fieldHeight) ? fieldWidth : fieldHeight;
  //lookForward=min/4;

}

void Tron::newGame()
{
  players[0].score=0;
  players[1].score=0;
  emit gameEnds(Nobody);
  reset();

  QTimer::singleShot(15000,this,SLOT(showBeginHint()));
}

void Tron::reset()
{
  gamePaused=false;
  stopGame();

  players[0].reset();
  players[1].reset();

  // If playfield exists, then clean it
  // ans set start coordinates
  if(playfield)
  {
      int i;
      for(i=0;i<fieldWidth;i++)
	   	playfield[i].fill(BACKGROUND);

      // set start coordinates

      players[0].setCoordinates(fieldWidth/3, fieldHeight/2);
      players[1].setCoordinates(2*fieldWidth/3, fieldHeight/2);

      playfield[players[0].xCoordinate][players[0].yCoordinate]=
         PLAYER1 | TOP | BOTTOM | LEFT | RIGHT;
      playfield[players[1].xCoordinate][players[1].yCoordinate]=
         PLAYER2 | TOP | BOTTOM | LEFT | RIGHT;

      updatePixmap();
      update();
    }

    setFocus();

    emit gameReset();
}

void Tron::computerStart()
{
   if(isComputer(Both))
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
   gameEnded=false;
   beginHint=false;
   timer->start(velocity);
}

void Tron::stopGame()
{
   timer->stop();
   gameEnded=true;

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

void Tron::showWinner(Player player)
{
   int i,j;

   if(player != Both && changeWinnerColor)
   {
      int winner;
      int looser;
      if(player==One)
      {
         winner=PLAYER1;
         looser=PLAYER2;
      }
      else
      {
         winner=PLAYER2;
         looser=PLAYER1;
      }

      for(i=0;i<fieldWidth;i++)
         for(j=0;j<fieldHeight;j++)
         {
            if(playfield[i][j]!=BACKGROUND)
            {
               // change player
               playfield[i][j] |= winner;
               playfield[i][j] &= ~looser;
            }
         }

      updatePixmap();
   }

   repaint();

   emit gameEnds(player);

   if(isComputer(Both))
   {
       QTimer::singleShot(1000,this,SLOT(computerStart()));
   }
}

/* *************************************************************** **
**                    paint functions										 **
** *************************************************************** */

void Tron::updatePixmap()
{
  int i,j;

  if(!bgPix.isNull())
  {
     int pw=bgPix.width();
     int ph=bgPix.height();
     for (int x = 0; x <= width(); x+=pw)
        for (int y = 0; y <= height(); y+=ph)
	    bitBlt(pixmap, x, y, &bgPix);
  }
  else
  {
    pixmap->fill(colors[0]);
  }

  QPainter p;
  p.begin(pixmap);

  // alle Pixel prüfen und evt. zeichnen
  for(i=0;i<fieldWidth;i++)
     for(j=0;j<fieldHeight;j++)
     {
        if(playfield[i][j]!=BACKGROUND)
        {
           drawRect(p,i,j);
	}
     }

   // draw frame
   QColor light=parentWidget()->colorGroup().midlight();
   QColor dark=parentWidget()->colorGroup().mid();

   p.setPen(NoPen);
   p.setBrush(light);
      p.drawRect(width()-FRAMESIZE,0,FRAMESIZE,height());
   p.drawRect(0,height()-FRAMESIZE,width(),FRAMESIZE);
   p.setBrush(dark);
   p.drawRect(0,0,width(),FRAMESIZE);
   p.drawRect(0,0,FRAMESIZE,height());

   p.end();
}

// draw new player rects
void Tron::paintPlayers()
{
   QPainter p;
   p.begin(this);
   drawRect(p,players[0].xCoordinate,players[0].yCoordinate);
   drawRect(p,players[1].xCoordinate,players[1].yCoordinate);
   p.end();

   p.begin(pixmap);
   drawRect(p,players[0].xCoordinate,players[0].yCoordinate);
   drawRect(p,players[1].xCoordinate,players[1].yCoordinate);
   p.end();
}

void Tron::drawRect(QPainter & p, int x, int y)
{
   int xOffset=x*rectSize+(width()-fieldWidth*rectSize)/2;
   int yOffset=y*rectSize+(height()-fieldHeight*rectSize)/2;

   int type=playfield[x][y];

   // find out which color to draw
   QColor toDraw;
   int player;
   if(type&PLAYER1) // check player bit
   {
      toDraw=colors[1];
      player=0;
   }
   else if(type&PLAYER2)
   {
      toDraw=colors[2];
      player=1;
   }
   else
   {
      kdDebug() << "No player defined in Tron::drawRect(...)" << endl;
      return;
   }

   switch(style)
   {
      case Line:
         p.setBrush(toDraw);
         p.setPen(toDraw);
         p.drawRect(xOffset,yOffset,rectSize,rectSize);
         break;
      case OLine:
      {
         p.setBrush(toDraw);
         p.setPen(toDraw);
         p.drawRect(xOffset,yOffset,rectSize,rectSize);
         p.setPen(toDraw.light());
         if(type&TOP)
         {
            p.drawLine(xOffset,yOffset,xOffset+rectSize-1,yOffset);
         }
         if(type&LEFT)
         {
            p.drawLine(xOffset,yOffset,xOffset,yOffset+rectSize-1);
         }
         p.setPen(toDraw.dark());
         if(type&RIGHT)
         {
            p.drawLine(xOffset+rectSize-1,yOffset,xOffset+rectSize-1,yOffset+rectSize-1);
         }
         if(type&BOTTOM)
         {
            p.drawLine(xOffset,yOffset+rectSize-1,xOffset+rectSize-1,yOffset+rectSize-1);
         }

         break;
      }
      case Circle:
         p.setBrush(toDraw);
         p.setPen(toDraw);
         p.drawEllipse(xOffset ,yOffset ,rectSize,rectSize);
         break;
      case ORect:
         p.setBrush(toDraw);
         p.setPen(toDraw.light());
         p.drawRect(xOffset,yOffset,rectSize,rectSize);
         p.setPen(toDraw.dark());
         p.drawLine(xOffset,yOffset+rectSize-1,xOffset+rectSize-1
             ,yOffset+rectSize-1);
         p.drawLine(xOffset+rectSize-1,yOffset,xOffset+rectSize-1,yOffset+rectSize-1);
         break;
    }
}

/* *************************************************************** **
**                    config functions										 **
** *************************************************************** */

void Tron::setAccel(KAccel *acc)
{
   accel=acc;
}

void Tron::setBackgroundPix(QPixmap pix)
{
    bgPix=pix;

    if(pixmap!=0)
       updatePixmap();

    // most pictures have colors, that you can read white text
    setPalette(QColor("black"));
}

// configure colors
bool Tron::changeColor(int id)
{
   QColor color;
   int result=KColorDialog::getColor(color);

   bool flag=false;

   if(result)  // if button 'ok' pressed
   {
      // backgroundcolor changed
      if(id==0)
      {
         colors[0]=color;
         setPalette(color);
         bgPix.resize(0,0);
      }
      else  // set other color
      {
         colors[id]=color;
      }
      updatePixmap();
      repaint();

      flag=true;
   }

   return flag;
}

void Tron::saveColors(KConfig* config) const
{
   config->writeEntry("Color_Background",colors[0]);
   config->writeEntry("Color_Player1",colors[1]);
   config->writeEntry("Color_Player2",colors[2]);
}

void Tron::restoreColors(KConfig *config)
{
   QColor bg("black");
   QColor pl1("red");
   QColor pl2("blue");
   colors[0]=config->readColorEntry("Color_Background",&bg);
   colors[1]=config->readColorEntry("Color_Player1",&pl1);
   colors[2]=config->readColorEntry("Color_Player2",&pl2);

   setPalette(colors[0]);
}

void Tron::setVelocity(int newVel)            // set new velocity
{
  velocity=(10-newVel)*15;

  if(!gameEnded && !gamePaused)
    timer->changeInterval(velocity);

}

int Tron::getVelocity() const
{
  return 10-velocity/15;
}

void Tron::setStyle(TronStyle newStyle)
{
   style=newStyle;

   if(pixmap)
   {
      updatePixmap();
      repaint();
   }
}

TronStyle Tron::getStyle() const
{
   return style;
}

void Tron::setRectSize(int newSize)
{
   if(newSize!=rectSize)
   {
      rectSize=newSize;
      createNewPlayfield();
      reset();
   }
}

int Tron::getRectSize() const
{
   return rectSize;
}

void Tron::setAcceleratorBlocked(bool flag)
{
   blockAccelerator=flag;
}

bool Tron::acceleratorBlocked() const
{
   return blockAccelerator;
}

void Tron::enableWinnerColor(bool flag)
{
   changeWinnerColor=flag;
}

bool Tron::winnerColor() const
{
   return changeWinnerColor;
}

void Tron::setOppositeDirCrashes(bool flag)
{
   crashOnOppositeDir=flag;
}

bool Tron::oppositeDirCrashes() const
{
   return crashOnOppositeDir;
}

void Tron::setComputerplayer(Player player, bool flag)
{
  if(player==One)
  {
     players[0].setComputer(flag);
  }
  else if(player==Two)
  {
     players[1].setComputer(flag);
  }

  if(isComputer(Both))
  {
     QTimer::singleShot(1000,this,SLOT(computerStart()));
  }

}

bool Tron::isComputer(Player player)
{
   bool flag=false;
   if(player==One)
   {
       flag=players[0].computer;
   }
   else if(player==Two)
   {
      flag=players[1].computer;
   }
   else if(player==Both)
   {
      if(players[0].computer && players[1].computer)
         flag=true;
      else
         flag=false;
   }

   return flag;
}

void Tron::setSkill(Skill newSkill)
{
   _skill=newSkill;
}

Skill Tron::skill() const
{
   return _skill;
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
  else if(playfield[newX][newY] != BACKGROUND)
    flag=true;
  else flag=false;

  return flag;
}

void Tron::switchDir(int playerNr,Direction newDirection)
{
  if(playerNr!=0 && playerNr != 1)
  {
     kdDebug() << "wrong playerNr" << endl;
     return;
  }

  if (oppositeDirCrashes()==false)
  {
    if (newDirection==::Up && players[playerNr].dir==::Down)
      return;
    if (newDirection==::Down && players[playerNr].dir==::Up)
      return;
    if (newDirection==::Left && players[playerNr].dir==::Right)
      return;
    if (newDirection==::Right && players[playerNr].dir==::Left)
      return;
  }

  players[playerNr].dir=newDirection;
}

void Tron::updateDirections(int playerNr)
{
   if(playerNr==-1 || playerNr==0)
   {
      int x=players[0].xCoordinate;
      int y=players[0].yCoordinate;

      // necessary for drawing the 3d-line
      switch(players[0].dir)
      {
         // unset drawing flags in the moving direction
         case ::Up:
         {
  	    playfield[x][y] &= (~TOP);
  	    break;
       	 }
         case ::Down:
            playfield[x][y] &= (~BOTTOM);
            break;
         case ::Right:
            playfield[x][y] &= (~RIGHT);
            break;
         case ::Left:
            playfield[x][y] &= (~LEFT);
            break;
      }

   }
   if(playerNr==-1 || playerNr==1)
   {
      int x=players[1].xCoordinate;
      int y=players[1].yCoordinate;

      // necessary for drawing the 3d-line
      switch(players[1].dir)
      {
          // unset drawing flags in the moving direction
     	  case ::Up:
     	  {
             playfield[x][y] &= (~TOP);
             break;
       	  }
          case ::Down:
             playfield[x][y] &= (~BOTTOM);
             break;
          case ::Right:
             playfield[x][y] &= (~RIGHT);
             break;
          case ::Left:
             playfield[x][y] &= (~LEFT);
             break;
      }

   }

   paintPlayers();
}

/* *************************************************************** **
**                    			Events										 **
** *************************************************************** */

void Tron::paintEvent(QPaintEvent *e)
{
   bitBlt(this,e->rect().topLeft(),pixmap,e->rect());

   // if game is paused, print message
   if(gamePaused)
   {
      QString message=i18n("Game paused");
      QPainter p(this);
      QFontMetrics fm=p.fontMetrics();
      int w=fm.width(message);
      p.drawText(width()/2-w/2,height()/2,message);
   }

   // If game ended, print "Crash!"
   else if(gameEnded)
   {
      QString message=i18n("Crash!");
      QPainter p(this);
      int w=p.fontMetrics().width(message);
      for(int i=0;i<2;i++)
      {
         if(!players[i].alive)
     	 {
            int x=players[i].xCoordinate*rectSize+(width()%rectSize)/2-w/2;
            int y=players[i].yCoordinate*rectSize+(height()%rectSize)/2;
            if(players[i].dir==::Right)
            {
               x+=rectSize;
               y+=rectSize/2;
            }
            if(players[i].dir==::Left)
            {
               y+=rectSize/2;
            }
            if(players[i].dir==::Down)
            {
               y+=rectSize;
               x+=rectSize/2;
            }
            if(players[i].dir==::Up)
            {
               x+=rectSize/2;
               y+=rectSize/3;
            }


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
    createNewPlayfield();
    reset();
}

void Tron::keyPressEvent(QKeyEvent *e)
{
  int key=e->key();

  if(!players[1].computer)
  {
      if(key==accel->currentKey("Pl2::Up"))
		{
	  		switchDir(1,::Up);
	  		players[1].keyPressed=true;
		}
      else if(key==accel->currentKey("Pl2::Left"))
	   {
	  		switchDir(1,::Left);
	  		players[1].keyPressed=true;
		}
      else if(key==accel->currentKey("Pl2::Right"))
		{
	  		switchDir(1,::Right);
	  		players[1].keyPressed=true;
		}
      else if(key==accel->currentKey("Pl2::Down"))
		{
	  		switchDir(1,::Down);
	  		players[1].keyPressed=true;
		}
      else if(key==accel->currentKey("Pl2Ac"))
		{
		   if(!blockAccelerator)
	  			players[1].accelerated=true;
		}

  }

  if(!players[0].computer)
  {
      if(key==accel->currentKey("Pl1::Left"))
		{
	  		switchDir(0,::Left);
	  		players[0].keyPressed=true;
		}
      else if(key==accel->currentKey("Pl1::Right"))
		{
	  		switchDir(0,::Right);
	  		players[0].keyPressed=true;
		}
      else if(key==accel->currentKey("Pl1::Up"))
		{
	  		switchDir(0,::Up);
	  		players[0].keyPressed=true;
		}
      else if(key==accel->currentKey("Pl1::Down"))
		{
	 		switchDir(0,::Down);
	  		players[0].keyPressed=true;
		}

      else if(key==accel->currentKey("Pl1Ac"))
		{
		   if(!blockAccelerator)
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
  int key=e->key();

  if(!players[1].computer)
  {
      if(key==accel->currentKey("Pl2Ac"))
	   {
	      players[1].accelerated=false;
	      return;
  		}

		else if(key==accel->currentKey("Pl2::Left"))
		{
	  		players[1].keyPressed=false;
	  		return;
		}
      else if(key==accel->currentKey("Pl2::Right"))
		{
	  		players[1].keyPressed=false;
	  		return;
		}
      else if(key==accel->currentKey("Pl2::Up"))
		{
	  		players[1].keyPressed=false;
	  		return;
		}
      else if(key==accel->currentKey("Pl2::Down"))
		{
		  players[1].keyPressed=false;
		  return;
		}
  }

	if(!players[0].computer)
   {
   	if(key==accel->currentKey("Pl1::Left"))
    	{
	 		players[0].keyPressed=false;
	 		return;
    	}
      else if(key==accel->currentKey("Pl1::Right"))
		{
	  		players[0].keyPressed=false;
	  		return;
		}
      else if(key==accel->currentKey("Pl1::Up"))
		{
	 		players[0].keyPressed=false;
	  		return;
		}
     	else if(key==accel->currentKey("Pl1::Down"))
		{
	 		players[0].keyPressed=false;
	 		return;
		}
      else if(key==accel->currentKey("Pl1Ac"))
		{
	  		players[0].accelerated=false;
	  		return;
		}
 	}

   e->ignore();  // if pressed key is unknown, ignore it

}

// if playingfield looses keyboard focus, pause game
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
         repaint();
      }
   }
}

// doMove() is called from QTimer
void Tron::doMove()
{
   int i;
  	for(i=0;i<2;i++)
  	{
  		// Überprüfen, ob Acceleratortaste gedrückt wurde...
      if(players[i].accelerated)
		{

		   updateDirections(i);

		   int newType; // determine type of rect to set
		   if(i==0)
		   {
		      newType=PLAYER1;
		   }
		   else
		   {
		      newType=PLAYER2;
		   }
	  		switch(players[i].dir)
	    	{
	    		case ::Up:
	      		if(crashed(i,0,-1))
		 				players[i].alive=false;
	      		else
	      		{
	      			players[i].yCoordinate--;
	      			newType|=(TOP | LEFT | RIGHT);
	      		}
	      		break;
	    		case ::Down:
	    			if(crashed(i,0,1))
	       			players[i].alive=false;
	    			else
	    			{
	    			   players[i].yCoordinate++;
	    			   newType |= (BOTTOM | LEFT | RIGHT);
	    			}

	      		break;
	    		case ::Left:
	       		if(crashed(i,-1,0))
		 				players[i].alive=false;
	       		else
	       		{
	       		   players[i].xCoordinate--;
	       		   newType |= (LEFT | TOP | BOTTOM);
	       		}
	      		break;
	    		case ::Right:
	      	if(crashed(i,1,0))
					players[i].alive=false;
	      	else
	      	{
	      	   players[i].xCoordinate++;
	      	   newType |= (RIGHT | TOP | BOTTOM);
	      	}
	      	break;
	    	}

	      if(players[i].alive)
      		playfield[players[i].xCoordinate][players[i].yCoordinate]=newType;
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
	    		case ::Left:
	      		xInc = -1;
	      		break;
	    		case ::Right:
	      		xInc = 1;
	      		break;
	    		case ::Up:
	      		yInc = -1;
	      		break;
	    		case ::Down:
	      		yInc = 1;
	      		break;
	    	}
	  		if ((players[1].xCoordinate+xInc) == players[0].xCoordinate)
	    		if ((players[1].yCoordinate+yInc) == players[0].yCoordinate)
	      	{
					players[0].alive=false;
	      	}
		}

      paintPlayers();

      // crashtest
      if(!players[0].alive && !players[1].alive)
		{
		   stopGame();
	  		players[0].score++;
	  		players[1].score++;
	  		showWinner(Both);
		}
      else
      {
			for(i=0;i<2;i++)
	  		{
	    		if(!players[i].alive)
	      	{
	      	   stopGame();
					showWinner((i==0)? Two:One);
					players[i].score++;
	      	}
	  		}
	  	}


      if(gameEnded)
		{
	  		//this is for waiting 0,5s before starting next game
	  		gameBlocked=true;
	  		QTimer::singleShot(1000,this,SLOT(unblockGame()));
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
			newType=PLAYER1;
		else
			newType=PLAYER2;

      switch(players[i].dir)
		{
			case ::Up:
	      	if(crashed(i,0,-1))
					players[i].alive=false;
	      	else
	      	{
	      	   players[i].yCoordinate--;
	      	   newType |= (TOP | RIGHT | LEFT);
	      	}
	  			break;
			case ::Down:
	      	if(crashed(i,0,1))
					players[i].alive=false;
	      	else
	      	{
	      	    players[i].yCoordinate++;
	      	    newType |= (BOTTOM | RIGHT | LEFT);
	      	}
	  			break;
			case ::Left:
	      	if(crashed(i,-1,0))
					players[i].alive=false;
	      	else
	      	{
	      	   players[i].xCoordinate--;
	      	   newType |= (LEFT | TOP | BOTTOM);
	      	}
	  			break;
			case ::Right:
	      	if(crashed(i,1,0))
					players[i].alive=false;
	      	else
	      	{
	      	   players[i].xCoordinate++;
	      	   newType |= (RIGHT | TOP | BOTTOM);
	      	}
	  			break;
		}
		if(players[i].alive)
			playfield[players[i].xCoordinate][players[i].yCoordinate]=newType;

 	}

  	/* player collision check */
  	if(!players[1].alive)
   {
      int xInc=0,yInc=0;
      switch(players[1].dir)
		{
			case ::Left:
	  			xInc = -1; break;
			case ::Right:
	  			xInc = 1; break;
			case ::Up:
	  			yInc = -1; break;
			case ::Down:
	  			yInc = 1; break;
		}
      if ((players[1].xCoordinate+xInc) == players[0].xCoordinate)
			if ((players[1].yCoordinate+yInc) == players[0].yCoordinate)
	  		{
	    		players[0].alive=false;
	  		}
   }

  paintPlayers();

  if(!players[0].alive && !players[1].alive)
  {
      stopGame();
      players[0].score++;
      players[1].score++;
      showWinner(Both);
  }
	else
		for(i=0;i<2;i++)
    	{
			// crashtests
			if(!players[i].alive)
	  		{
	  		   stopGame();
	    		showWinner((i==0)? Two:One);
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
if(_skill != Easy)
{
  int opponent=(playerNr==1)? 0 : 1;

  // determines left and right side
  Direction sides[2];
  // increments for moving to the different sides
  int flags[6]={0,0,0,0,0,0};
  int index[2];
  // distances to barrier
  int dis_forward,  dis_left, dis_right;

  dis_forward = dis_left = dis_right = 1;


  	switch (players[playerNr].dir)
   	{
  			case ::Left:
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
    			sides[0] = ::Down;
    			sides[1] = ::Up;
    			break;
  			case ::Right:
    			flags[0] = 1;
    			flags[1] = 0;
    			flags[2] = 0;
			   flags[3] = -1;
    			flags[4] = 0;
    			flags[5] = 1;
    			sides[0] = ::Up;
    			sides[1] = ::Down;
    			break;
  			case ::Up:
    			flags[0] = 0;
    			flags[1] = -1;
    			flags[2] = -1;
    			flags[3] = 0;
    			flags[4] = 1;
    			flags[5] = 0;
    			sides[0] = ::Left;
    			sides[1] = ::Right;
    			break;
  			case ::Down:
    			flags[0] = 0;
    			flags[1] = 1;
    			flags[2] = 1;
    			flags[3] = 0;
    			flags[4] = -1;
    			flags[5] = 0;
    			sides[0] = ::Right;
    			sides[1] = ::Left;
    			break;
  		}

  		// check forward
  		index[0] = players[playerNr].xCoordinate+flags[0];
  		index[1] = players[playerNr].yCoordinate+flags[1];
  		while (index[0] < fieldWidth && index[0] >= 0 &&
	 		index[1] < fieldHeight && index[1] >= 0 &&
	 		playfield[index[0]][index[1]] == BACKGROUND)
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
	   playfield[index[0]][index[1]] == BACKGROUND) {
      dis_left++;
      index[0] += flags[2];
      index[1] += flags[3];
    }

    // check right
    index[0] = players[playerNr].xCoordinate+flags[4];
    index[1] = players[playerNr].yCoordinate+flags[5];
    while (index[0] < fieldWidth && index[0] >= 0 &&
	   index[1] <  fieldHeight && index[1] >= 0 &&
	   playfield[index[0]][index[1]] == BACKGROUND) {
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
  	   case ::Up:
  	      opForwardDis=vert_dis;
  	      opSideDis=-hor_dis;
  	      if(players[opponent].dir==::Down)
  	         opMovesOppositeDir=true;
  	      else if(players[opponent].dir==::Up)
  	         opMovesSameDir=true;
  	      else if(players[opponent].dir==::Left)
  	         opMovesLeft=true;
  	      else if(players[opponent].dir==::Right)
  	         opMovesRight=true;
  	      break;
  	   case ::Down:
  	      opForwardDis=-vert_dis;
  	      opSideDis=hor_dis;
  	      if(players[opponent].dir==::Up)
  	         opMovesOppositeDir=true;
  	      else if(players[opponent].dir==::Down)
  	         opMovesSameDir=true;
  	      else if(players[opponent].dir==::Left)
  	         opMovesRight=true;
  	      else if(players[opponent].dir==::Right)
  	         opMovesLeft=true;
  	      break;
  	   case ::Left:
  	      opForwardDis=hor_dis;
  	      opSideDis=vert_dis;
  	      if(players[opponent].dir==::Right)
  	         opMovesOppositeDir=true;
  	      else if(players[opponent].dir==::Left)
  	         opMovesSameDir=true;
  	      else if(players[opponent].dir==::Down)
  	         opMovesLeft=true;
  	      else if(players[opponent].dir==::Up)
  	         opMovesRight=true;
  	      break;
  	   case ::Right:
  	      opForwardDis=-hor_dis;
  	      opSideDis=-vert_dis;
  	      if(players[opponent].dir==::Left)
  	         opMovesOppositeDir=true;
  	      else if(players[opponent].dir==::Right)
  	         opMovesSameDir=true;
  	      else if(players[opponent].dir==::Up)
  	         opMovesLeft=true;
  	      else if(players[opponent].dir==::Down)
  	         opMovesRight=true;
  	      break;
  	}

  	int doPercentage=100;
  	if(_skill==Medium)
  		doPercentage=5;
  	else if(_skill==Hard)
  		doPercentage=90;

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
// This part is completly ported from
// xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>
else //_skill==Easy
{
  Direction sides[2];
  int flags[6] = {0,0,0,0,0,0};
  int index[2];
  int dis_forward,  dis_left, dis_right;

  dis_forward = dis_left = dis_right = 1;

  switch (players[playerNr].dir) {
  case ::Left:

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
    sides[0] = ::Down;
    sides[1] = ::Up;
    break;
  case ::Right:
    flags[0] = 1;
    flags[1] = 0;
    flags[2] = 0;
    flags[3] = -1;
    flags[4] = 0;
    flags[5] = 1;
    sides[0] = ::Up;
    sides[1] = ::Down;
    break;
  case ::Up:
    flags[0] = 0;
    flags[1] = -1;
    flags[2] = -1;
    flags[3] = 0;
    flags[4] = 1;
    flags[5] = 0;
    sides[0] = ::Left;
    sides[1] = ::Right;
    break;
  case ::Down:
    flags[0] = 0;
    flags[1] = 1;
    flags[2] = 1;
    flags[3] = 0;
    flags[4] = -1;
    flags[5] = 0;
    sides[0] = ::Right;
    sides[1] = ::Left;
    break;
  }

  // check forward
  index[0] = players[playerNr].xCoordinate+flags[0];
  index[1] = players[playerNr].yCoordinate+flags[1];
  while (index[0] < fieldWidth && index[0] >= 0 &&
	 index[1] < fieldHeight && index[1] >= 0 &&
	 playfield[index[0]][index[1]] == BACKGROUND) {
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
	   playfield[index[0]][index[1]] == BACKGROUND) {
      dis_left++;
      index[0] += flags[2];
      index[1] += flags[3];
    }

    // check right
    index[0] = players[playerNr].xCoordinate+flags[4];
    index[1] = players[playerNr].yCoordinate+flags[5];
    while (index[0] < fieldWidth && index[0] >= 0 &&
	   index[1] <  fieldHeight && index[1] >= 0 &&
	   playfield[index[0]][index[1]] == BACKGROUND) {
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
   Direction currentDir=players[playerNr].dir;
   Direction sides[2];
   switch (currentDir)
   {
  		case ::Left:
    		//turns to either side
    		sides[0] = ::Down;
    		sides[1] = ::Up;
    		break;
  		case ::Right:
    		sides[0] = ::Up;
    		sides[1] = ::Down;
    		break;
  		case ::Up:
    		sides[0] = ::Left;
    		sides[1] = ::Right;
    		break;
  		case ::Down:
    		sides[0] = ::Right;
    		sides[1] = ::Left;
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

#include "tron.moc"
