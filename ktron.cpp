/* ********************************************************************************
   This file is part of the game 'KTron'

  Copyright (C) 1998,1999 by Matthias Kiefer <matthias.kiefer@gmx.de>

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
#include <qmessagebox.h>
#include <kmenubar.h>
#include <kconfig.h>


#define ID_GAME_NEW 1
#define ID_GAME_PAUSE 2
#define ID_GAME_QUIT 3
#define ID_HELP_ABOUT 11
#define ID_HELP_CONTENTS 12
#define ID_VIEW_TOOLBAR 15
#define ID_VIEW_STATUSBAR 16
#define ID_OPT_KEYS 21
#define ID_OPT_OTHER 22
#define ID_OPT_SAVE 23
#define ID_OPT_WALLPAPER 24
#define ID_COLOR_BASE 30
#define ID_STATUS_BASE 40
#define ID_VELOCITY_BASE 50
#define ID_COMP_BASE 80
#define ID_SKILL_BASE 100
#define ID_STYLE_BASE 120
#define ID_SIZE_BASE 140


#define MESSAGE_TIME 2000

KTron::KTron(const char *name) : KTMainWindow(name)
{
  playerPoints[0]=playerPoints[1]=0;

  setMinimumSize(150,200);
  optionsDialog=0;

  tron=new Tron(this);
  connect(tron,SIGNAL(gameEnds(Player)),SLOT(changeStatus(Player)));
  setView(tron);

  accel=new KAccel(this,"Key Accel");

  accel->insertItem(i18n("Pause/Continue"),"Pause","P");
  accel->connectItem("Pause",tron,SLOT(togglePause()));
  accel->insertStdItem(KStdAccel::New,i18n("New Game"));
  accel->connectItem(KStdAccel::New,tron,SLOT(newGame()));
  accel->connectItem(KStdAccel::Quit,this,SLOT(quit()));

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

  // create menubar
  game = new QPopupMenu(this);
  game->insertItem(i18n("&Pause/Continue"),ID_GAME_PAUSE);
  accel->changeMenuAccel(game,ID_GAME_PAUSE,"Pause");
  game->insertItem(i18n("&New Game"),ID_GAME_NEW);
  accel->changeMenuAccel(game,ID_GAME_NEW,KStdAccel::New);
  game->insertSeparator();
  game->insertItem(i18n("E&xit"),ID_GAME_QUIT);
  accel->changeMenuAccel(game,ID_GAME_QUIT,KStdAccel::Quit);

  connect(game,SIGNAL(activated(int)),SLOT(menuCallback(int)));

  /* options-menu  */

  compPlayerMenu =new QPopupMenu(this);
  compPlayerMenu->setCheckable(true);
  compPlayerMenu->insertItem(i18n("Player &1"),ID_COMP_BASE);
  compPlayerMenu->insertItem(i18n("Player &2"),ID_COMP_BASE+1);
  compPlayerMenu->insertSeparator();
  compPlayerMenu->insertItem(i18n("Beginner"),ID_SKILL_BASE+Easy);
  compPlayerMenu->insertItem(i18n("Average"),ID_SKILL_BASE+Medium);
  compPlayerMenu->insertItem(i18n("Expert"),ID_SKILL_BASE+Hard);

  connect(compPlayerMenu,SIGNAL(activated(int)),SLOT(menuCallback(int)));


  velocity=new QPopupMenu(this);
  velocity->setCheckable(true);
  velocity->insertItem(i18n("&1 (slow)"),ID_VELOCITY_BASE+1);
  velocity->insertItem("&2",ID_VELOCITY_BASE+2);
  velocity->insertItem("&3",ID_VELOCITY_BASE+3);
  velocity->insertItem("&4",ID_VELOCITY_BASE+4);
  velocity->insertItem(i18n("&5 (default)"),ID_VELOCITY_BASE+5);
  velocity->insertItem("&6",ID_VELOCITY_BASE+6);
  velocity->insertItem("&7",ID_VELOCITY_BASE+7);
  velocity->insertItem("&8",ID_VELOCITY_BASE+8);
  velocity->insertItem(i18n("&9 (fast)"),ID_VELOCITY_BASE+9);

  connect(velocity,SIGNAL(activated(int)),SLOT(menuCallback(int)));

  styleMenu=new QPopupMenu(this);
  styleMenu->setCheckable(true);
  styleMenu->insertItem(i18n("&3D Line"),ID_STYLE_BASE+OLine);
  styleMenu->insertItem(i18n("3&D Rects"),ID_STYLE_BASE+ORect);
  styleMenu->insertItem(i18n("&Line"),ID_STYLE_BASE+Line);
  styleMenu->insertItem(i18n("&Circles"),ID_STYLE_BASE+Circle);

  connect(styleMenu,SIGNAL(activated(int)),SLOT(menuCallback(int)));


  QPopupMenu *colors=new QPopupMenu(this);
  colors->insertItem(i18n("&Drawing style"),styleMenu);
  colors->insertItem(i18n("Color Player &1..."),ID_COLOR_BASE+1);
  colors->insertItem(i18n("Color Player &2..."),ID_COLOR_BASE+2);
  colors->insertItem(i18n("&Backgroundcolor..."),ID_COLOR_BASE);
  colors->insertItem(i18n("B&ackground image..."),ID_OPT_WALLPAPER);

  connect(colors,SIGNAL(activated(int)),SLOT(menuCallback(int)));


  sizeMenu=new QPopupMenu(this);
  sizeMenu->setCheckable(true);
  sizeMenu->insertItem(i18n("&Very Small"),ID_SIZE_BASE+4);
  sizeMenu->insertItem(i18n("&Small"),ID_SIZE_BASE+7);
  sizeMenu->insertItem(i18n("&Medium"),ID_SIZE_BASE+10);
  sizeMenu->insertItem(i18n("&Large"),ID_SIZE_BASE+13);
  sizeMenu->insertItem(i18n("V&ery Large"),ID_SIZE_BASE+16);

  connect(sizeMenu,SIGNAL(activated(int)),SLOT(menuCallback(int)));


  options = new QPopupMenu(this);
  options->setCheckable(true);
  options->insertItem(i18n("Show &Toolbar"),ID_VIEW_TOOLBAR);
  options->insertItem(i18n("Show &Statusbar"),ID_VIEW_STATUSBAR);
  options->insertSeparator();
  options->insertItem(i18n("&Configure Key Bindings..."),ID_OPT_KEYS);
  options->insertItem(i18n("C&omputerplayer"),compPlayerMenu);
  options->insertItem(i18n("&Velocity"),velocity);
  options->insertItem(i18n("&Appearance"),colors);
  options->insertItem(i18n("Si&ze"),sizeMenu);
  options->insertItem(i18n("Other..."),ID_OPT_OTHER);
  options->insertSeparator();
  options->insertItem(i18n("S&ave Options"),ID_OPT_SAVE);

  connect(options,SIGNAL(activated(int)),SLOT(menuCallback(int)));

  QString about = i18n(
    "KTron version %1 \n\n"
    "(C) 1998-1999 by Matthias Kiefer\n"
    "email: matthias.kiefer@gmx.de\n")
    .arg(KTRON_VERSION);
  about += i18n(
    "\nParts of the algorithms for the computerplayer are from\n"
    "xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>\n\n"
    "This program is free software; you can redistribute\n"
    "it and/or modify it under the terms of the GNU General\n"
    "Public License as published by the Free Software\n"
    "Foundation; either version 2 of the License, or\n"
    "(at your option) any later version.");
  QPopupMenu *help = helpMenu(about);

  KMenuBar *menu=menuBar();
  menu->insertItem(i18n("&Game"), game );
  menu->insertItem(i18n("&Options"),options);
  menu->insertSeparator();
  menu->insertItem( i18n("&Help"), help );

  connect(menu,SIGNAL(moved(menuPosition)),SLOT(barPositionChanged()));

  // Toolbar

  KIconLoader *loader = KGlobal::iconLoader();

  KToolBar *toolbar = toolBar();

  // restartbutton
  toolbar->insertButton(loader->loadIcon("reload.xpm"),ID_GAME_NEW,
			true,i18n("New Game"));
  toolbar->insertSeparator();
  //helpbutton
  toolbar->insertButton(loader->loadIcon("help.xpm"),ID_HELP_CONTENTS,true,
			i18n("Help"));

  connect(toolbar, SIGNAL(clicked(int)), this, SLOT(menuCallback(int)));
  connect(toolbar,SIGNAL(moved(BarPosition)),SLOT(barPositionChanged()));

  	// Statusleiste erstellen

   KStatusBar *statusbar= statusBar();
   statusbar->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE+1);
   statusbar->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE+2);

   connect(statusbar,SIGNAL(pressed(int)),this,SLOT(statusbarClicked(int)));


    // read config
  	KConfig *config=kapp->config();
  	
  	config->setGroup("Window");

  	int barPos = config->readNumEntry("ToolbarPos",(int)(KToolBar::Top));
  	toolBar()->setBarPos((KToolBar::BarPosition)barPos);

  	bool visible=config->readBoolEntry("Toolbar",true);
  	options->setItemChecked(ID_VIEW_TOOLBAR,visible);
 		if(visible)
	 		enableToolBar(KToolBar::Show);
  	else
			enableToolBar(KToolBar::Hide);
	
  	visible=config->readBoolEntry("Statusbar",true);
  	options->setItemChecked(ID_VIEW_STATUSBAR,visible);
   	if(visible)
	  	enableStatusBar(KStatusBar::Show);
   	else
   		enableStatusBar(KStatusBar::Hide);

   	int width=config->readNumEntry("Width",400);
   	int height=config->readNumEntry("Height",300);
   	resize(width,height);


   config->setGroup("Game");
   bool status=config->readBoolEntry("Computerplayer1",true);
   compPlayerMenu->setItemChecked(ID_COMP_BASE,status);
   tron->setComputerplayer(One,status);
   status=config->readBoolEntry("Computerplayer2",false);
   compPlayerMenu->setItemChecked(ID_COMP_BASE+1,status);
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
   compPlayerMenu->setItemChecked(ID_SKILL_BASE+skill,true);
   tron->setSkill(skill);

   int velocity=config->readNumEntry("Velocity",5);
   tron->setVelocity(velocity);
   updateVelocityMenu(ID_VELOCITY_BASE+velocity);

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

   tron->setStyle(newStyle);
   for(int i=ID_STYLE_BASE;i<ID_STYLE_BASE+4;i++)
	  styleMenu->setItemChecked(i,false);
   styleMenu->setItemChecked(ID_STYLE_BASE+newStyle,true);

 	int size=config->readNumEntry("RectSize",10);
  	tron->setRectSize(size);
   sizeMenu->setItemChecked(ID_SIZE_BASE+size,true);

   tron->enableWinnerColor(config->readBoolEntry("ChangeWinnerColor",true));
   tron->setAcceleratorBlocked(config->readBoolEntry("AcceleratorBlocked",false));

   tron->restoreColors(config);
   QString name=config->readEntry("BackgroundImage",0);
   if(!name.isNull())
   {
      QPixmap pix(name);
      if(!pix.isNull())
      {
         tron->setBackgroundPix(pix);
         bgPixName=name;
      }
   }
}

