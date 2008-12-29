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
#include <KScoreDialog>
#include <KStatusBar>
#include <KGameThemeSelector>
#include <KGameDifficulty>
#include <KShortcutsDialog>
#include <KToggleAction>

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

	tron = new Tron(this);
	connect(tron,SIGNAL(gameEnds(KTronEnum::Player)),SLOT(changeStatus(KTronEnum::Player)));
	connect(tron,SIGNAL(updatedScore()),SLOT(updateScore()));
	setCentralWidget(tron);
	tron->setMinimumSize(200,180);

	// create statusbar
	statusBar()->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE + 1);
	statusBar()->insertItem("abcdefghijklmnopqrst: 0  ",ID_STATUS_BASE + 2);

	// We match up keyboard events ourselves in Tron::keyPressEvent()
	// We must disable the actions, otherwise we don't get the keyPressEvent's
	
	player0Up = actionCollection()->addAction("Pl1Up");
	player0Up->setText(i18n("Player 1 Up"));
	player0Up->setShortcut(Qt::Key_Up);
	connect(player0Up, SIGNAL(triggered(bool)), SLOT(triggerKey0Up(bool)));
	addAction(player0Up);
	
	player0Down = actionCollection()->addAction("Pl1Down");
	player0Down->setText(i18n("Player 1 Down"));
	player0Down->setShortcut(Qt::Key_Down);
	connect(player0Down, SIGNAL(triggered(bool)), SLOT(triggerKey0Down(bool)));
	addAction(player0Down);
	
	player0Right = actionCollection()->addAction("Pl1Right");
	player0Right->setText(i18n("Player 1 Right"));
	player0Right->setShortcut(Qt::Key_Right);
	connect(player0Right, SIGNAL(triggered(bool)), SLOT(triggerKey0Right(bool)));
	addAction(player0Right);
	
	player0Left = actionCollection()->addAction("Pl1Left");
	player0Left->setText(i18n("Player 1 Left"));
	player0Left->setShortcut(Qt::Key_Left);
	connect(player0Left, SIGNAL(triggered(bool)), SLOT(triggerKey0Left(bool)));
	addAction(player0Left);
	
	player0Accelerate = actionCollection()->addAction("Pl1Ac");
	player0Left->setText(i18n("Player 1 Accelerator"));
	player0Accelerate->setShortcut(Qt::Key_0);
	player0Accelerate->setEnabled(false); // Alternate handling, because of up/down events
	addAction(player0Accelerate);

	player1Up = actionCollection()->addAction("Pl2Up");
	player1Up->setText(i18n("Player 2 Up"));
	player1Up->setShortcut(Qt::Key_R);
	connect(player1Up, SIGNAL(triggered(bool)), SLOT(triggerKey1Up(bool)));
	addAction(player1Up);

	player1Down = actionCollection()->addAction("Pl2Down");
	player1Down->setText(i18n("Player 2 Down"));
	player1Down->setShortcut(Qt::Key_F);
	connect(player1Down, SIGNAL(triggered(bool)), SLOT(triggerKey1Down(bool)));
	addAction(player1Down);

	player1Right = actionCollection()->addAction("Pl2Right");;
	player1Right->setText(i18n("Player 2 Right"));
	player1Right->setShortcut(Qt::Key_G);
	connect(player1Right, SIGNAL(triggered(bool)), SLOT(triggerKey1Right(bool)));
	addAction(player1Right);
	
	player1Left = actionCollection()->addAction("Pl2Left");
	player1Left->setText(i18n("Player 2 Left"));
	player1Left->setShortcut(Qt::Key_D);
	connect(player1Left, SIGNAL(triggered(bool)), SLOT(triggerKey1Left(bool)));
	addAction(player1Left);
	
	player1Accelerate = actionCollection()->addAction("Pl2Ac");
	player1Left->setText(i18n("Player 2 Accelerator"));
	player1Accelerate->setShortcut(Qt::Key_A);
	player1Accelerate->setEnabled(false); // Alternate handling, because of up/down events
	addAction(player1Accelerate);

	// Pause
	KStandardGameAction::pause(tron, SLOT(togglePause()), actionCollection());
	// New
	KStandardGameAction::gameNew(tron, SLOT( newGame() ), actionCollection());
	// Quit
	KStandardGameAction::quit(kapp, SLOT(quit()), actionCollection());
	// Settings
	KStandardAction::preferences(this, SLOT(showSettings()), actionCollection());
	// Configure keys
	KStandardAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
	// Highscores
	KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());

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

