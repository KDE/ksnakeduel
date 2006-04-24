/****************************************************************************
  This file is part of the game 'KTron'

  Copyright (C) 1998-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
  Copyright (C) 2005 Benjamin C. Meyer <ben at meyerhome dot net>

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
#include <kmessagebox.h>

// Normal class
#include <qtimer.h>
//Added by qt3to4:
#include <QPixmap>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QVector>

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kcolordialog.h>
#include <kaction.h>

#include "settings.h"
#include "tron.h"

#define TRON_FRAMESIZE 2

/**
 * init-functions
 **/

Tron::Tron(QWidget *parent) : QWidget(parent)
{
  pixmap=0;
  playfield=0;
  beginHint=false;
  lookForward=15;

  random.setSeed(0);

  setFocusPolicy(Qt::StrongFocus);

  gameBlocked=false;
  rectSize=10;

  timer = new QTimer(this);
  loadSettings();
  connect(timer, SIGNAL(timeout()), SLOT(doMove()));
  QTimer::singleShot(15000, this,SLOT(showBeginHint()));
}

void Tron::loadSettings(){
  setPalette(Settings::color_Background());

  // Size
  int newSize = Settings::rectSize();
  if(newSize!=rectSize){
    rectSize=newSize;
    createNewPlayfield();
  }

  reset();

  // Velocity
  setVelocity( Settings::velocity() );

  // Style
  if(pixmap){
    updatePixmap();
    update();
  }

  // Backgroundimage
#warning commented this because with it it crashed
//  setBackgroundPix(NULL);
  if(Settings::backgroundImageChoice()){
    KUrl url ( Settings::backgroundImage() );
    if(!url.isEmpty()){
      QString tmpFile;
      KIO::NetAccess::download(url, tmpFile, this);
      QPixmap pix(tmpFile);
      if(!pix.isNull()){
        setBackgroundPix(pix);
      } else {
  	QString msg=i18n("Wasn't able to load wallpaper\n%1", tmpFile);
	KMessageBox::sorry(this, msg);
      }
      KIO::NetAccess::removeTempFile(tmpFile);
    }
#warning commented this because with it it crashed
  //  else setBackgroundPix(NULL);
  }
  setComputerplayer(One, Settings::computerplayer1());
  setComputerplayer(Two, Settings::computerplayer2());
}

Tron::~Tron()
{
  if(playfield)
    {
      delete []  playfield;
    }
  if(pixmap)
    delete pixmap;
  delete timer;

}