// Destruktor

KTron::~KTron()
{
}


void KTron::updateVelocityMenu(int id)
{
  for(int i=1;i<10;i++)
    velocity->setItemChecked(ID_VELOCITY_BASE+i,false);

  velocity->setItemChecked(id,true);
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
  	{
  		KConfigGroupSaver saver(config,"Window");

  		bool status=options->isItemChecked(ID_VIEW_TOOLBAR);
  		config->writeEntry("Toolbar",status);
  		status=options->isItemChecked(ID_VIEW_STATUSBAR);
  		config->writeEntry("Statusbar",status);

  		config->writeEntry("Width",width());
  		config->writeEntry("Height",height());
  	}
  	{
  	   KConfigGroupSaver saver(config,"Game");
  		config->writeEntry("Velocity",tron->getVelocity());
  		config->writeEntry("Style",(int)tron->getStyle());
  		config->writeEntry("RectSize",tron->getRectSize());

  		config->writeEntry("Computerplayer1"
		     ,compPlayerMenu->isItemChecked(ID_COMP_BASE));
  		config->writeEntry("Computerplayer2"
		     ,compPlayerMenu->isItemChecked(ID_COMP_BASE+1));
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
  		config->writeEntry("BackgroundImage",bgPixName);
	}
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

      optionsDialog=new KTOptDlg(opts);

      connect(optionsDialog,SIGNAL(buttonPressed()),this,SLOT(takeOptions()));
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
     //tron->newGame();
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
		
  QMessageBox::information(this, i18n("KTron - Winner"), message, i18n("OK"));
  
  tron->newGame();
}