KTron::~KTron()
{
	delete tron;
}

void KTron::loadSettings() {
	if (!Renderer::self()->loadTheme(Settings::theme()))
    {
		KMessageBox::error(this, i18n("Failed to load \"%1\" theme. Please check your installation.", Settings::theme()));
		return;
	}
	
	Settings::setDifficulty((int) KGameDifficulty::level());

	playerName[0]=Settings::namePlayer1();
	if ( playerName[0].isEmpty() )
		playerName[0] = i18n("Player 1");
	playerName[1]=Settings::namePlayer2();
	if ( playerName[1].isEmpty() )
		playerName[1] = i18n("Player 2");
	
	updateStatusbar();
}

void KTron::updateStatusbar() {
	KTronEnum::Player winner = getWinner();
	
	QString message;
	if (!tron->running() && (winner == KTronEnum::One || winner == KTronEnum::Two)) {
		QString winnerName = i18n("KTron");
		if (!tron->isComputer(winner)) {
			winnerName = playerName[winner];
		}
		
		message = i18n("%1 has won!", winnerName);
	}
	else if (tron->paused()) {
		message = i18n("Game paused");
	}
	else {
		message = QString("");
	}
	
	statusBar()->showMessage(message);

	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		QString string = QString("%1: %2").arg(playerName[0]).arg(tron->getPlayer(0)->getScore());
		statusBar()->changeItem(string, ID_STATUS_BASE + 1);
		statusBar()->changeItem(QString(), ID_STATUS_BASE + 2);
	}
	else
	{
		for (int i = 0; i < 2; ++i) {
			KTronEnum::Player player;
			player = (i == 0 ? KTronEnum::One : KTronEnum::Two);

			QString name;
			if (tron->isComputer(player))
				name = i18n("KTron");
			else
				name = playerName[i];
			
			QString string = QString("%1: %2").arg(name).arg(tron->getPlayer(i)->getScore());
			statusBar()->changeItem(string, ID_STATUS_BASE + i + 1);
		}
	}
}

void KTron::updateScore()
{
	updateStatusbar();
}

void KTron::changeStatus(KTronEnum::Player player) {
	// if player=Nobody, then new game
	if (player == KTronEnum::Nobody){
		updateStatusbar();
		return;
	}

	updateStatusbar();

	if (Settings::gameType() != Settings::EnumGameType::Snake)
	{
		KTronEnum::Player winner = getWinner();
		if (winner == KTronEnum::One || winner == KTronEnum::Two)
			showWinner(winner);
	}
	else
	{
		KScoreDialog scoreDialog(KScoreDialog::Score | KScoreDialog::Name, this);
		scoreDialog.addLocalizedConfigGroupNames(KGameDifficulty::localizedLevelStrings());
		scoreDialog.setConfigGroupWeights(KGameDifficulty::levelWeights());
		scoreDialog.setConfigGroup(KGameDifficulty::localizedLevelString());

		KScoreDialog::FieldInfo scoreInfo;
		scoreInfo[KScoreDialog::Score].setNum(tron->getPlayer(0)->getScore());
		if (scoreDialog.addScore(scoreInfo) != 0)
			scoreDialog.exec();
	}
}

