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

#include "ktron.h"
#include "version.h"
#include "ktoptdlg.h"

#include <kglobal.h>
#include <kaccel.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kkeydialog.h>
#include <kfiledialog.h>
#include <kstddirs.h>
#include <kmenubar.h>
#include <kconfig.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kapp.h>

#define ID_STATUS_BASE 40

#define MESSAGE_TIME 2000

KTron::KTron(const char *name)
      : KTMainWindow(name),
       skillAction(3)
{
   playerPoints[0]=playerPoints[1]=0;

   setMinimumSize(150,200);
   optionsDialog=0;

   tron=new Tron(this);
   connect(tron,SIGNAL(gameEnds(Player)),SLOT(changeStatus(Player)));
   setView(tron);

   // create statusbar
   KStatusBar *statusbar= statusBar();
   statusbar->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE+1);
   statusbar->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE+2);

   accel=new KAccel(this,"Key Accel");

   accel->insertItem(i18n("Player 1 Up"),"Pl1Up","R");
   accel->insertItem(i18n("Player 1 Down"),"Pl1Down","F");
   accel->insertItem(i18n("Player 1 Right"),"Pl1Right","G");
   accel->insertItem(i18n("Player 1 Left"),"Pl1Left","D");
   accel->insertItem(i18n("Player 1 Accelerator"),"Pl1Ac","A");

   accel->insertItem(i18n("Player 2 Up"),"Pl2Up","Up");
   accel->insertItem(i18n("Player 2 Down"),"Pl2Down","Down");
   accel->insertItem(i18n("Player 2 Right"),"Pl2Right","Right");
   accel->insertItem(i18n("Player 2 Left"),"Pl2Left","Left");
   accel->insertItem(i18n("Player 2 Accelerator"),"Pl2Ac","Plus");
   accel->readSettings();

   tron->setAccel(accel);

   KAction* action;
   action=new KAction(i18n("&Pause/Continue"), Key_P, tron, SLOT(togglePause()),
          actionCollection(), "game_pause");
   action->plugAccel(accel);
   action=new KAction(i18n("&New Game"), KStdAccel::openNew(), tron, SLOT(newGame()),
          actionCollection(), "game_new");
   action->plugAccel(accel);
   action=new KAction(i18n("&Exit"), KStdAccel::quit(), this, SLOT(quit()),
          actionCollection(), "game_quit");
   action->plugAccel(accel);

   /* options-menu  */
   new KToggleAction(i18n("Player &1"), 0 , this, SLOT(toggleComPl1()),
          actionCollection(), "computer_player1");
   new KToggleAction(i18n("Player &2"), 0 , this, SLOT(toggleComPl2()),
          actionCollection(), "computer_player2");

   skillAction[0]=new KRadioAction(i18n("Beginner"), 0 , this, SLOT(beginnerSkill()),
          actionCollection(), "skill_beginner");
   skillAction[1]=new KRadioAction(i18n("Average"), 0 , this, SLOT(averageSkill()),
          actionCollection(), "skill_average");
   skillAction[2]=new KRadioAction(i18n("Expert"), 0 , this, SLOT(expertSkill()),
          actionCollection(), "skill_expert");
   uint i;
   for (i=0; i<skillAction.size(); i++)
      skillAction[i]->setExclusiveGroup("skill");


   KSelectAction* selectAction = new KSelectAction( i18n("&Velocity"),  0
                 , actionCollection(), "select_velocity");
   QStringList itemList;
   itemList.append(i18n("&1 (slow)"));
   itemList.append(i18n("&2"));
   itemList.append(i18n("&3"));
   itemList.append(i18n("&4"));
   itemList.append( i18n("&5 (default)") );
   itemList.append(i18n("&6"));
   itemList.append(i18n("&7"));
   itemList.append(i18n("&8"));
   itemList.append(i18n("&9 (fast)"));
   selectAction->setItems(itemList);
   connect(selectAction, SIGNAL(activated(int)), this, SLOT(setVelocity(int)));

   selectAction = new KSelectAction( i18n("&Drawing style"),  0
                 , actionCollection(), "select_style");
   itemList.clear();
   itemList.append(i18n("&3D Line"));
   itemList.append(i18n("3&D Rects"));
   itemList.append(i18n("&Line"));
   itemList.append(i18n("&Circles"));
   selectAction->setItems(itemList);
   connect(selectAction, SIGNAL(activated(int)), this, SLOT(setStyle(int)));

   selectAction = new KSelectAction( i18n("Si&ze"),  0
                 , actionCollection(), "select_size");
   itemList.clear();
   itemList.append(i18n("&Very Small"));
   itemList.append(i18n("&Small"));
   itemList.append(i18n("&Medium"));
   itemList.append(i18n("&Large"));
   itemList.append(i18n("&Very Large"));
   selectAction->setItems(itemList);
   connect(selectAction, SIGNAL(activated(int)), this, SLOT(setSize(int)));



   action=new KAction(i18n("Color Player &1..."), 0, this, SLOT(colorPl1()),
          actionCollection(), "color_player1");
   action->plugAccel(accel);
   action=new KAction(i18n("Color Player &2..."), 0, this, SLOT(colorPl2()),
          actionCollection(), "color_player2");
   action->plugAccel(accel);
   action=new KAction(i18n("&Backgroundcolor..."), 0, this, SLOT(colorBackground()),
          actionCollection(), "color_background");
   action->plugAccel(accel);
   action=new KAction(i18n("B&ackground image..."), 0, this, SLOT(chooseBgPix()),
          actionCollection(), "background_image");
   action->plugAccel(accel);



   KStdAction::showStatusbar(this, SLOT(toggleStatusbar()), actionCollection());
   KStdAction::keyBindings(this, SLOT(configureKeys()), actionCollection());

   new KAction(i18n("Other..."), 0 , this, SLOT(configureOther()),
          actionCollection(), "configure_other");

   createGUI("ktronui.rc");

  	
   // read config
   KConfig *config=kapp->config();
  	
   config->setGroup("Window");

   KToggleAction* toggleAction;
	
   bool visible=config->readBoolEntry("Statusbar",true);
   toggleAction=(KToggleAction *)actionCollection()->action(KStdAction::stdName(KStdAction::ShowStatusbar));
   toggleAction->setChecked(visible);

   int width=config->readNumEntry("Width",400);
   int height=config->readNumEntry("Height",300);
   resize(width,height);


   config->setGroup("Game");
   bool status=config->readBoolEntry("Computerplayer1",true);
   toggleAction=(KToggleAction *)actionCollection()->action("computer_player1");
   toggleAction->setChecked(status);
   tron->setComputerplayer(One,status);
   
   status=config->readBoolEntry("Computerplayer2",false);
   toggleAction=(KToggleAction *)actionCollection()->action("computer_player2");
   toggleAction->setChecked(status);
   tron->setComputerplayer(Two,status);

   QString temp="";
   playerName[0]=config->readEntry("Name_Pl1",temp);
   if(playerName[0]=="")
      playerName[0]=i18n("Player 1");
   playerName[1]=config->readEntry("Name_Pl2",temp);
   if(playerName[1]=="")
      playerName[1]=i18n("Player 2");

   updateStatusbar();

   Skill skill=(Skill)config->readNumEntry("Skill",(int)Medium);
   skillAction[skill]->setChecked(true);
   tron->setSkill(skill);

   selectAction=(KSelectAction*)actionCollection()->action("select_velocity");
   int velocity=config->readNumEntry("Velocity",5);
   selectAction->setCurrentItem(velocity-1);
   tron->setVelocity(velocity);


   selectAction=(KSelectAction*)actionCollection()->action("select_style");
   TronStyle newStyle=(TronStyle)config->readNumEntry("Style",(int) OLine);
   // if the configfile is from a older than from KTron 0.5 than switch to
   // 3d-Line
   config->setGroup("KTron");
   if(!config->hasKey("VERSION"))
   {
      config->writeEntry("VERSION",KTRON_VERSION);
      newStyle=OLine;
      config->setGroup("Game");
      config->writeEntry("Style",(int)OLine);
   }
   config->setGroup("Game");
   selectAction->setCurrentItem(newStyle);
   tron->setStyle(newStyle);

   int size=config->readNumEntry("RectSize",10);
   selectAction=(KSelectAction*)actionCollection()->action("select_size");
   switch(size)
   {
      case 4:
         selectAction->setCurrentItem(0);
         break;
      case 7:
         selectAction->setCurrentItem(1);
         break;
      case 10:
         selectAction->setCurrentItem(2);
         break;
      case 13:
         selectAction->setCurrentItem(3);
         break;
      case 16:
         selectAction->setCurrentItem(4);
         break;
   }
   tron->setRectSize(size);

   tron->enableWinnerColor(config->readBoolEntry("ChangeWinnerColor",true));
   tron->setAcceleratorBlocked(config->readBoolEntry("AcceleratorBlocked",false));

   tron->restoreColors(config);
   readBackground(config);
}

