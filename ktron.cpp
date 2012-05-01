/**********************************************************************************
  This file is part of the game 'KTron'

  Copyright (C) 1998-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
  Copyright (C) 2005 Benjamin C. Meyer <ben at meyerhome dot net>
  Copyright (C) 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

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

  *******************************************************************************/

#include "ktron.h"
#include "renderer.h"
#include "settings.h"
#include "ui_general.h"

#include <KConfigDialog>
#include <KLocale>
#include <KMessageBox>
#include <KAction>
#include <KActionCollection>
#include <KStandardGameAction>
#include <KScoreDialog>
#include <KgDifficulty>
#include <KShortcutsDialog>
#include <KStatusBar>
#include <KToggleAction>
#include <KApplication>

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgamethemeselector.h>

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
	m_tron = new Tron(this);
	connect(m_tron,SIGNAL(gameEnds()),SLOT(changeStatus()));
	connect(m_tron,SIGNAL(updatedScore()),SLOT(updateScore()));
	connect(m_tron,SIGNAL(pauseBlocked(bool)),SLOT(blockPause(bool)));
	m_tron->setMinimumSize(700,420);
	setCentralWidget(m_tron);

	// create statusbar
	statusBar()->insertItem(QLatin1String( "abcdefghijklmnopqrst: 0  " ),ID_STATUS_BASE + 1);
	statusBar()->insertItem(QLatin1String( "abcdefghijklmnopqrst: 0  " ),ID_STATUS_BASE + 2);

	// We match up keyboard events ourselves in Tron::keyPressEvent()
	// We must disable the actions, otherwise we don't get the keyPressEvent's

	m_player0Up = actionCollection()->addAction( QLatin1String( "Pl1Up" ));
	m_player0Up->setText(i18n("Right Player / KSnake: Up"));
	m_player0Up->setShortcut(Qt::Key_Up);
	connect(m_player0Up, SIGNAL(triggered(bool)), SLOT(triggerKey0Up(bool)));
	addAction(m_player0Up);

	m_player0Down = actionCollection()->addAction( QLatin1String( "Pl1Down" ));
	m_player0Down->setText(i18n("Right Player / KSnake: Down"));
	m_player0Down->setShortcut(Qt::Key_Down);
	connect(m_player0Down, SIGNAL(triggered(bool)), SLOT(triggerKey0Down(bool)));
	addAction(m_player0Down);

	m_player0Right = actionCollection()->addAction( QLatin1String( "Pl1Right" ));
	m_player0Right->setText(i18n("Right Player / KSnake: Right"));
	m_player0Right->setShortcut(Qt::Key_Right);
	connect(m_player0Right, SIGNAL(triggered(bool)), SLOT(triggerKey0Right(bool)));
	addAction(m_player0Right);

	m_player0Left = actionCollection()->addAction( QLatin1String( "Pl1Left" ));
	m_player0Left->setText(i18n("Right Player / KSnake: Left"));
	m_player0Left->setShortcut(Qt::Key_Left);
	connect(m_player0Left, SIGNAL(triggered(bool)), SLOT(triggerKey0Left(bool)));
	addAction(m_player0Left);

	m_player0Accelerate = actionCollection()->addAction( QLatin1String( "Pl1Ac" ));
	m_player0Accelerate->setText(i18n("Right Player: Accelerator"));
	m_player0Accelerate->setShortcut(Qt::Key_0);
	m_player0Accelerate->setEnabled(false); // Alternate handling, because of up/down events
	addAction(m_player0Accelerate);

	m_player1Up = actionCollection()->addAction( QLatin1String( "Pl2Up" ));
	m_player1Up->setText(i18n("Left Player: Up"));
	m_player1Up->setShortcut(Qt::Key_W);
	connect(m_player1Up, SIGNAL(triggered(bool)), SLOT(triggerKey1Up(bool)));
	addAction(m_player1Up);

	m_player1Down = actionCollection()->addAction( QLatin1String( "Pl2Down" ));
	m_player1Down->setText(i18n("Left Player: Down"));
	m_player1Down->setShortcut(Qt::Key_S);
	connect(m_player1Down, SIGNAL(triggered(bool)), SLOT(triggerKey1Down(bool)));
	addAction(m_player1Down);

	m_player1Right = actionCollection()->addAction( QLatin1String( "Pl2Right" ));;
	m_player1Right->setText(i18n("Left Player: Right"));
	m_player1Right->setShortcut(Qt::Key_D);
	connect(m_player1Right, SIGNAL(triggered(bool)), SLOT(triggerKey1Right(bool)));
	addAction(m_player1Right);

	m_player1Left = actionCollection()->addAction( QLatin1String( "Pl2Left" ));
	m_player1Left->setText(i18n("Left Player: Left"));
	m_player1Left->setShortcut(Qt::Key_A);
	connect(m_player1Left, SIGNAL(triggered(bool)), SLOT(triggerKey1Left(bool)));
	addAction(m_player1Left);

	m_player1Accelerate = actionCollection()->addAction( QLatin1String( "Pl2Ac" ));
	m_player1Accelerate->setText(i18n("Left Player: Accelerator"));
	m_player1Accelerate->setShortcut(Qt::Key_Q);
	m_player1Accelerate->setEnabled(false); // Alternate handling, because of up/down events
	addAction(m_player1Accelerate);

	// Pause
	m_pauseButton = KStandardGameAction::pause(m_tron, SLOT(togglePause()), actionCollection());
	m_pauseButton->setEnabled(false);
	// New
	KStandardGameAction::gameNew(m_tron, SLOT(newGame()), actionCollection());
	// Quit
	KStandardGameAction::quit(kapp, SLOT(quit()), actionCollection());
	// Settings
	KStandardAction::preferences(this, SLOT(showSettings()), actionCollection());
	// Configure keys
	KStandardAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
	// Highscores
	KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());

	//difficulty
    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::VeryEasy, KgDifficultyLevel::VeryHard,
        KgDifficultyLevel::Easy //default
    );
    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), m_tron, SLOT(loadSettings()));

	setupGUI( KXmlGuiWindow::Keys | StatusBar | Save | Create);
	loadSettings();
	m_tron->loadSettings();
}

