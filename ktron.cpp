/* ****************************************************************************
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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  *****************************************************************************/

#include "ktron.h"

#include <klocale.h>
#include <kkeydialog.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kstdgameaction.h>
#include <kapplication.h>
#include <kstatusbar.h>

// Settings
#include "general.h"
#include "ai.h"
#include "appearance.h"
#include <kiconloader.h>
#include <qvbox.h>
#include <kautoconfig.h>

#define ID_STATUS_BASE 40
#define MESSAGE_TIME 2000
#define WINNING_DIFF 5

/**
 * Constuctor
 */ 
KTron::KTron() : settings(0) {
  playerPoints[0]=playerPoints[1]=0;

  tron=new Tron(this);
  connect(tron,SIGNAL(gameEnds(Player)),SLOT(changeStatus(Player)));
  setCentralWidget(tron);
  tron->setMinimumSize(200,180);

  // create statusbar
  statusBar()->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE+1);
  statusBar()->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE+2);

  actionCollection()->setAutoConnectShortcuts(false);
  (void)new KAction(i18n("Player 1 Up"), Key_R, 0, 0, actionCollection(), "Pl1Up");
  (void)new KAction(i18n("Player 1 Down"), Key_F, 0, 0, actionCollection(), "Pl1Down");
  (void)new KAction(i18n("Player 1 Right"), Key_G, 0, 0, actionCollection(), "Pl1Right");
  (void)new KAction(i18n("Player 1 Left"), Key_D, 0, 0, actionCollection(), "Pl1Left");
  (void)new KAction(i18n("Player 1 Accelerator"), Key_A, 0, 0, actionCollection(), "Pl1Ac");
 
  (void)new KAction(i18n("Player 2 Up"), Key_Up, 0, 0, actionCollection(), "Pl2Up");
  (void)new KAction(i18n("Player 2 Down"), Key_Down, 0, 0, actionCollection(), "Pl2Down");
  (void)new KAction(i18n("Player 2 Right"), Key_Right, 0, 0, actionCollection(), "Pl2Right");
  (void)new KAction(i18n("Player 2 Left"), Key_Left, 0, 0, actionCollection(), "Pl2Left");
  (void)new KAction(i18n("Player 2 Accelerator"), Key_0, 0, 0, actionCollection(), "Pl2Ac");

  actionCollection()->setAutoConnectShortcuts(true);

  tron->setActionCollection(actionCollection());

  KStdGameAction::pause(tron, SLOT(togglePause()), actionCollection());
  KStdGameAction::gameNew( tron, SLOT( newGame() ), actionCollection() );
  KStdGameAction::quit(this, SLOT( close() ), actionCollection());

  KStdAction::keyBindings(this, SLOT(configureKeys()), actionCollection());
   
  KStdAction::preferences(this, SLOT(showSettings()), actionCollection());
  createStandardStatusBarAction();

  createGUI();
  resize(400,300);
  setAutoSaveSettings();
  readSettings();
}

void KTron::readSettings() {
   KConfig *config=kapp->config();
   config->setGroup("Game");
   playerName[0]=config->readEntry("Name_Pl1");
   if ( playerName[0].isEmpty() )
       playerName[0] = i18n("Player 1");
   playerName[1]=config->readEntry("Name_Pl2");
   if ( playerName[1].isEmpty() )
       playerName[1] = i18n("Player 2");
   
   updateStatusbar();
}

// Destructor
KTron::~KTron(){
  delete tron;
}

void KTron::updateStatusbar(){
  for(int i=0;i<2;i++){
    Player player;
    player=(i==0?One:Two);

    QString name;
    if(tron->isComputer(Both))
      name=i18n("Computer(%1)").arg(i+1);
    else if(tron->isComputer(player))
      name=i18n("Computer");
    else
      name=playerName[i];
    QString string = QString("%1: %2").arg(name).arg(playerPoints[i]);
    statusBar()->changeItem(string,ID_STATUS_BASE+i+1);
  }
}

