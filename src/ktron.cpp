/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#include "ktron.h"
#include "renderer.h"
#include "settings.h"
#include "ui_general.h"

#include <QApplication>
#include <QStatusBar>

#include <KActionCollection>
#include <KConfigDialog>
#include <KgDifficulty>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardGameAction>
#include <KScoreDialog>
#include <KShortcutsDialog>
#include <KToggleAction>

#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgamethemeselector.h>

//UI
class General : public QWidget, public Ui::General
{
public:
    explicit General(QWidget *parent = nullptr)
        : QWidget(parent)
        {
            setupUi(this);
        }
};

/**
 * Constructor
 */
KTron::KTron(QWidget *parent) : KXmlGuiWindow(parent, Qt::WindowFlags()) {
	m_tron = new Tron(this);
	connect(m_tron, &Tron::gameEnds, this, &KTron::changeStatus);
	connect(m_tron, &Tron::updatedScore, this, &KTron::updateScore);
	connect(m_tron, &Tron::pauseBlocked, this, &KTron::blockPause);
	m_tron->setMinimumSize(700,420);
	setCentralWidget(m_tron);

	// create statusbar
	for (auto &label : m_statusBarLabel) {
		label = new QLabel(this);
		label->setAlignment(Qt::AlignCenter);
		statusBar()->addWidget(label, 1);
	}

	// We match up keyboard events ourselves in Tron::keyPressEvent()
	// We must disable the actions, otherwise we don't get the keyPressEvent's

	m_player0Up = actionCollection()->addAction( QStringLiteral( "Pl1Up" ));
	m_player0Up->setText(i18n("Right Player / KSnakeDuel: Up"));
	actionCollection()->setDefaultShortcut(m_player0Up,Qt::Key_Up);
	connect(m_player0Up, &QAction::triggered, this, &KTron::triggerKey0Up);
	addAction(m_player0Up);

	m_player0Down = actionCollection()->addAction( QStringLiteral( "Pl1Down" ));
	m_player0Down->setText(i18n("Right Player / KSnakeDuel: Down"));
	actionCollection()->setDefaultShortcut(m_player0Down,Qt::Key_Down);
	connect(m_player0Down, &QAction::triggered, this, &KTron::triggerKey0Down);
	addAction(m_player0Down);

	m_player0Right = actionCollection()->addAction( QStringLiteral( "Pl1Right" ));
	m_player0Right->setText(i18n("Right Player / KSnakeDuel: Right"));
	actionCollection()->setDefaultShortcut(m_player0Right,Qt::Key_Right);
	connect(m_player0Right, &QAction::triggered, this, &KTron::triggerKey0Right);
	addAction(m_player0Right);

	m_player0Left = actionCollection()->addAction( QStringLiteral( "Pl1Left" ));
	m_player0Left->setText(i18n("Right Player / KSnakeDuel: Left"));
	actionCollection()->setDefaultShortcut(m_player0Left,Qt::Key_Left);
	connect(m_player0Left, &QAction::triggered, this, &KTron::triggerKey0Left);
	addAction(m_player0Left);

	m_player0Accelerate = actionCollection()->addAction( QStringLiteral( "Pl1Ac" ));
	m_player0Accelerate->setText(i18n("Right Player: Accelerator"));
	actionCollection()->setDefaultShortcut(m_player0Accelerate,Qt::Key_0);
	m_player0Accelerate->setEnabled(false); // Alternate handling, because of up/down events
	addAction(m_player0Accelerate);

	m_player1Up = actionCollection()->addAction( QStringLiteral( "Pl2Up" ));
	m_player1Up->setText(i18n("Left Player: Up"));
	actionCollection()->setDefaultShortcut(m_player1Up,Qt::Key_W);
	connect(m_player1Up, &QAction::triggered, this, &KTron::triggerKey1Up);
	addAction(m_player1Up);

	m_player1Down = actionCollection()->addAction( QStringLiteral( "Pl2Down" ));
	m_player1Down->setText(i18n("Left Player: Down"));
	actionCollection()->setDefaultShortcut(m_player1Down,Qt::Key_S);
	connect(m_player1Down, &QAction::triggered, this, &KTron::triggerKey1Down);
	addAction(m_player1Down);

	m_player1Right = actionCollection()->addAction( QStringLiteral( "Pl2Right" ));;
	m_player1Right->setText(i18n("Left Player: Right"));
	actionCollection()->setDefaultShortcut(m_player1Right,Qt::Key_D);
	connect(m_player1Right, &QAction::triggered, this, &KTron::triggerKey1Right);
	addAction(m_player1Right);

	m_player1Left = actionCollection()->addAction( QStringLiteral( "Pl2Left" ));
	m_player1Left->setText(i18n("Left Player: Left"));
	actionCollection()->setDefaultShortcut(m_player1Left,Qt::Key_A);
	connect(m_player1Left, &QAction::triggered, this, &KTron::triggerKey1Left);
	addAction(m_player1Left);

	m_player1Accelerate = actionCollection()->addAction( QStringLiteral( "Pl2Ac" ));
	m_player1Accelerate->setText(i18n("Left Player: Accelerator"));
	actionCollection()->setDefaultShortcut(m_player1Accelerate,Qt::Key_Q);
	m_player1Accelerate->setEnabled(false); // Alternate handling, because of up/down events
	addAction(m_player1Accelerate);

	// Pause
	m_pauseButton = KStandardGameAction::pause(m_tron, &Tron::togglePause, actionCollection());
	m_pauseButton->setEnabled(false);
	// New
	KStandardGameAction::gameNew(m_tron, &Tron::newGame, actionCollection());
	// Quit
	KStandardGameAction::quit(qApp, &QApplication::quit, actionCollection());
	// Settings
	KStandardAction::preferences(this, &KTron::showSettings, actionCollection());
	// Configure keys
	KStandardAction::keyBindings(this, &KTron::optionsConfigureKeys, actionCollection());
	// Highscores
	KStandardGameAction::highscores(this, &KTron::showHighscores, actionCollection());

	//difficulty
    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::VeryEasy, KgDifficultyLevel::VeryHard,
        KgDifficultyLevel::Easy //default
    );
    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), &KgDifficulty::currentLevelChanged, m_tron, &Tron::loadSettings);

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

	m_statusBarLabel[0]->setText(message);

	if (Settings::gameType() == Settings::EnumGameType::Snake)
	{
		QString string = QStringLiteral( "%1: %2").arg(m_tron->getPlayer(0)->getName()).arg(m_tron->getPlayer(0)->getScore());
		m_statusBarLabel[1]->setText(string);
		m_statusBarLabel[2]->clear();
	}
	else
	{
		for (int i = 0; i < 2; ++i) {
			QString name = m_tron->getPlayer(1 - i)->getName();
			int score = m_tron->getPlayer(1 - i)->getScore();

			QString string = QStringLiteral( "%1: %2").arg(name).arg(score);
			m_statusBarLabel[i+1]->setText(string);
		}
	}
}

