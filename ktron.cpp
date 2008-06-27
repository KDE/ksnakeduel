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
#include "renderer.h"

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

/**
 * Constuctor
 */ 
KTron::KTron(QWidget *parent) : KXmlGuiWindow(parent, KDE_DEFAULT_WINDOWFLAGS) {
  //playerPoints[0]=playerPoints[1]=0;

  tron=new Tron(this);
  connect(tron,SIGNAL(gameEnds(KTronEnum::Player)),SLOT(changeStatus(KTronEnum::Player)));
  connect(tron,SIGNAL(updatedScore()),SLOT(updateScore()));
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
  act->setShortcut(Qt::Key_Up);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl1Down");
  act->setText(i18n("Player 1 Down"));
  act->setShortcut(Qt::Key_Down);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl1Right");
  act->setText(i18n("Player 1 Right"));
  act->setShortcut(Qt::Key_Right);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl1Left");
  act->setText(i18n("Player 1 Left"));
  act->setShortcut(Qt::Key_Left);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl1Ac");
  act->setText(i18n("Player 1 Accelerator"));
  act->setShortcut(Qt::Key_0);
  act->setEnabled(false);

  act = actionCollection()->addAction("Pl2Up");
  act->setText(i18n("Player 2 Up"));
  act->setShortcut(Qt::Key_R);
  act->setEnabled(false);

  act = actionCollection()->addAction("Pl2Down");
  act->setText(i18n("Player 2 Down"));
  act->setShortcut(Qt::Key_F);
  act->setEnabled(false);

  act = actionCollection()->addAction("Pl2Right");;
  act->setText(i18n("Player 2 Right"));
  act->setShortcut(Qt::Key_G);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl2Left");
  act->setText(i18n("Player 2 Left"));
  act->setShortcut(Qt::Key_D);
  act->setEnabled(false);
  
  act = actionCollection()->addAction("Pl2Ac");
  act->setText(i18n("Player 2 Accelerator"));
  act->setShortcut(Qt::Key_A);
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
	if (!Renderer::self()->loadTheme(Settings::theme()))
    {
		KMessageBox::error(this, i18n("Failed to load \"%1\" theme. Please check your installation.", Settings::theme()));
		return;
	}

	playerName[0]=Settings::namePlayer1();
	if ( playerName[0].isEmpty() )
		playerName[0] = i18n("Player 1");
	playerName[1]=Settings::namePlayer2();
	if ( playerName[1].isEmpty() )
		playerName[1] = i18n("Player 2");
	
	updateStatusbar();
}

void KTron::updateStatusbar(){
  if (Settings::gameType() == Settings::EnumGameType::Snake)
  {
    QString string = QString("%1: %2").arg(playerName[0]).arg(tron->players[0].score);
    statusBar()->changeItem(string,ID_STATUS_BASE+1);
    statusBar()->changeItem(QString(),ID_STATUS_BASE+2);
  }
  else
  {
    for(int i=0;i<2;i++){
      KTronEnum::Player player;
      player=(i==0 ? KTronEnum::One : KTronEnum::Two);

      QString name;
      if(tron->isComputer(KTronEnum::Both))
        name=i18n("Computer(%1)", (i+1));
      else if(tron->isComputer(player))
        name=i18n("Computer");
      else
        name=playerName[i];
      QString string = QString("%1: %2").arg(name).arg(tron->players[i].score);
      statusBar()->changeItem(string,ID_STATUS_BASE+i+1);
    }
  }
}

void KTron::updateScore()
{
	updateStatusbar();
}

void KTron::changeStatus(KTronEnum::Player player) {
  // if player=Nobody, then new game
  if(player==KTronEnum::Nobody){
    //playerPoints[0]=playerPoints[1]=0;
    updateStatusbar();
    return;
  }
  
  //if(player==KTronEnum::One)
  //  playerPoints[0]++;
  //else if(player==KTronEnum::Two)
  //  playerPoints[1]++;
  //else if(player==KTronEnum::Both){
  //  playerPoints[0]++;
  //  playerPoints[1]++;
  //}

  updateStatusbar();

  if (Settings::gameType() != Settings::EnumGameType::Snake)
  {
    if(tron->players[0].score >= WINNING_DIFF && tron->players[1].score < tron->players[0].score - 1)
      showWinner(KTronEnum::One);
    else if(tron->players[1].score >= WINNING_DIFF && tron->players[0].score < tron->players[1].score - 1)
      showWinner(KTronEnum::Two);
  }
}

void KTron::showWinner(KTronEnum::Player winner){
  if(tron->isComputer(KTronEnum::Both) || (winner != KTronEnum::One && winner != KTronEnum::Two))
    return;

  QString loserName = i18n("KTron");
  KTronEnum::Player loser = KTronEnum::Two;
  if(winner == KTronEnum::Two)
    loser = KTronEnum::One;
  if(!tron->isComputer(loser))
    loserName = playerName[loser];
  
  QString winnerName = i18n("KTron");
  if(!tron->isComputer(winner))
    winnerName = playerName[winner];
  
  QString message=i18n("%1 has won!", winnerName);
  statusBar()->showMessage(message);

  message = i18n("%1 has won versus %2 with %3 : %4 points!", winnerName, loserName, tron->players[winner].score, tron->players[loser].score);

  
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

