/* ********************************************************************************
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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  ***************************************************************************** */  


#include "ktoptdlg.h"
#include <kglobal.h>
#include <klocale.h>
#include <kapp.h>
#include <kwizard.h>
#include <kbuttonbox.h>
#include <kseparator.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qwhatsthis.h>

KTOptDlg::KTOptDlg(QWidget* parent,ExtOptions opts)
        :KDialogBase(Tabbed, i18n("Options"), Help|Ok|Cancel,Ok,parent)
{
   QWidget* page=addPage(i18n("Other"));

   initOther(page);

   setHelp("ktron/index-4.html","extended");

   connect(this,SIGNAL(cancelClicked()),this,SLOT(reset()));

   _options=opts;
   reset();

   adjustSize();
}

KTOptDlg::~KTOptDlg()
{
}

void KTOptDlg::reset()
{
    changeColor->setChecked(_options.changeColor);
    blockAcc->setChecked(_options.blockAccelerator);
    namePl1->setText(_options.namePl1);
    namePl2->setText(_options.namePl2);
}

ExtOptions KTOptDlg::options()
{
    ExtOptions opts;
    opts.changeColor=changeColor->isChecked();
    opts.blockAccelerator=blockAcc->isChecked();

    opts.namePl1=namePl1->text();
    opts.namePl2=namePl2->text();

    _options=opts;

    return opts;
}



void KTOptDlg::initOther(QWidget* parent)
{
   otherWidget=parent;

   QVBoxLayout* layout=new QVBoxLayout(otherWidget,KDialog::marginHint(),KDialog::spacingHint());

   QGroupBox* box=new QGroupBox(1,Qt::Horizontal,i18n("Behavior")
            ,otherWidget);
   layout->addWidget(box);

   changeColor=new QCheckBox(i18n("Show winner by changing color")
               ,box);

   blockAcc=new QCheckBox(i18n("Disable acceleration"),box);


   box=new QGroupBox(2,Qt::Horizontal,i18n("Name of Players")
               ,otherWidget);
   layout->addWidget(box);

   QLabel* label=new QLabel(i18n("Player 1:"),box);
   namePl1=new QLineEdit(box);
   namePl1->setMaxLength(20);
   label=new QLabel(i18n("Player 2:"),box);
   namePl2=new QLineEdit(box);
   namePl2->setMaxLength(20);

   // Quickhelp message
   QString message=i18n(
"Show winner by changing color\n\n"
"If this is enabled and a player crashes,\n"
"his color changes to the other players color.");
   QWhatsThis::add(changeColor,message);

   // Quickhelp message
   message=i18n(
"Disable acceleration\n\n"
"If checked, the accelerator key is blocked.");
   QWhatsThis::add(blockAcc,message);

   layout->addStretch(3);

   otherWidget->setMinimumSize(otherWidget->sizeHint());
}















