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


#ifndef KTOPTDLG_H
#define KTOPTDLG_H

#include <qdialog.h>
#include <ktabctl.h>

class QLineEdit;
class QCheckBox;

struct ExtOptions
{
   bool blockAccelerator;
   bool changeColor;
   QString namePl1;
   QString namePl2;
};


/**Dialog to configure extended options of ktron
  *@author Matthias Kiefer
  */

class KTOptDlg : public QDialog
{
   Q_OBJECT
public: 
  KTOptDlg(ExtOptions);
  ~KTOptDlg();
  ExtOptions options();


signals:
  void buttonPressed();

private slots:
   void showHelp();

private:
   void initOther();

   KTabCtl* tabWidget;
   QWidget *otherWidget;

   QCheckBox* changeColor;
   QCheckBox* blockAcc;

   QLineEdit* namePl1;
   QLineEdit* namePl2;
};

#endif



