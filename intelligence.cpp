/*
    This file is part of the game 'KTron'

    SPDX-FileCopyrightText: 1998-2000 Matthias Kiefer <matthias.kiefer@gmx.de>
    SPDX-FileCopyrightText: 2005 Benjamin C. Meyer <ben at meyerhome dot net>
    SPDX-FileCopyrightText: 2008-2009 Stas Verberkt <legolas at legolasweb dot nl>

    SPDX-License-Identifier: GPL-2.0-or-later

*/
  
#include "intelligence.h"

#include "tron.h"
#include "settings.h"

#include <KgDifficulty>

Intelligence::Intelligence()
    : m_random(QRandomGenerator::global()->generate())
{
	m_lookForward = 15;
}

void Intelligence::referenceTron(Tron *t)
{
	m_tron = t;
}

//
// Settings
//

/** retrieves the opponentSkill */
int Intelligence::opponentSkill() {
	switch (Kg::difficultyLevel()) {
		case KgDifficultyLevel::VeryEasy:
			return 1;
		default:
		case KgDifficultyLevel::Easy:
			return 1;
		case KgDifficultyLevel::Medium:
			return 2;
		case KgDifficultyLevel::Hard:
			return 3;
		case KgDifficultyLevel::VeryHard:
			return 3;
	}
}

//
// Algorithm helper function
//

void Intelligence::changeDirection(int playerNr,int dis_right,int dis_left)
{
   PlayerDirections::Direction currentDir = m_tron->getPlayer(playerNr)->getDirection();
   PlayerDirections::Direction sides[2];
   sides[0] = PlayerDirections::None;
   sides[1] = PlayerDirections::None;
   
   switch (currentDir)
   {
  		case PlayerDirections::Left:
    		//turns to either side
    		sides[0] = PlayerDirections::Down;
    		sides[1] = PlayerDirections::Up;
    		break;
  		case PlayerDirections::Right:
    		sides[0] = PlayerDirections::Up;
    		sides[1] = PlayerDirections::Down;
    		break;
  		case PlayerDirections::Up:
    		sides[0] = PlayerDirections::Left;
    		sides[1] = PlayerDirections::Right;
    		break;
  		case PlayerDirections::Down:
    		sides[0] = PlayerDirections::Right;
    		sides[1] = PlayerDirections::Left;
    		break;
		default:
		break;

  	}

   if(!(dis_left == 1 && dis_right == 1))
   {
			// change direction
			if (m_random.bounded(100) <= (100*dis_left)/(dis_left+dis_right))
			{
	  			if (dis_left != 1)
		    		// turn to the left
		    		m_tron->getPlayer(playerNr)->setDirection(sides[0]);
	  			else
	   	 		// turn to the right
	    			m_tron->getPlayer(playerNr)->setDirection(sides[1]);
	    	}
			else
			{
	  				if (dis_right != 1)
	  					// turn to the right
	    				m_tron->getPlayer(playerNr)->setDirection(sides[1]);
	  				else
	    				// turn to the left
	    				m_tron->getPlayer(playerNr)->setDirection(sides[0]);
          }
    }
}

