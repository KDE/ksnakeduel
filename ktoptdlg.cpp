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

KTOptDlg::KTOptDlg(ExtOptions opts) : QDialog()
{
   setCaption(i18n("KTron - Options"));

   QVBoxLayout* layout=new QVBoxLayout(this,10);

   tabWidget=new KTabCtl(this);
   layout->addWidget(tabWidget,1);

   initOther();
   tabWidget->addTab(otherWidget,i18n("Other"));

   KButtonBox* bg=new KButtonBox(this);
   layout->addWidget(bg);
   QPushButton *ok, *cancel;
   QPushButton *help;
   help=bg->addButton(i18n("&Help"));
   connect(help,SIGNAL(clicked()),this,SLOT(showHelp()));
   bg->addStretch();
   ok = bg->addButton(i18n("&OK"));
   connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
   connect(ok,SIGNAL(clicked()),SIGNAL(buttonPressed()));
   cancel=bg->addButton(i18n("&Cancel"));
   connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));
   bg->layout();


    changeColor->setChecked(opts.changeColor);
    blockAcc->setChecked(opts.blockAccelerator);
    namePl1->setText(opts.namePl1);
    namePl2->setText(opts.namePl2);
}
KTOptDlg::~KTOptDlg()
{
}

ExtOptions KTOptDlg::options()
{
    ExtOptions opts;
    opts.changeColor=changeColor->isChecked();
    opts.blockAccelerator=blockAcc->isChecked();

    opts.namePl1=namePl1->text();
    opts.namePl2=namePl2->text();

    return opts;
}


void KTOptDlg::showHelp()
{
   kapp->invokeHTMLHelp("ktron/index-4.html","extended");
}


void KTOptDlg::initOther()
{
   otherWidget=new QWidget(tabWidget);

   QVBoxLayout* layout=new QVBoxLayout(otherWidget,10);

   QGroupBox* box=new QGroupBox(i18n("Behavior"),otherWidget);
   layout->addWidget(box);

   QVBoxLayout* boxLayout=new QVBoxLayout(box,10);
   boxLayout->addSpacing(10);
   changeColor=new QCheckBox(i18n("Show winner by changing color")
   							,box);

   boxLayout->addWidget(changeColor);

   // Quickhelp message
   QString message=i18n(
"Show winner by changing color\n\n\
If this is enabled and a player crashes,\n\
his color changes to the other players color.");
   QWhatsThis::add(changeColor,message);

   blockAcc=new QCheckBox(i18n("Disable acceleration"),box);
   boxLayout->addWidget(blockAcc);

   // Quickhelp message
   message=i18n(
"Disable acceleration\n\n\
If checked, the accelerator key is blocked.");
   QWhatsThis::add(blockAcc,message);
   // first Buttongroup ready

   box=new QGroupBox(i18n("Name of Players"),otherWidget);
   layout->addWidget(box);

   boxLayout=new QVBoxLayout(box,10);
   boxLayout->addSpacing(10);

   QHBoxLayout* hl=new QHBoxLayout(boxLayout);
   QLabel* label=new QLabel(i18n("Player 1:"),box);
   hl->addWidget(label);

   namePl1=new QLineEdit(box);
   namePl1->setMaxLength(20);
   hl->addWidget(namePl1);

   hl=new QHBoxLayout();
   boxLayout->addLayout(hl);
   label=new QLabel(i18n("Player 2:"),box);
   hl->addWidget(label);


   namePl2=new QLineEdit(box);
   namePl2->setMaxLength(20);
   hl->addWidget(namePl2);

   layout->addStretch(3);

   otherWidget->setMinimumSize(otherWidget->sizeHint());
}















