/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef PLAYER_H
#define PLAYER_H

#include "snakepart.h"
#include "playfield.h"

#include <QObject>
#include <QQueue>

namespace PlayerDirections
{
	enum Direction {
		None,
		Up,
		Down,
		Left,
		Right
	};
}

/**
* @short This class represents a player with current position and several flags
*/
class Player : public QObject
{
	Q_OBJECT

	public:
		Player(PlayField &pf, int playerNr);
		int getPlayerNumber();
		void reset();
        bool isComputer() const;
		void setComputer(bool computer);
		void setStartPosition();
		void movePlayer();
		bool crashed(int x, int y);
		int getX();
		int getY();
		int getScore();
		void addScore(int increment);
		void resetScore();
		void setEnlargement(int enlargement);
		PlayerDirections::Direction getDirection();
		void setDirection(PlayerDirections::Direction direction);
		bool isAlive();
		void die();
		bool isAccelerated();
		void setAccelerated(bool value);
		bool hasKeyPressed();
		void setKeyPressed(bool value);
        QString getName() const;
        void setName(const QString &name);
		
	private:
		int m_playerNumber;
		QQueue<SnakePart> m_snakeParts;
		PlayField *m_playField;
		int m_score;
		int m_enlarge;
		PlayerDirections::Direction m_dir;
		bool m_alive;
		bool m_computer;
		bool m_accelerated;
		bool m_keyPressed;
		QString m_name;
		bool m_blockSwitchDir;
		
	Q_SIGNALS:
		void fetchedItem(int playerNumber, int x, int y);
};

#endif //PLAYER_H