KTron::~KTron()
{
	delete m_tron;
}

void KTron::loadSettings() {
	if (!Renderer::self()->loadTheme(Settings::theme()))
	{
		KMessageBox::error(this, i18n("Failed to load \"%1\" theme. Please check your installation.", Settings::theme()));
	}

	m_tron->getPlayer(0)->setName(Settings::namePlayer1());
	m_tron->getPlayer(1)->setName(Settings::namePlayer2());
	Settings::setNamePlayer1(m_tron->getPlayer(0)->getName());
	if (!m_tron->getPlayer(1)->isComputer()) {
		Settings::setNamePlayer2(m_tron->getPlayer(1)->getName());
	}

	updateStatusbar();
}

void KTron::updateStatusbar() {
	QString message;

	if (!m_tron->running() && m_tron->hasWinner()) {
		QString winnerName = m_tron->getPlayer(m_tron->getWinner())->getName();

		message = i18n("%1 has won!", winnerName);
	}
	else if (m_tron->paused()) {
		message = i18n("Game paused");
	}
	else {
		message = QString();
	}

	statusBar()->showMessage(message);

	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		QString string = QString::fromLatin1( "%1: %2").arg(m_tron->getPlayer(0)->getName()).arg(m_tron->getPlayer(0)->getScore());
		statusBar()->changeItem(string, ID_STATUS_BASE + 1);
		statusBar()->changeItem(QString(), ID_STATUS_BASE + 2);
	}
	else
	{
		for (int i = 0; i < 2; ++i) {
			QString name = m_tron->getPlayer(1 - i)->getName();
			int score = m_tron->getPlayer(1 - i)->getScore();

			QString string = QString::fromLatin1( "%1: %2").arg(name).arg(score);
			statusBar()->changeItem(string, ID_STATUS_BASE + i + 1);
		}
	}
}

void KTron::blockPause(bool block)
{
	//kDebug() << "Setting pause button state to: "  << !block;
	m_pauseButton->setEnabled(!block);
}

void KTron::updateScore()
{
	updateStatusbar();
}

