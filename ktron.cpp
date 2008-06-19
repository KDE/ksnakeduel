/* ****************************************************************************
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

  *****************************************************************************/

#include "ktron.h"

#include <KConfigDialog>
#include <KLocale>
#include <KMessageBox>
#include <KAction>
#include <KActionCollection>
#include <KStandardGameAction>
#include <KApplication>
#include <KStatusBar>
#include <KToggleAction>
#include <KGameThemeSelector>
#include <KGameDifficulty>

// Settings
#include "settings.h"
#include "ui_general.h"
#include "ui_ai.h"
#include "ui_appearance.h"

#define ID_STATUS_BASE 40
#define MESSAGE_TIME 2000
#define WINNING_DIFF 5

//UI
class General : public QWidget, public Ui::General
{
public:
    General(QWidget *parent = 0)
        : QWidget(parent)
        {
            setupUi(this);
        }
};
class Ai : public QWidget, public Ui::Ai
{
public:
    Ai(QWidget *parent = 0)
        : QWidget(parent)
        {
            setupUi(this);
        }
};
class Appearance : public QWidget, public Ui::Appearance
{
public:
    Appearance(QWidget *parent = 0)
        : QWidget(parent)
        {
            setupUi(this);
        }
};

/**
 * Constuctor
 */ 
KTron::KTron(QWidget *parent) : KXmlGuiWindow(parent, KDE_DEFAULT_WINDOWFLAGS) {
  playerPoints[0]=playerPoints[1]=0;

  tron=new Tron(this);
  connect(tron,SIGNAL(gameEnds(Player)),SLOT(changeStatus(Player)));
  setCentralWidget(tron);
  tron->setMinimumSize(200,180);

  // create statusbar
  statusBar()->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE+1);
  statusBar()->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE+2);

  // We match up keyboard events ourselves in Tron::keyPressEvent()
  // We must disable the actions, otherwise we don't get the keyPressEvent's
  QAction *act;

  act = actionCollection()->addAction("Pl1Up");
  act->setText(i18n("Player 1 Up"));
  act->setShortcut(Qt::Key_R);
  act->setEnabled(false);

  act = actionCollection()->addAction("Pl1Down");
  act->setText(i18n("Player 1 Down"));
  act->setShortcut(Qt::Key_F);
  act->setObjectName("Pl1Down");
  act->setEnabled(false);

  act = actionCollection()->addAction("Pl1Right");;
  act->setText(i18n("Player 1 Right"));
  act->setShortcut(Qt::Key_G);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl1Left");
  act->setText(i18n("Player 1 Left"));
  act->setShortcut(Qt::Key_D);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl1Ac");
  act->setText(i18n("Player 1 Accelerator"));
  act->setShortcut(Qt::Key_A);
  act->setEnabled(false);
 
  act = actionCollection()->addAction("Pl2Up");
  act->setText(i18n("Player 2 Up"));
  act->setShortcut(Qt::Key_Up);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl2Down");
  act->setText(i18n("Player 2 Down"));
  act->setShortcut(Qt::Key_Down);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl2Right");
  act->setText(i18n("Player 2 Right"));
  act->setShortcut(Qt::Key_Right);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl2Left");
  act->setText(i18n("Player 2 Left"));
  act->setShortcut(Qt::Key_Left);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl2Ac");
  act->setText(i18n("Player 2 Accelerator"));
  act->setShortcut(Qt::Key_0);
  act->setEnabled(false);

  tron->setActionCollection(actionCollection());

  act = KStandardGameAction::pause(tron, SLOT(togglePause()), this);
  actionCollection()->addAction(act->objectName(), act);
  act = KStandardGameAction::gameNew(tron, SLOT( newGame() ), this);
  actionCollection()->addAction(act->objectName(), act);
  act = KStandardGameAction::quit(kapp, SLOT(quit()), this);
  actionCollection()->addAction(act->objectName(), act);
  act = KStandardAction::preferences(this, SLOT(showSettings()), this);
  actionCollection()->addAction(act->objectName(), act);

  //difficulty
  KGameDifficulty::init(this, tron, SLOT(loadSettings()));
  KGameDifficulty::addStandardLevel(KGameDifficulty::VeryEasy);
  KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
  KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
  KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
  KGameDifficulty::addStandardLevel(KGameDifficulty::VeryHard);
  int skill = Settings::difficulty();
  if (skill < (int)KGameDifficulty::VeryEasy || skill > (int)KGameDifficulty::VeryHard) {
    KGameDifficulty::setLevel(KGameDifficulty::Easy);
    Settings::setDifficulty((int) KGameDifficulty::Easy);
  }
  else {
    KGameDifficulty::setLevel((KGameDifficulty::standardLevel) (skill));
  }

  setupGUI( KXmlGuiWindow::Keys | StatusBar | Save | Create);
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
      name=i18n("Computer(%1)", (i+1));
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
  
  QString message=i18n("%1 has won!", winnerName);
  statusBar()->showMessage(message);

  message = i18n("%1 has won versus %2 with %3 : %4 points!", winnerName, loserName, playerPoints[winner], playerPoints[loser]);
  
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
  dialog->addPage(new General, i18n("General"), "package_settings");
  dialog->addPage(new Ai, i18n("A.I."), "personal");
  dialog->addPage(new Appearance, i18n("Appearance"), "preferences-desktop-theme-style");
  dialog->addPage(new KGameThemeSelector(dialog, Settings::self(), KGameThemeSelector::NewStuffDisableDownload), i18n("Theme"), "games-config-theme");
  connect(dialog, SIGNAL(settingsChanged(const QString &)), tron, SLOT(loadSettings()));
  connect(dialog, SIGNAL(settingsChanged(const QString &)), this, SLOT(loadSettings()));
  dialog->show();
}

/**
 * Close KTron
 */
void KTron::close() {
	Settings::self()->writeConfig();
}

void KTron::closeEvent(QCloseEvent *event)
{
    close();
    event->accept();
}

#include "ktron.moc"

