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

#include <kconfigdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kstdgameaction.h>
#include <kapplication.h>
#include <kstatusbar.h>

// Settings
#include "settings.h"
#include "general.h"
#include "ai.h"
#include "appearance.h"

#define ID_STATUS_BASE 40
#define MESSAGE_TIME 2000
#define WINNING_DIFF 5

/**
 * Constuctor
 */ 
KTron::KTron(QWidget *parent, const char *name) : KMainWindow(parent, name) {
  playerPoints[0]=playerPoints[1]=0;

  tron=new Tron(this, "Tron");
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

  KStdAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), 
actionCollection());
   
  KStdAction::preferences(this, SLOT(showSettings()), actionCollection());
  createStandardStatusBarAction();

  createGUI();
  resize(400,300);
  setAutoSaveSettings();
  loadSettings();
}

void KTron::loadSettings() {
   playerName[0]=Settings::namePlayer1();
   if ( playerName[0].isEmpty() )
       playerName[0] = i18n("Player 1");
   playerName[1]=Settings::namePlayer2();
   if ( playerName[1].isEmpty() )
       playerName[1] = i18n("Player 2");
   
   updateStatusbar();
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

  QString loserName = i18n("KTron");
  int loser = Two;
  if(winner == Two)
    loser = One;
  if(!tron->isComputer(((Player)loser)))
    loserName = playerName[loser];
  
  QString winnerName = i18n("KTron");
  if(!tron->isComputer(winner))
    winnerName = playerName[winner];
  
  QString message=i18n("%1 has won!").arg(winnerName);
  statusBar()->message(message,MESSAGE_TIME);

  message = i18n("%1 has won versus %2 with %3 : %4 points!");
      message=message.arg(winnerName).arg(loserName);
      message=message.arg(playerPoints[winner]).arg(playerPoints[loser]);
  
  KMessageBox::information(this, message, i18n("Winner"));
  tron->newGame();
}

void KTron::paletteChange(const QPalette &/*oldPalette*/){
   update();
   tron->updatePixmap();
   tron->update();
}

/**
 * Show Settings dialog.
 */
void KTron::showSettings(){
  if(KConfigDialog::showDialog("settings"))
    return;
  
  KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
  dialog->addPage(new General(0, "General"), i18n("General"), "package_settings");
  dialog->addPage(new Ai(0, "Ai"), i18n("Ai"), "package_system");
  dialog->addPage(new Appearance(0, "Appearance"), i18n("Appearance"), "style");
  connect(dialog, SIGNAL(settingsChanged()), tron, SLOT(loadSettings()));
  connect(dialog, SIGNAL(settingsChanged()), this, SLOT(loadSettings()));
  dialog->show();
}

#include "ktron.moc"