void KTron::changeStatus() {
	updateStatusbar();

	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		KScoreDialog scoreDialog(KScoreDialog::Score | KScoreDialog::Name, this);
		scoreDialog.initFromDifficulty(Kg::difficulty());

		KScoreDialog::FieldInfo scoreInfo;
		scoreInfo[KScoreDialog::Name] = m_tron->getPlayer(0)->getName();
		scoreInfo[KScoreDialog::Score].setNum(m_tron->getPlayer(0)->getScore());
		if (scoreDialog.addScore(scoreInfo) != 0)
			scoreDialog.exec();
	}
}

void KTron::paletteChange(const QPalette &){
   update();
   m_tron->updatePixmap();
   m_tron->update();
}

/**
 * Show Settings dialog.
 */
void KTron::showSettings(){
	if (KConfigDialog::showDialog(QLatin1String( "settings" )))
		return;

	m_generalConfigDialog = new General();

	if (Settings::gameType() == Settings::EnumGameType::Snake) {
		m_generalConfigDialog->namePlayer1Label->setText(i18n("Player Name:"));
		m_generalConfigDialog->namePlayer2Label->setText(i18n("Opponent:"));
	}
	else {
		m_generalConfigDialog->namePlayer1Label->setText(i18n("Right Player:"));
		m_generalConfigDialog->namePlayer2Label->setText(i18n("Left Player:"));
	}

	KConfigDialog *dialog = new KConfigDialog(this, QLatin1String( "settings" ), Settings::self());
	dialog->addPage(m_generalConfigDialog, i18n("General"), QLatin1String( "games-config-options" ));
	dialog->addPage(new KGameThemeSelector(dialog, Settings::self(), KGameThemeSelector::NewStuffEnableDownload), i18n("Theme"), QLatin1String( "games-config-theme" ));
	connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(loadSettings()));
	connect(dialog, SIGNAL(settingsChanged(QString)), m_tron, SLOT(loadSettings()));
	dialog->show();
}

/**
 * Show highscores
 */
void KTron::showHighscores() {
	KScoreDialog scoreDialog(KScoreDialog::Score | KScoreDialog::Name, this);
	scoreDialog.initFromDifficulty(Kg::difficulty());
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
	if (m_player0Accelerate->shortcuts().contains(e->key()))
	{
		triggerKey0Accelerate(true);
	}
	else if (m_player1Accelerate->shortcuts().contains(e->key()))
	{
		triggerKey1Accelerate(true);
	}
}

void KTron::keyReleaseEvent(QKeyEvent *e)
{
	if (m_player0Accelerate->shortcuts().contains(e->key()))
	{
		triggerKey0Accelerate(false);
	}
	else if (m_player1Accelerate->shortcuts().contains(e->key()))
	{
		triggerKey1Accelerate(false);
	}
}

// Triggers
void KTron::triggerKey0Up(bool b)
{
	m_tron->triggerKey(0, KBAction::UP, b);
}

void KTron::triggerKey0Down(bool b)
{
	m_tron->triggerKey(0, KBAction::DOWN, b);
}

void KTron::triggerKey0Left(bool b)
{
	m_tron->triggerKey(0, KBAction::LEFT, b);
}

void KTron::triggerKey0Right(bool b)
{
	m_tron->triggerKey(0, KBAction::RIGHT, b);
}

void KTron::triggerKey0Accelerate(bool b)
{
	m_tron->triggerKey(0, KBAction::ACCELERATE, b);
}

void KTron::triggerKey1Up(bool b)
{
	m_tron->triggerKey(1, KBAction::UP, b);
}

void KTron::triggerKey1Down(bool b)
{
	m_tron->triggerKey(1, KBAction::DOWN, b);
}

void KTron::triggerKey1Left(bool b)
{
	m_tron->triggerKey(1, KBAction::LEFT, b);
}

void KTron::triggerKey1Right(bool b)
{
	m_tron->triggerKey(1, KBAction::RIGHT, b);
}

void KTron::triggerKey1Accelerate(bool b)
{
	m_tron->triggerKey(1, KBAction::ACCELERATE, b);
}


#include "ktron.moc"