void Tron::createNewPlayfield()
{
  if(playfield)
    delete [] playfield;

  if(pixmap)
    delete pixmap;

  // field size
  fieldWidth=(width()-2*TRON_FRAMESIZE)/rectSize;
  fieldHeight=(height()-2*TRON_FRAMESIZE)/rectSize;

  // start positions
  playfield=new QVector<int>[fieldWidth];
  for(int i=0;i<fieldWidth;i++)
    playfield[i].resize(fieldHeight);

  pixmap=new QPixmap(size());
  pixmap->fill(Settings::color_Background());

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
   players[0].last_dir = Directions::None;
   players[1].last_dir = Directions::None;
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

   if(player != Both && Settings::changeWinnerColor())
   {
      int winner;
      int loser;
      if(player==One)
      {
         winner=PLAYER1;
         loser=PLAYER2;
      }
      else
      {
         winner=PLAYER2;
         loser=PLAYER1;
      }

      for(i=0;i<fieldWidth;i++)
         for(j=0;j<fieldHeight;j++)
         {
            if(playfield[i][j]!=BACKGROUND)
            {
               // change player
               playfield[i][j] |= winner;
               playfield[i][j] &= ~loser;
            }
         }

      updatePixmap();
   }

   update();

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

  QPainter p;
  p.begin(pixmap);

  if(!bgPix.isNull())
  {
     int pw=bgPix.width();
     int ph=bgPix.height();
     for (int x = 0; x <= width(); x+=pw)
        for (int y = 0; y <= height(); y+=ph)
	    p.drawPixmap(x, y, bgPix);
  }
  else
  {
    pixmap->fill(Settings::color_Background());
  }

  // Examine all Pixels and draw
  for(i=0;i<fieldWidth;i++)
     for(j=0;j<fieldHeight;j++)
     {
        if(playfield[i][j]!=BACKGROUND)
        {
           drawRect(p,i,j);
	}
     }

   // draw frame
   QColor light=parentWidget()->palette().color( QPalette::Midlight );
   QColor dark=parentWidget()->palette().color( QPalette::Mid );

   p.setPen(Qt::NoPen);
   p.setBrush(light);
      p.drawRect(width()-TRON_FRAMESIZE,0,TRON_FRAMESIZE,height());
   p.drawRect(0,height()-TRON_FRAMESIZE,width(),TRON_FRAMESIZE);
   p.setBrush(dark);
   p.drawRect(0,0,width(),TRON_FRAMESIZE);
   p.drawRect(0,0,TRON_FRAMESIZE,height());

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
      toDraw=Settings::color_Player1();
      player=0;
   }
   else if(type&PLAYER2)
   {
      toDraw=Settings::color_Player2();
      player=1;
   }
   else
   {
      kDebug() << "No player defined in Tron::drawRect(...)" << endl;
      return;
   }

   switch(Settings::style())
   {
      case Settings::EnumStyle::Line:
         p.setBrush(toDraw);
         p.setPen(toDraw);
         p.drawRect(xOffset,yOffset,rectSize,rectSize);
         break;
      case Settings::EnumStyle::OLine:
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
      case Settings::EnumStyle::Circle:
         p.setBrush(toDraw);
         p.setPen(toDraw);
         p.drawEllipse(xOffset ,yOffset ,rectSize,rectSize);
         break;
      case Settings::EnumStyle::ORect:
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

void Tron::setActionCollection(KActionCollection *a)
{
   actionCollection = a;
}

void Tron::setBackgroundPix(QPixmap pix)
{
    bgPix=pix;

    if(pixmap!=0){
       updatePixmap();
      // most pictures have colors, that you can read white text
      setPalette(QColor("black"));
    }
}

void Tron::setVelocity(int newVel)            // set new velocity
{
  velocity=(10-newVel)*15;

  if(!gameEnded && !gamePaused)
    timer->start(velocity);
}

void Tron::setComputerplayer(Player player, bool flag) {
  if(player==One)
    players[0].setComputer(flag);
  else if(player==Two)
    players[1].setComputer(flag);

  if(isComputer(Both))
      QTimer::singleShot(1000,this,SLOT(computerStart()));
}

bool Tron::isComputer(Player player)
{
   if(player==One)
     return players[0].computer;
   else if(player==Two)
     return players[1].computer;
   else if(player==Both)
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
  else if(playfield[newX][newY] != BACKGROUND)
    flag=true;
  else flag=false;

  return flag;
}

void Tron::switchDir(int playerNr,Directions::Direction newDirection)
{
  if(playerNr!=0 && playerNr != 1)
  {
     kDebug() << "wrong playerNr" << endl;
     return;
  }

  if (Settings::oppositeDirCrashes()==false)
  {
    if (newDirection==Directions::Up && players[playerNr].last_dir==Directions::Down)
      return;
    if (newDirection==Directions::Down && players[playerNr].last_dir==Directions::Up)
      return;
    if (newDirection==Directions::Left && players[playerNr].last_dir==Directions::Right)
      return;
    if (newDirection==Directions::Right && players[playerNr].last_dir==Directions::Left)
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
         case Directions::Up:
         {
            playfield[x][y] &= (~TOP);
            break;
         }
         case Directions::Down:
            playfield[x][y] &= (~BOTTOM);
            break;
         case Directions::Right:
            playfield[x][y] &= (~RIGHT);
            break;
         case Directions::Left:
            playfield[x][y] &= (~LEFT);
            break;
         default:
            break;
      }
      players[0].last_dir = players[0].dir;

   }
   if(playerNr==-1 || playerNr==1)
   {
      int x=players[1].xCoordinate;
      int y=players[1].yCoordinate;

      // necessary for drawing the 3d-line
      switch(players[1].dir)
      {
          // unset drawing flags in the moving direction
     	  case Directions::Up:
     	  {
             playfield[x][y] &= (~TOP);
             break;
       	  }
          case Directions::Down:
             playfield[x][y] &= (~BOTTOM);
             break;
          case Directions::Right:
             playfield[x][y] &= (~RIGHT);
             break;
          case Directions::Left:
             playfield[x][y] &= (~LEFT);
             break;
         default:
            break;
      }
      players[1].last_dir = players[1].dir;

   }

   update();
}