void KTron::blockPause(bool block)
{
	//qCDebug(KSNAKEDUEL_LOG) << "Setting pause button state to: "  << !block;
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
	if (KConfigDialog::showDialog(QStringLiteral( "settings" )))
		return;

	m_generalConfigDialog = new General();

	if (Settings::gameType() == Settings::EnumGameType::Snake) {
		m_generalConfigDialog->namePlayer1Label->setText(i18n("Player Name:"));
		m_generalConfigDialog->namePlayer2Label->setText(i18n("Opponent:"));
	}
	else {
		m_generalConfigDialog->namePlayer1Label->setText(i18n("Right player:"));
		m_generalConfigDialog->namePlayer2Label->setText(i18n("Left player:"));
	}

	KConfigDialog *dialog = new KConfigDialog(this, QStringLiteral( "settings" ), Settings::self());
	dialog->addPage(m_generalConfigDialog, i18n("General"), QStringLiteral( "games-config-options" ));
	dialog->addPage(new KGameThemeSelector(dialog, Settings::self(), KGameThemeSelector::NewStuffEnableDownload), i18n("Theme"), QStringLiteral( "games-config-theme" ));
	connect(dialog, &KConfigDialog::settingsChanged, this, &KTron::loadSettings);
	connect(dialog, &KConfigDialog::settingsChanged, m_tron, &Tron::loadSettings);
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
	Settings::self()->save();
}

void KTron::closeEvent(QCloseEvent *event)
{
    close();
    event->accept();
}

void KTron::optionsConfigureKeys()
{
    KShortcutsDialog::showDialog(actionCollection(), KShortcutsEditor::LetterShortcutsAllowed, this);
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