// Destruktor

KTron::~KTron()
{
	saveSettings();
}


void KTron::updateStatusbar()
{
   for(int i=0;i<2;i++)
   {
      Player player;
      player=(i==0?One:Two);
	
      QString s;
      QString name;
      if(tron->isComputer(Both))
      {
         name=i18n("Computer(%1)").arg(i+1);
      }
      else if(tron->isComputer(player))
      {
         name=i18n("Computer");
      }
      else
      {
         name=playerName[i];
      }
      s="%1: %2";
      s=s.arg(name).arg(playerPoints[i]);
      statusBar()->changeItem(s,ID_STATUS_BASE+i+1);
   }
}

void KTron::saveSettings()
{
   KConfig *config=kapp->config();

   KConfigGroupSaver saver(config,"Window");

   KToggleAction* toggleAction;	
   toggleAction=(KToggleAction *)actionCollection()->action(KStdAction::stdName(KStdAction::ShowStatusbar));
   config->writeEntry("Statusbar",toggleAction->isChecked());

   config->writeEntry("Width",width());
   config->writeEntry("Height",height());

   config->setGroup("Game");
   config->writeEntry("Velocity",tron->getVelocity());
   config->writeEntry("Style",(int)tron->getStyle());
   config->writeEntry("RectSize",tron->getRectSize());

   config->writeEntry("Computerplayer1", tron->isComputer(One));
   config->writeEntry("Computerplayer2", tron->isComputer(Two));
   config->writeEntry("Skill",(int)tron->skill());

   config->writeEntry("ChangeWinnerColor",tron->winnerColor());
   config->writeEntry("AcceleratorBlocked",tron->acceleratorBlocked());

   if(playerName[0]==i18n("Player 1"))
      config->writeEntry("Name_Pl1","");
   else
      config->writeEntry("Name_Pl1",playerName[0]);

   if(playerName[1]==i18n("Player 2"))
       config->writeEntry("Name_Pl2","");
   else
       config->writeEntry("Name_Pl2",playerName[1]);
  	
   tron->saveColors(config);
   config->writeEntry("BackgroundImage",bgPixURL.url());

   config->sync();
}