void KTron::quit()
{
   kapp->quit();
}

void KTron::menuCallback(int id)
{
  	switch(id)
  	{
  		case ID_GAME_PAUSE:
      	tron->togglePause();
      	break;
    	case ID_GAME_NEW:
      	tron->newGame();
      	break;
    	case ID_GAME_QUIT:
      	quit();
      	break;

    	case ID_COMP_BASE:
      {
			bool status=compPlayerMenu->isItemChecked(id);
			tron->setComputerplayer(One,status ? false : true);
			compPlayerMenu->setItemChecked(id,status ? false : true);
			updateStatusbar();
			break;
      }
    	case ID_COMP_BASE+1:
      {
			bool status=compPlayerMenu->isItemChecked(id);
			tron->setComputerplayer(Two,status ? false : true);
			compPlayerMenu->setItemChecked(id,status ? false : true);
			updateStatusbar();
			break;
      }
      case ID_SKILL_BASE+Easy:
      case ID_SKILL_BASE+Medium:
      case ID_SKILL_BASE+Hard:
      {
         for(int i=0;i<3;i++)
         {
             compPlayerMenu->setItemChecked(ID_SKILL_BASE+i,false);
         }
			compPlayerMenu->setItemChecked(id,true);
			tron->setSkill((Skill)(id-ID_SKILL_BASE));
         break;
      }
    	case ID_STYLE_BASE+OLine:
    	case ID_STYLE_BASE+ORect:
    	case ID_STYLE_BASE+Line:
    	case ID_STYLE_BASE+Circle:
      {
			int i;
	  		for(i=ID_STYLE_BASE;i<ID_STYLE_BASE+4;i++)
	      	styleMenu->setItemChecked(i,false);
	
	    	styleMenu->setItemChecked(id,true);

	    	tron->setStyle((TronStyle)(id-ID_STYLE_BASE));
	
			break;
      }

    	case ID_SIZE_BASE+4:
    	case ID_SIZE_BASE+7:
    	case ID_SIZE_BASE+10:
    	case ID_SIZE_BASE+13:
    	case ID_SIZE_BASE+16:
      {
			for(int i=ID_SIZE_BASE+4;i<ID_SIZE_BASE+17;i++)
	  		   sizeMenu->setItemChecked(i,false);
	
			sizeMenu->setItemChecked(id,true);

			tron->setRectSize(id-ID_SIZE_BASE);
			break;
      }
    	case ID_OPT_KEYS:
    	   if( KKeyDialog::configureKeys(accel) )
    	   {
  				accel->changeMenuAccel(game,ID_GAME_PAUSE,"Pause");
  			}
      	break;
      case ID_OPT_OTHER:
         configureOther();
         break;
    	case ID_OPT_SAVE:
      	saveSettings();
      	break;
    	case ID_VIEW_TOOLBAR:
      {
			enableToolBar(); // toggles toolbar
			bool status=options->isItemChecked(ID_VIEW_TOOLBAR);
			options->setItemChecked(ID_VIEW_TOOLBAR, status?false:true);
			break;
      }
    	case ID_VIEW_STATUSBAR:
      {
			enableStatusBar(); // toggles statusbar
			bool status=options->isItemChecked(ID_VIEW_STATUSBAR);
			options->setItemChecked(ID_VIEW_STATUSBAR, status?false:true);
			break;
      }
    	case ID_COLOR_BASE:
    	{
    	   bool success=tron->changeColor(0);
    	   if(success)
    	      bgPixName=QString::null;
    	   break;
    	}
    	case ID_COLOR_BASE+1:
    	case ID_COLOR_BASE+2:
      	tron->changeColor(id-ID_COLOR_BASE);
      	break;

    	case ID_VELOCITY_BASE+1:
    	case ID_VELOCITY_BASE+2:
    	case ID_VELOCITY_BASE+3:
    	case ID_VELOCITY_BASE+4:
    	case ID_VELOCITY_BASE+5:
    	case ID_VELOCITY_BASE+6:
    	case ID_VELOCITY_BASE+7:
    	case ID_VELOCITY_BASE+8:
    	case ID_VELOCITY_BASE+9:
      	tron->setVelocity(id-ID_VELOCITY_BASE);
      	updateVelocityMenu(id);
      	break;
      case ID_HELP_CONTENTS:
         kapp->invokeHTMLHelp("","");
         break;
      case ID_OPT_WALLPAPER:
         chooseBgPix();
         break;
	}
}