// This part is partly ported from
// xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>
void Intelligence::think(int playerNr)
{
	if (opponentSkill() != 1)
	{
		int opponent=(playerNr==1)? 0 : 1;

		// determines left and right side
		PlayerDirections::Direction sides[2];
		sides[0] = PlayerDirections::None;
		sides[1] = PlayerDirections::None;
		// increments for moving to the different sides
		int flags[6]={0,0,0,0,0,0};
		int index[2];
		// distances to barrier
		int dis_forward,  dis_left, dis_right;

		dis_forward = dis_left = dis_right = 1;

		switch (m_tron->getPlayer(playerNr)->getDirection())
		{
			case PlayerDirections::Left:
				//forward flags
				flags[0] = -1;
				flags[1] = 0;

				//left flags
				flags[2] = 0;
				flags[3] = 1;

				// right flags
				flags[4] = 0;
				flags[5] = -1;

				//turns to either side
				sides[0] = PlayerDirections::Down;
				sides[1] = PlayerDirections::Up;
				break;
			case PlayerDirections::Right:
				flags[0] = 1;
				flags[1] = 0;
				flags[2] = 0;
				flags[3] = -1;
				flags[4] = 0;
				flags[5] = 1;
				sides[0] = PlayerDirections::Up;
				sides[1] = PlayerDirections::Down;
				break;
			case PlayerDirections::Up:
				flags[0] = 0;
				flags[1] = -1;
				flags[2] = -1;
				flags[3] = 0;
				flags[4] = 1;
				flags[5] = 0;
				sides[0] = PlayerDirections::Left;
				sides[1] = PlayerDirections::Right;
				break;
			case PlayerDirections::Down:
				flags[0] = 0;
				flags[1] = 1;
				flags[2] = 1;
				flags[3] = 0;
				flags[4] = -1;
				flags[5] = 0;
				sides[0] = PlayerDirections::Right;
				sides[1] = PlayerDirections::Left;
				break;
			default:
				break;
		}

		// check forward
		index[0] = m_tron->getPlayer(playerNr)->getX()+flags[0];
		index[1] = m_tron->getPlayer(playerNr)->getY()+flags[1];
		while (index[0] < m_tron->getPlayField()->getWidth() && index[0] >= 0 && index[1] < m_tron->getPlayField()->getHeight() && index[1] >= 0 && m_tron->getPlayField()->getObjectAt(index[0], index[1])->getObjectType() == ObjectType::Object)
		{
			dis_forward++;
			index[0] += flags[0];
			index[1] += flags[1];
		}

		// check left
		index[0] = m_tron->getPlayer(playerNr)->getX()+flags[2];
		index[1] = m_tron->getPlayer(playerNr)->getY()+flags[3];
		while (index[0] < m_tron->getPlayField()->getWidth() && index[0] >= 0 && index[1] < m_tron->getPlayField()->getHeight() && index[1] >= 0 && m_tron->getPlayField()->getObjectAt(index[0], index[1])->getObjectType() == ObjectType::Object)
		{
			dis_left++;
			index[0] += flags[2];
			index[1] += flags[3];
		}

		// check right
		index[0] = m_tron->getPlayer(playerNr)->getX()+flags[4];
		index[1] = m_tron->getPlayer(playerNr)->getY()+flags[5];
		while (index[0] < m_tron->getPlayField()->getWidth() && index[0] >= 0 && index[1] <  m_tron->getPlayField()->getHeight() && index[1] >= 0 && m_tron->getPlayField()->getObjectAt(index[0], index[1])->getObjectType() == ObjectType::Object)
		{
			dis_right++;
			index[0] += flags[4];
			index[1] += flags[5];
		}

		// distances to opponent
		int hor_dis=0; // negative is opponent to the right
		int vert_dis=0; // negative is opponent to the bottom
		hor_dis = m_tron->getPlayer(playerNr)->getX() - m_tron->getPlayer(opponent)->getX();
		vert_dis = m_tron->getPlayer(playerNr)->getY() - m_tron->getPlayer(opponent)->getY();

		int opForwardDis=0; // negative is to the back
		int opSideDis=0;  // negative is to the left
		bool opMovesOppositeDir=false;
		bool opMovesSameDir=false;
		bool opMovesRight=false;
		bool opMovesLeft=false;

		switch (m_tron->getPlayer(playerNr)->getDirection())
		{
			case PlayerDirections::Up:
				opForwardDis=vert_dis;
				opSideDis=-hor_dis;
				if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Down)
					opMovesOppositeDir=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Up)
					opMovesSameDir=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Left)
					opMovesLeft=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Right)
					opMovesRight=true;
				break;
			case PlayerDirections::Down:
				opForwardDis=-vert_dis;
				opSideDis=hor_dis;
				if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Up)
					opMovesOppositeDir=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Down)
					opMovesSameDir=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Left)
					opMovesRight=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Right)
					opMovesLeft=true;
				break;
			case PlayerDirections::Left:
				opForwardDis=hor_dis;
				opSideDis=vert_dis;
				if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Right)
					opMovesOppositeDir=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Left)
					opMovesSameDir=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Down)
					opMovesLeft=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Up)
					opMovesRight=true;
				break;
			case PlayerDirections::Right:
				opForwardDis=-hor_dis;
				opSideDis=-vert_dis;
				if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Left)
					opMovesOppositeDir=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Right)
					opMovesSameDir=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Up)
					opMovesLeft=true;
				else if(m_tron->getPlayer(opponent)->getDirection()==PlayerDirections::Down)
					opMovesRight=true;
				break;
			default:
				break;
		}

		int doPercentage = 100;
		switch(opponentSkill())
		{
			case 1:
				// Never reached
				break;
			case 2:
				doPercentage=5;
				break;
			case 3:
				doPercentage=90;
				break;
		}

		// if opponent moves the opposite direction as we
		if(opMovesOppositeDir)
		{
			// if opponent is in front
			if(opForwardDis>0)
			{
				// opponent is to the right and we have the chance to block the way
				if(opSideDis>0 && opSideDis < opForwardDis && opSideDis < dis_right && opForwardDis < m_lookForward)
				{
					if (m_random.bounded(100) <= doPercentage || dis_forward==1)
						m_tron->getPlayer(playerNr)->setDirection(sides[1]); // turn right
				}
				// opponent is to the left and we have the chance to block the way
				else if(opSideDis<0 && -opSideDis < opForwardDis && -opSideDis < dis_left && opForwardDis < m_lookForward)
				{
					if (m_random.bounded(100) <= doPercentage || dis_forward==1)
						m_tron->getPlayer(playerNr)->setDirection(sides[0]); // turn left
				}
				// if we can do nothing, go forward
				else if(dis_forward < m_lookForward)
				{
					dis_forward = 100 - 100/dis_forward;

					if(!(dis_left == 1 && dis_right == 1))
						if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
							changeDirection(playerNr,dis_right,dis_left);
				}
			}
			// opponent is in back of us and moves away: do nothing
			else if(dis_forward < m_lookForward)
			{
				dis_forward = 100 - 100/dis_forward;

				if(!(dis_left == 1 && dis_right == 1))
					if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
							changeDirection(playerNr,dis_right,dis_left);
			}
		} // end  if(opMovesOppositeDir)
		else if(opMovesSameDir)
		{
			// if opponent is to the back
			if(opForwardDis < 0)
			{
					// opponent is to the right and we have the chance to block the way
				if(opSideDis>0 && opSideDis < -opForwardDis && opSideDis < dis_right)
				{
					if (m_random.bounded(100) <= doPercentage || dis_forward==1)
						m_tron->getPlayer(playerNr)->setDirection(sides[1]); // turn right
				}
				// opponent is to the left and we have the chance to block the way
				else if(opSideDis<0 && -opSideDis < -opForwardDis && -opSideDis < dis_left)
				{
					if (m_random.bounded(100) <= doPercentage || dis_forward==1)
						m_tron->getPlayer(playerNr)->setDirection(sides[0]); // turn left
				}
				// if we can do nothing, go forward
				else if(dis_forward < m_lookForward)
				{
					dis_forward = 100 - 100/dis_forward;

						if(!(dis_left == 1 && dis_right == 1))
							if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
								changeDirection(playerNr,dis_right,dis_left);
				}
			}
			// opponent is in front of us and moves away
			else if(dis_forward < m_lookForward)
			{
				dis_forward = 100 - 100/dis_forward;

					if(!(dis_left == 1 && dis_right == 1))
						if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
							changeDirection(playerNr,dis_right,dis_left);
			}
		} // end if(opMovesSameDir)
		else if(opMovesRight)
		{
			// opponent is in front of us
			if(opForwardDis>0)
			{
				// opponent is to the left
				if(opSideDis < 0 && -opSideDis < opForwardDis && -opSideDis < dis_left)
				{
					if(opForwardDis < m_lookForward && dis_left > m_lookForward)
					{
						if (m_random.bounded(100) <= doPercentage/2 || dis_forward==1)
							changeDirection(playerNr,dis_right,dis_left);
					}
					else if(dis_forward < m_lookForward)
					{
						dis_forward = 100 - 100/dis_forward;

							if(!(dis_left == 1 && dis_right == 1))
								if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
									changeDirection(playerNr,dis_right,dis_left);
					}
				}
				// op is to the right and moves away, but maybe we can block him
				else if(opSideDis>=0 && opSideDis < dis_right)
				{
					if(opForwardDis < m_lookForward && dis_right > m_lookForward)
					{
						if (m_random.bounded(100) <= doPercentage/2 || dis_forward==1)
							m_tron->getPlayer(playerNr)->setDirection(sides[1]); // turn right
					}
					else if(dis_forward < m_lookForward)
					{
						dis_forward = 100 - 100/dis_forward;

							if(!(dis_left == 1 && dis_right == 1))
								if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
									changeDirection(playerNr,dis_right,dis_left);
					}
				}
				else if(dis_forward < m_lookForward)
				{
					dis_forward = 100 - 100/dis_forward;

						if(!(dis_left == 1 && dis_right == 1))
							if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
								changeDirection(playerNr,dis_right,dis_left);
				}
			}
			// opponent is in the back of us
			else
			{
				// opponent is right from us and we already blocked him
				if(opSideDis>0 && opForwardDis < m_lookForward && opSideDis < dis_right)
				{
					if (m_random.bounded(100) <= doPercentage/2 || dis_forward==1)
						changeDirection(playerNr,dis_right,dis_left);
				}
				else if(dis_forward < m_lookForward)
				{
					dis_forward = 100 - 100/dis_forward;

						if(!(dis_left == 1 && dis_right == 1))
							if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
								changeDirection(playerNr,dis_right,dis_left);
				}
			}
		} // end if(opMovesRight)
		else if(opMovesLeft)
		{
			// opponent is in front of us
			if(opForwardDis>0)
			{
				// opponent is to the right, moves towards us and could block us
				if(opSideDis > 0 && opSideDis < opForwardDis && opSideDis < dis_right)
				{
					if(opForwardDis < m_lookForward && dis_right > m_lookForward)
					{
						if (m_random.bounded(100) <= doPercentage/2 || dis_forward==1)
							changeDirection(playerNr,dis_right,dis_left);
					}
					else if(dis_forward < m_lookForward)
					{
						dis_forward = 100 - 100/dis_forward;

						if(!(dis_left == 1 && dis_right == 1))
							if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
								changeDirection(playerNr,dis_right,dis_left);
					}
				}
				// op is to the left and moves away, but maybe we can block him
				else if(opSideDis<=0 && opSideDis < dis_left)
				{
					if(opForwardDis < m_lookForward && dis_left > m_lookForward)
					{
						if (m_random.bounded(100) <= doPercentage/2 || dis_forward==1)
							m_tron->getPlayer(playerNr)->setDirection(sides[0]); // m_turn left
						}
					else if(dis_forward < m_lookForward)
					{
						dis_forward = 100 - 100/dis_forward;

						if(!(dis_left == 1 && dis_right == 1))
							if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
								changeDirection(playerNr,dis_right,dis_left);
					}

				}
				else if(dis_forward < m_lookForward)
				{
					dis_forward = 100 - 100/dis_forward;

					if(!(dis_left == 1 && dis_right == 1))
						if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
							changeDirection(playerNr,dis_right,dis_left);
				}
			}
			// opponent is in the back of us
			else //if(opForwardDis<=0)
			{
				// opponent is left from us and we already blocked him
				if(opSideDis<0 && opForwardDis < m_lookForward && -opSideDis < dis_left)
				{
					if (m_random.bounded(100) <= doPercentage/2 || dis_forward==1)
						changeDirection(playerNr,dis_right,dis_left);
				}
				else if(dis_forward < m_lookForward)
				{
					dis_forward = 100 - 100/dis_forward;

					if(!(dis_left == 1 && dis_right == 1))
						if (m_random.bounded(100) >= dis_forward || dis_forward == 1)
							changeDirection(playerNr,dis_right,dis_left);
				}
			}
		} // end if(opMovesLeft)

	}
	// This part is completely ported from
	// xtron-1.1 by Rhett D. Jacobs <rhett@hotel.canberra.edu.au>
	else // Settings::skill() == Settings::EnumSkill::Easy
	{
		PlayerDirections::Direction sides[2];
		sides[0] = PlayerDirections::None;
		sides[1] = PlayerDirections::None;
		int flags[6] = {0,0,0,0,0,0};
		int index[2];
		int dis_forward,  dis_left, dis_right;

		dis_forward = dis_left = dis_right = 1;

		switch (m_tron->getPlayer(playerNr)->getDirection()) {
			case PlayerDirections::Left:
				//forward flags
				flags[0] = -1;
				flags[1] = 0;
				//left flags
				flags[2] = 0;
				flags[3] = 1;
				// right flags
				flags[4] = 0;
				flags[5] = -1;
				//turns to either side
				sides[0] = PlayerDirections::Down;
				sides[1] = PlayerDirections::Up;
				break;
			case PlayerDirections::Right:
				flags[0] = 1;
				flags[1] = 0;
				flags[2] = 0;
				flags[3] = -1;
				flags[4] = 0;
				flags[5] = 1;
				sides[0] = PlayerDirections::Up;
				sides[1] = PlayerDirections::Down;
				break;
			case PlayerDirections::Up:
				flags[0] = 0;
				flags[1] = -1;
				flags[2] = -1;
				flags[3] = 0;
				flags[4] = 1;
				flags[5] = 0;
				sides[0] = PlayerDirections::Left;
				sides[1] = PlayerDirections::Right;
				break;
			case PlayerDirections::Down:
				flags[0] = 0;
				flags[1] = 1;
				flags[2] = 1;
				flags[3] = 0;
				flags[4] = -1;
				flags[5] = 0;
				sides[0] = PlayerDirections::Right;
				sides[1] = PlayerDirections::Left;
				break;
			default:
				break;
		}

		// check forward
		index[0] = m_tron->getPlayer(playerNr)->getX() + flags[0];
		index[1] = m_tron->getPlayer(playerNr)->getY() + flags[1];
		while (index[0] < m_tron->getPlayField()->getWidth() && index[0] >= 0 && index[1] < m_tron->getPlayField()->getHeight() && index[1] >= 0 && m_tron->getPlayField()->getObjectAt(index[0], index[1])->getObjectType() == ObjectType::Object) {
			dis_forward++;
			index[0] += flags[0];
			index[1] += flags[1];
		}

		if (dis_forward < m_lookForward)
		{
			dis_forward = 100 - 100 / dis_forward;

			// check left
			index[0] = m_tron->getPlayer(playerNr)->getX() + flags[2];
			index[1] = m_tron->getPlayer(playerNr)->getY() + flags[3];
			while (index[0] < m_tron->getPlayField()->getWidth() && index[0] >= 0 && index[1] < m_tron->getPlayField()->getHeight() && index[1] >= 0 && m_tron->getPlayField()->getObjectAt(index[0], index[1])->getObjectType() == ObjectType::Object) {
				dis_left++;
				index[0] += flags[2];
				index[1] += flags[3];
			}

			// check right
			index[0] = m_tron->getPlayer(playerNr)->getX() + flags[4];
			index[1] = m_tron->getPlayer(playerNr)->getY() + flags[5];
			while (index[0] < m_tron->getPlayField()->getWidth() && index[0] >= 0 && index[1] <  m_tron->getPlayField()->getHeight() && index[1] >= 0 && m_tron->getPlayField()->getObjectAt(index[0], index[1])->getObjectType() == ObjectType::Object) {
				dis_right++;
				index[0] += flags[4];
				index[1] += flags[5];
			}
			if(!(dis_left == 1 && dis_right == 1)) {
				if (m_random.bounded(100) >= dis_forward || dis_forward == 0) {
					// change direction
					if (m_random.bounded(100) <= (100*dis_left)/(dis_left+dis_right)) {
						if (dis_left != 1)
							// turn to the left
							m_tron->getPlayer(playerNr)->setDirection(sides[0]);
						else
							// turn to the right
							m_tron->getPlayer(playerNr)->setDirection(sides[1]);
					}
					else {
						if (dis_right != 1)
							// turn to the right
							m_tron->getPlayer(playerNr)->setDirection(sides[1]);
						else
							// turn to the left
							m_tron->getPlayer(playerNr)->setDirection(sides[0]);
					}
				}
			}
		}
	}
}