void KTron::configureOther()
{
   if(optionsDialog==0)
   {
      ExtOptions opts;
      opts.changeColor=tron->winnerColor();
      opts.blockAccelerator=tron->acceleratorBlocked();
      if(playerName[0]==i18n("Player 1"))
         opts.namePl1="";
      else
         opts.namePl1=playerName[0];

      if(playerName[1]==i18n("Player 2"))
         opts.namePl2="";
      else
         opts.namePl2=playerName[1];

      optionsDialog=new KTOptDlg(this,opts);

      connect(optionsDialog,SIGNAL(okClicked()),this,SLOT(takeOptions()));
   }

   optionsDialog->show();
   optionsDialog->raise();
}

void KTron::takeOptions()
{
   ExtOptions opts=optionsDialog->options();
   tron->enableWinnerColor(opts.changeColor);
   tron->setAcceleratorBlocked(opts.blockAccelerator);

   if(opts.namePl1=="")
      playerName[0]=i18n("Player 1");
   else
      playerName[0]=opts.namePl1;

   if(opts.namePl2=="")
      playerName[1]=i18n("Player 2");
   else
      playerName[1]=opts.namePl2;

   updateStatusbar();
}

void KTron::showWinner(Player winner)
{	
   if(tron->isComputer(Both))
   {
      return;
   }

   QString message;
   QString name[2];
   int winnerNr;
   int looserNr;
   switch(winner)
   {
      case One:
         winnerNr=0;
         looserNr=1;
         break;
      case Two:
         winnerNr=1;
         looserNr=0;
         break;
      default:
         return;
         break;
   }

   for(int i=0;i<2;i++)
   {
      Player player;
      player=(i==0)?One:Two;

      if(tron->isComputer(player))
      {
         name[i]=i18n("KTron");
      }
      else
      {
         name[i]=playerName[i];
      }
   }
   	
   message=i18n("%1 has won!").arg(name[winnerNr]);
   statusBar()->message(message,MESSAGE_TIME);
  	

   if(looserNr==0 && playerName[0]!=i18n("Player 1"))
   {
      message=i18n("%1 has won versus %2 with %3 : %4 points!");
      message=message.arg(name[winnerNr]).arg(name[looserNr]);
      message=message.arg(playerPoints[winnerNr]).arg(playerPoints[looserNr]);
   }
   else if(looserNr==1 && playerName[1]!=i18n("Player 2"))
   {
      message=i18n("%1 has won versus %2 with %3 : %4 points!");
      message=message.arg(name[winnerNr]).arg(name[looserNr]);
      message=message.arg(playerPoints[winnerNr]).arg(playerPoints[looserNr]);
   }
   else
   {
      message=i18n("%1 has won with %2 : %3 points!");
      message=message.arg(name[winnerNr]).arg(playerPoints[winnerNr]).arg(playerPoints[looserNr]);
   }
		
   KMessageBox::information(this, message, i18n("Winner"));

   tron->newGame();
}