/* *************************************************************** **
**                    			Events										 **
** *************************************************************** */

void Tron::paintEvent(QPaintEvent *e)
{
   QPainter p(this);
   p.drawPixmap(e->rect().topLeft(), *pixmap, e->rect());

   // if game is paused, print message
   if(gamePaused)
   {
      QString message=i18n("Game paused");
      QFontMetrics fm=p.fontMetrics();
      int w=fm.width(message);
      p.drawText(width()/2-w/2,height()/2,message);
   }

   // If game ended, print "Crash!"
   else if(gameEnded)
   {
      QString message=i18n("Crash!");
      int w=p.fontMetrics().width(message);
      int h=p.fontMetrics().height();
      for(int i=0;i<2;i++)
      {
         if(!players[i].alive)
     	 {
            int x=players[i].xCoordinate*rectSize;
            int y=players[i].yCoordinate*rectSize;
            while(x<0) x+=rectSize;
            while(x+w>width()) x-=rectSize;
            while(y-h<0) y+=rectSize;
            while(y>height()) y-=rectSize;
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
   else /* Qt4 porting question if something? */ paintPlayers();
}

void Tron::resizeEvent(QResizeEvent *)
{
    createNewPlayfield();
    reset();
}

void Tron::keyPressEvent(QKeyEvent *e)
{
  if(!players[1].computer)
  {
      if(actionCollection->action("Pl2Up")->shortcut().contains(e->key()))
		{
	  		switchDir(1,Directions::Up);
	  		players[1].keyPressed=true;
		}
      else if(actionCollection->action("Pl2Left")->shortcut().contains(e->key()))
	   {
	  		switchDir(1,Directions::Left);
	  		players[1].keyPressed=true;
		}
      else if(actionCollection->action("Pl2Right")->shortcut().contains(e->key()))
		{
	  		switchDir(1,Directions::Right);
	  		players[1].keyPressed=true;
		}
      else if(actionCollection->action("Pl2Down")->shortcut().contains(e->key()))
		{
	  		switchDir(1,Directions::Down);
	  		players[1].keyPressed=true;
		}
      else if(actionCollection->action("Pl2Ac")->shortcut().contains(e->key()))
		{
		   if(!Settings::acceleratorBlocked())
	  			players[1].accelerated=true;

		}
  }

  if(!players[0].computer)
  {
      if(actionCollection->action("Pl1Left")->shortcut().contains(e->key()))
		{
	  		switchDir(0,Directions::Left);
	  		players[0].keyPressed=true;
		}
      else if(actionCollection->action("Pl1Right")->shortcut().contains(e->key()))
		{
	  		switchDir(0,Directions::Right);
	  		players[0].keyPressed=true;
		}
      else if(actionCollection->action("Pl1Up")->shortcut().contains(e->key()))
		{
	  		switchDir(0,Directions::Up);
	  		players[0].keyPressed=true;
		}
      else if(actionCollection->action("Pl1Down")->shortcut().contains(e->key()))
		{
	 		switchDir(0,Directions::Down);
	  		players[0].keyPressed=true;
		}
      else if(actionCollection->action("Pl1Ac")->shortcut().contains(e->key()))
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
      if(actionCollection->action("Pl2Ac")->shortcut().contains(e->key()))
		{
			players[1].accelerated=false;
			return;
  		}
      else if(actionCollection->action("Pl2Left")->shortcut().contains(e->key()))
		{
			players[1].keyPressed=false;
			return;
		}
      else if(actionCollection->action("Pl2Right")->shortcut().contains(e->key()))
		{
	  		players[1].keyPressed=false;
	  		return;
		}
      else if(actionCollection->action("Pl2Up")->shortcut().contains(e->key()))
		{
	  		players[1].keyPressed=false;
	  		return;
		}
      else if(actionCollection->action("Pl2Down")->shortcut().contains(e->key()))
		{
		  players[1].keyPressed=false;
		  return;
		}
  }

   if(!players[0].computer)
   {
      if(actionCollection->action("Pl1Left")->shortcut().contains(e->key()))
		{
	 		players[0].keyPressed=false;
	 		return;
		}
      else if(actionCollection->action("Pl1Right")->shortcut().contains(e->key()))
		{
	  		players[0].keyPressed=false;
	  		return;
		}
      else if(actionCollection->action("Pl1Up")->shortcut().contains(e->key()))
		{
	 		players[0].keyPressed=false;
	  		return;
		}
      else if(actionCollection->action("Pl1Down")->shortcut().contains(e->key()))
		{
	 		players[0].keyPressed=false;
	 		return;
		}
      else if(actionCollection->action("Pl1Ac")->shortcut().contains(e->key()))
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
            newType=PLAYER1;
         }
         else
         {
            newType=PLAYER2;
         }
         switch(players[i].dir)
         {
            case Directions::Up:
               if(crashed(i,0,-1))
                  players[i].alive=false;
               else
               {
                  players[i].yCoordinate--;
                  newType|=(TOP | LEFT | RIGHT);
               }
            break;
            case Directions::Down:
               if(crashed(i,0,1))
                  players[i].alive=false;
               else
               {
                  players[i].yCoordinate++;
                  newType |= (BOTTOM | LEFT | RIGHT);
               }
            break;
            case Directions::Left:
               if(crashed(i,-1,0))
                  players[i].alive=false;
               else
               {
                  players[i].xCoordinate--;
                  newType |= (LEFT | TOP | BOTTOM);
               }
            break;
            case Directions::Right:
               if(crashed(i,1,0))
                  players[i].alive=false;
               else
               {
                  players[i].xCoordinate++;
                  newType |= (RIGHT | TOP | BOTTOM);
               }
            break;
            default:
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

      update();

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
         newType=PLAYER1;
      else
         newType=PLAYER2;

      switch(players[i].dir)
      {
         case Directions::Up:
            if(crashed(i,0,-1))
               players[i].alive=false;
            else
            {
               players[i].yCoordinate--;
               newType |= (TOP | RIGHT | LEFT);
            }
         break;
         case Directions::Down:
            if(crashed(i,0,1))
               players[i].alive=false;
            else
            {
               players[i].yCoordinate++;
               newType |= (BOTTOM | RIGHT | LEFT);
            }
         break;
         case Directions::Left:
            if(crashed(i,-1,0))
               players[i].alive=false;
            else
            {
               players[i].xCoordinate--;
               newType |= (LEFT | TOP | BOTTOM);
            }
         break;
         case Directions::Right:
            if(crashed(i,1,0))
               players[i].alive=false;
            else
            {
               players[i].xCoordinate++;
               newType |= (RIGHT | TOP | BOTTOM);
            }
         break;
         default:
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

   update();

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
if(Settings::skill() != Settings::EnumSkill::Easy)
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
  	switch(Settings::skill())
  	{
  	  case Settings::EnumSkill::Easy:
  	        // Never reached
  		break;

  	  case Settings::EnumSkill::Medium:
  		doPercentage=5;
  		break;

  	  case Settings::EnumSkill::Hard:
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

#include "tron.moc"