void KTron::changeStatus(Player player) {
  // if player=Nobody, then new game
  if(player==Nobody){
    playerPoints[0]=playerPoints[1]=0;
    updateStatusbar();
    return;
  }
  
  if(player==One)
    playerPoints[0]++;
  else if(player==Two)
    playerPoints[1]++;
  else if(player==Both){
    playerPoints[0]++;
    playerPoints[1]++;
  }

  updateStatusbar();

  if(playerPoints[0]>=WINNING_DIFF && playerPoints[1] < playerPoints[0]-1)
    showWinner(One);
  else if(playerPoints[1]>=WINNING_DIFF && playerPoints[0] < playerPoints[1]-1)
    showWinner(Two);
}

void KTron::showWinner(Player winner){
  if(tron->isComputer(Both) || (winner != One && winner != Two))
    return;

  QString looserName = i18n("KTron");
  int looser = Two;
  if(winner == Two)
    looser = One;
  if(!tron->isComputer(((Player)looser)))
    looserName = playerName[looser];
  
  QString winnerName = i18n("KTron");
  if(!tron->isComputer(winner))
    winnerName = playerName[winner];
  
  QString message=i18n("%1 has won!").arg(winnerName);
  statusBar()->message(message,MESSAGE_TIME);

  message = i18n("%1 has won versus %2 with %3 : %4 points!");
      message=message.arg(winnerName).arg(looserName);
      message=message.arg(playerPoints[winner]).arg(playerPoints[looser]);
  
  KMessageBox::information(this, message, i18n("Winner"));
  tron->newGame();
}

void KTron::paletteChange(const QPalette &/*oldPalette*/){
   update();
   tron->updatePixmap();
   tron->update();
}

/**
 * Bring up the key configuration dialog.
 */ 
void KTron::configureKeys(){
  KKeyDialog::configure(actionCollection(), this);
}

/**
 * Show Settings dialog.
 */
void KTron::showSettings(){
  settings = new KDialogBase(KDialogBase::IconList, i18n("Configure"), KDialogBase::Default | KDialogBase::Ok | KDialogBase::Apply | KDialogBase::Cancel , KDialogBase::Ok, this, "SettingsDialog", false, WDestructiveClose ); 
  KAutoConfig *kautoconfig = new KAutoConfig(settings, "KAutoConfig");
  //KAutoConfig_KURLRequester *r = new KAutoConfig_KURLRequester();
  //kautoconfig->addKAutoConfigWidget("KURLRequester", r);
	  
  connect(settings, SIGNAL(okClicked()), kautoconfig, SLOT(saveSettings()));
  connect(settings, SIGNAL(okClicked()), this, SLOT(closeSettings()));
  connect(settings, SIGNAL(applyClicked()), kautoconfig, SLOT(saveSettings()));
  connect(settings, SIGNAL(defaultClicked()), kautoconfig, SLOT(resetSettings()));

  QVBox *frame = settings->addVBoxPage(i18n("General"),i18n("General"), SmallIcon("package_settings", 32));
  General *general = new General(frame, "General");
  kautoconfig->addWidget(general, "Game");

  frame = settings->addVBoxPage(i18n("Ai"),i18n("Ai"), SmallIcon("package_system", 32));
  Ai *ai = new Ai(frame, "Ai");
  kautoconfig->addWidget(ai, "Game");

  frame = settings->addVBoxPage(i18n("Appearance"),i18n("Appearance"), SmallIcon("style", 32));
  Appearance *appearance = new Appearance(frame, "Appearance");
  kautoconfig->addWidget(appearance, "Game");
  
  kautoconfig->retrieveSettings();
  settings->show();
	
  connect(kautoconfig, SIGNAL(settingsChanged()), tron, SLOT(loadSettings()));
  connect(kautoconfig, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void KTron::closeSettings(){
  settings->close(true);
}

#include "ktron.moc"