KTronEnum::Player KTron::getWinner() {
	if (Settings::gameType() != Settings::EnumGameType::Snake)
	{
		if (tron->getPlayer(0)->getScore() >= WINNING_DIFF && tron->getPlayer(1)->getScore() < tron->getPlayer(0)->getScore() - 1) {
			return KTronEnum::One;
		}
		else if (tron->getPlayer(1)->getScore() >= WINNING_DIFF && tron->getPlayer(0)->getScore() < tron->getPlayer(1)->getScore() - 1) {
			return KTronEnum::Two;
		}
		else {
			return KTronEnum::Nobody;
		}
	}
	else {
		return KTronEnum::Nobody;
	}
}

void KTron::showWinner(KTronEnum::Player winner){
	if (tron->isComputer(KTronEnum::Both) || (winner != KTronEnum::One && winner != KTronEnum::Two))
		return;

	QString loserName = i18n("KTron");
	KTronEnum::Player loser = KTronEnum::Two;
	if (winner == KTronEnum::Two)
		loser = KTronEnum::One;
	if (!tron->isComputer(loser))
		loserName = playerName[loser];
	
	QString winnerName = i18n("KTron");
	if (!tron->isComputer(winner))
		winnerName = playerName[winner];

	QString message = i18n("%1 has won versus %2 with %3 : %4 points!", winnerName, loserName, tron->getPlayer(winner)->getScore(), tron->getPlayer(loser)->getScore());

	
	KMessageBox::information(this, message, i18n("Winner"));
	tron->newGame();
}

void KTron::paletteChange(const QPalette &){
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
 * Show highscores
 */
void KTron::showHighscores() {
	KScoreDialog scoreDialog(KScoreDialog::Score | KScoreDialog::Name, this);
	scoreDialog.addLocalizedConfigGroupNames(KGameDifficulty::localizedLevelStrings());
	scoreDialog.setConfigGroupWeights(KGameDifficulty::levelWeights());
	scoreDialog.setConfigGroup( KGameDifficulty::localizedLevelString() );
	scoreDialog.exec();
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

void KTron::optionsConfigureKeys()
{
    KShortcutsDialog::configure(actionCollection());
}

// Key events
void KTron::keyPressEvent(QKeyEvent *e)
{
	if (player0Accelerate->shortcuts().contains(e->key()))
	{
		triggerKey0Accelerate(true);
	}
	else if (player1Accelerate->shortcuts().contains(e->key()))
	{
		triggerKey1Accelerate(true);
	}
}

void KTron::keyReleaseEvent(QKeyEvent *e)
{
	if (player0Accelerate->shortcuts().contains(e->key()))
	{
		triggerKey0Accelerate(false);
	}
	else if (player1Accelerate->shortcuts().contains(e->key()))
	{
		triggerKey1Accelerate(false);
	}
}

// Triggers
void KTron::triggerKey0Up(bool b)
{
	tron->triggerKey(0, KBAction::UP, b);
}

void KTron::triggerKey0Down(bool b)
{
	tron->triggerKey(0, KBAction::DOWN, b);
}

void KTron::triggerKey0Left(bool b)
{
	tron->triggerKey(0, KBAction::LEFT, b);
}

void KTron::triggerKey0Right(bool b)
{
	tron->triggerKey(0, KBAction::RIGHT, b);
}

void KTron::triggerKey0Accelerate(bool b)
{
	tron->triggerKey(0, KBAction::ACCELERATE, b);
}

void KTron::triggerKey1Up(bool b)
{
	tron->triggerKey(1, KBAction::UP, b);
}

void KTron::triggerKey1Down(bool b)
{
	tron->triggerKey(1, KBAction::DOWN, b);
}

void KTron::triggerKey1Left(bool b)
{
	tron->triggerKey(1, KBAction::LEFT, b);
}

void KTron::triggerKey1Right(bool b)
{
	tron->triggerKey(1, KBAction::RIGHT, b);
}

void KTron::triggerKey1Accelerate(bool b)
{
	tron->triggerKey(1, KBAction::ACCELERATE, b);
}


#include "ktron.moc"