void KTron::chooseBgPix()
{
  //const QString dir=KGlobal::dirs()->getResourceDirs("wallpaper").first();
  const QString dir="";

  // Used to be:
  //QString name=KFilePreviewDialog::getOpenFileName(dir,"*",this);
  //When do we get back KFilePreviewDialog???
  QString name=KFileDialog::getOpenFileName(dir, "*", this);
  if(name.isNull())
     return;

  QPixmap bgPix(name);

  if(!bgPix.isNull())
  {
     bgPixName=name;
     tron->setBackgroundPix(bgPix);
  }
  else
  {
     QString msg=i18n("Wasn't able to load wallpaper\n%1");
     msg=msg.arg(name);
     QMessageBox::information(this, kapp->caption(), msg, i18n("OK"));
  }
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

void KTron::statusbarClicked(int )
{
  //debug("click");
}


void KTron::paletteChange(const QPalette &/*oldPalette*/)
{
   update();
   tron->updatePixmap();
   tron->update();
}


void KTron::barPositionChanged()
{
  KConfig *config=kapp->config();
  KConfigGroupSaver saver(config,"Window");
  config->writeEntry("ToolbarPos",(int)(toolBar()->barPos()));
}



void KTron::readProperties(KConfig *config)
{
  int i;

  bool visible=true;
  visible=config->readBoolEntry("Toolbar",true);
  options->setItemChecked(ID_VIEW_TOOLBAR,visible);
    	
  visible=config->readBoolEntry("Statusbar",true);
  options->setItemChecked(ID_VIEW_STATUSBAR,visible);

  bool status=false;
  status=config->readBoolEntry("Computerplayer1",false);
  compPlayerMenu->setItemChecked(ID_COMP_BASE,status);
  tron->setComputerplayer(One,status);
  status=config->readBoolEntry("Computerplayer2",false);
  compPlayerMenu->setItemChecked(ID_COMP_BASE+1,status);
  tron->setComputerplayer(Two,status);

   Skill skill=Medium;
   skill=(Skill)config->readNumEntry("Skill",(int)Medium);
   for(i=0;i<3;i++)
   {
      compPlayerMenu->setItemChecked(ID_SKILL_BASE+i,false);
   }
   compPlayerMenu->setItemChecked(ID_SKILL_BASE+skill,true);
   tron->setSkill(skill);

  int velocity=5;
  velocity=config->readNumEntry("Velocity",5);
  tron->setVelocity(velocity);
  updateVelocityMenu(ID_VELOCITY_BASE+velocity);

  TronStyle newStyle=OLine;
  newStyle=(TronStyle)config->readNumEntry("Style",(int) OLine);
  tron->setStyle(newStyle);
  for(i=ID_STYLE_BASE;i<ID_STYLE_BASE+4;i++)
    styleMenu->setItemChecked(i,false);

  styleMenu->setItemChecked(ID_STYLE_BASE+newStyle,true);


  	int size=10;
        size=config->readNumEntry("RectSize",10);
  	for(i=ID_SIZE_BASE+4;i<ID_SIZE_BASE+17;i+=3)
   	sizeMenu->setItemChecked(i,false);
  	tron->setRectSize(size);
  	sizeMenu->setItemChecked(size+ID_SIZE_BASE,true);

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

   QString name=config->readEntry("BackgroundImage",0);
   if(!name.isNull())
   {
      QPixmap pix(name);
      if(!pix.isNull())
      {
         tron->setBackgroundPix(pix);
         bgPixName=name;
      }
   }
}


void KTron::saveProperties(KConfig *config)
{
  bool status=options->isItemChecked(ID_VIEW_TOOLBAR);
  config->writeEntry("Toolbar",status);
  status=options->isItemChecked(ID_VIEW_STATUSBAR);
  config->writeEntry("Statusbar",status);

  config->writeEntry("Velocity",tron->getVelocity());
  config->writeEntry("Style",(int)tron->getStyle());
  config->writeEntry("RectSize",tron->getRectSize());
  config->writeEntry("Computerplayer1"
		     ,compPlayerMenu->isItemChecked(ID_COMP_BASE));
  config->writeEntry("Computerplayer2"
		     ,compPlayerMenu->isItemChecked(ID_COMP_BASE+1));
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

  config->writeEntry("BackgroundImage",bgPixName);
}



