void KTron::quit()
{
   saveSettings();
   kapp->quit();
}

void KTron::toggleComPl1()
{
     bool status=tron->isComputer(One);
     tron->setComputerplayer(One,status ? false : true);
     updateStatusbar();
}

void KTron::toggleComPl2()
{
     bool status=tron->isComputer(Two);
     tron->setComputerplayer(Two,status ? false : true);
     updateStatusbar();
}

void KTron::beginnerSkill()
{
   tron->setSkill(Easy);
}

void KTron::averageSkill()
{
   tron->setSkill(Medium);
}

void KTron::expertSkill()
{
   tron->setSkill(Hard);
}

void KTron::setStyle(int index)
{
   tron->setStyle((TronStyle)index);
}

void KTron::setSize(int index)
{
   switch(index)
   {
      case 0:
         tron->setRectSize(4);
         break;
      case 1:
         tron->setRectSize(7);
         break;
      case 2:
         tron->setRectSize(10);
         break;
      case 3:
         tron->setRectSize(13);
         break;
      case 4:
         tron->setRectSize(16);
         break;
   }
}

void KTron::setVelocity(int index)
{
    tron->setVelocity(index+1);
}

void KTron::configureKeys()
{
   KKeyDialog::configureKeys(accel);
}

void KTron::toggleStatusbar()
{
   KToggleAction* toggleAction;
   toggleAction=(KToggleAction *)actionCollection()->action(KStdAction::stdName(KStdAction::ShowStatusbar));

   if(toggleAction->isChecked())
      statusBar()->show();
   else
      statusBar()->hide();
}

void KTron::colorPl1()
{
   tron->changeColor(1);
}

void KTron::colorPl2()
{
   tron->changeColor(2);
}

void KTron::colorBackground()
{
   bool success=tron->changeColor(0);
   if(success)
      bgPixURL = QString::null;
}

void KTron::chooseBgPix()
{
    // Used to be:
    //QString name=KFilePreviewDialog::getOpenFileName(dir,"*",this);
    //When do we get back KFilePreviewDialog???
    KURL url = KFileDialog::getOpenURL(QString::null, "*", this);
    if(url.isEmpty())
	return;

    QString tmpFile;
    if (!KIO::NetAccess::download( url, tmpFile ) )
	return;

    QPixmap bgPix(tmpFile);

    if (!bgPix.isNull()) {
        QString name;
        tron->setBackgroundPix(bgPix);
        bgPixURL = url;
    } else {
	QString msg=i18n("Wasn't able to load wallpaper\n%1");
	msg=msg.arg(tmpFile);
	KMessageBox::sorry(this, msg);
    }

    KIO::NetAccess::removeTempFile( tmpFile );
}

void KTron::changeStatus(Player player)
{  	
   QString s;

   // if player=Nobody, then new game
   if(player==Nobody)
   {
      playerPoints[0]=playerPoints[1]=0;
      updateStatusbar();
   }
   else
   {
      if(player==One)
      {
         playerPoints[0]++;
      }
      else if(player==Two)
      {
         playerPoints[1]++;
      }
      else if(player==Both)
      {
         playerPoints[0]++;
         playerPoints[1]++;
      }

      updateStatusbar();

      if(playerPoints[0]>=9 && playerPoints[1] < playerPoints[0]-1)
      {
         showWinner(One);
      }
      else if(playerPoints[1]>=9 && playerPoints[0] < playerPoints[1]-1)
      {
         showWinner(Two);
      }
  }
}

void KTron::paletteChange(const QPalette &/*oldPalette*/)
{
   update();
   tron->updatePixmap();
   tron->update();
}



void KTron::readProperties(KConfig *config)
{
   KToggleAction* toggleAction;
	
   bool visible=config->readBoolEntry("Statusbar",true);
   toggleAction=(KToggleAction *)actionCollection()->action(KStdAction::stdName(KStdAction::ShowStatusbar));
   toggleAction->setChecked(visible);


   bool status=config->readBoolEntry("Computerplayer1",true);
   toggleAction=(KToggleAction *)actionCollection()->action("computer_player1");
   toggleAction->setChecked(status);
   tron->setComputerplayer(One,status);

   status=config->readBoolEntry("Computerplayer2",false);
   toggleAction=(KToggleAction *)actionCollection()->action("computer_player2");
   toggleAction->setChecked(status);
   tron->setComputerplayer(Two,status);

   Skill skill=(Skill)config->readNumEntry("Skill",(int)Medium);
   skillAction[skill]->setChecked(true);

   KSelectAction* selectAction=(KSelectAction*)actionCollection()->action("select_velocity");
   int velocity=config->readNumEntry("Velocity",5);
   selectAction->setCurrentItem(velocity-1);


   selectAction=(KSelectAction*)actionCollection()->action("select_style");
   TronStyle newStyle=(TronStyle)config->readNumEntry("Style",(int) OLine);
   selectAction->setCurrentItem(newStyle);

   int size=config->readNumEntry("RectSize",10);
   selectAction=(KSelectAction*)actionCollection()->action("select_size");
   switch(size)
   {
      case 4:
         selectAction->setCurrentItem(0);
         break;
      case 7:
         selectAction->setCurrentItem(1);
         break;
      case 10:
         selectAction->setCurrentItem(2);
         break;
      case 13:
         selectAction->setCurrentItem(3);
         break;
      case 16:
         selectAction->setCurrentItem(4);
         break;
   }

   tron->enableWinnerColor(config->readBoolEntry("ChangeWinnerColor",true));
   tron->setAcceleratorBlocked(config->readBoolEntry("AcceleratorBlocked",false));

   QString temp="";
   playerName[0]=config->readEntry("Name_Pl1",temp);
   if(playerName[0]=="")
      playerName[0]=i18n("Player 1");
   playerName[1]=config->readEntry("Name_Pl2",temp);
   if(playerName[1]=="")
      playerName[1]=i18n("Player 2");
   updateStatusbar();

   tron->restoreColors(config);
   readBackground(config);
}

void KTron::readBackground(KConfig *config)
{
   KURL url = config->readEntry("BackgroundImage");
   if(!url.isEmpty())
   {
      QString tmpFile;
      KIO::NetAccess::download(url, tmpFile);
      QPixmap pix(tmpFile);
      if(!pix.isNull())
      {
         tron->setBackgroundPix(pix);
         bgPixURL = url;
      }
      KIO::NetAccess::removeTempFile(tmpFile);
    }
}

void KTron::saveProperties(KConfig *config)
{
   KToggleAction* toggleAction;	
   toggleAction=(KToggleAction *)actionCollection()->action(KStdAction::stdName(KStdAction::ShowStatusbar));
   config->writeEntry("Statusbar",toggleAction->isChecked());

   config->writeEntry("Velocity",tron->getVelocity());
   config->writeEntry("Style",(int)tron->getStyle());
   config->writeEntry("RectSize",tron->getRectSize());
   config->writeEntry("Computerplayer1", tron->isComputer(One) );
   config->writeEntry("Computerplayer2", tron->isComputer(Two) );
   config->writeEntry("Skill",(int)tron->skill());
		
   config->writeEntry("ChangeWinnerColor",tron->winnerColor());
   config->writeEntry("AcceleratorBlocked",tron->acceleratorBlocked());
		
   if(playerName[0]==i18n("Player 1"))
      config->writeEntry("Name_Pl1","");
   else
      config->writeEntry("Name_Pl1",playerName[0]);

   if(playerName[1]==i18n("Player 2"))
      config->writeEntry("Name_Pl2","");
   else
      config->writeEntry("Name_Pl2",playerName[1]);
  		
  tron->saveColors(config);

  config->writeEntry("BackgroundImage",bgPixURL.url());
}

