/*
 * UKDN-S is the "Service prog for UKDN", i.e. the program to make a communication
 * with the Voltage Divider Controlling Device by AVM-Energo LLC
 *
 * Copyright (C) 2017 <Evel> <forevel@yandex.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <QToolBar>
#include <QVBoxLayout>
#include "pkdn_s.h"
#include "../check/checkdialoga1.h"
#include "../config/confdialoga1.h"
#include "../dialogs/fwupdialog.h"
#include "../dialogs/downloaddialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/oscdialog.h"
#include "../tune/tunedialoga1.h"
#include "../tune/tunedialoga1dn.h"
#include "../dialogs/a1dialog.h"
#include "../widgets/mytabwidget.h"


pkdn_s::pkdn_s(QWidget *parent)
    : MainWindow(parent)
{
//    SetParent(this);
    SetupUI();
}

void pkdn_s::SetupUI()
{
    setWindowTitle(PROGCAPTION);
    QString tmps = "QMainWindow {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(800, 600));
    QWidget *wdgt = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
//    lyout->setSpacing(0);
//    lyout->setMargin(0);
    setMinimumHeight(650);

    QHBoxLayout *hlyout = new QHBoxLayout;
    QToolBar *tb = new QToolBar;
    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(20,20));
    QAction *act = new QAction(this);
    act->setToolTip("Соединение");
    act->setIcon(QIcon(":/pic/play.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Разрыв соединения");
    act->setIcon(QIcon(":/pic/stop.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Disconnect()));
    tb->addAction(act);
    tb->addSeparator();
    act = new QAction(this);
    act->setToolTip("Эмуляция A1");
    quint16 MType = MTB_A1;
    MType = MType << 8 & MTM_00;
    act->setObjectName(QString::number(MType, 16)); // для слота StartEmul
    act->setIcon(QIcon(":/pic/a1.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartEmul()));
    tb->addAction(act);
    tb->addSeparator();
    act = new QAction(this);
    act->setToolTip("Настройки");
    act->setIcon(QIcon(":/pic/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Протокол ошибок");
    act->setIcon(QIcon(":/pic/skull-and-bones.png"));
    connect(act,SIGNAL(triggered(bool)),this,SLOT(ShowErrorDialog()));
    tb->addAction(act);
    hlyout->addWidget(tb);
    hlyout->addWidget(HthWidget());
    lyout->addLayout(hlyout);
//    lyout->addWidget(HthWidget());
    lyout->addWidget(Least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    SetSlideWidget();
}

void pkdn_s::Stage3()
{
    ClearTW();
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    InfoDialog *idlg = new InfoDialog;
    connect(this,SIGNAL(BsiRefresh()),idlg,SLOT(FillBsi()));
    connect(this,SIGNAL(ClearBsi()),idlg,SLOT(ClearBsi()));
    MainTW->addTab(idlg, "Информация");
    ConfDialogA1 *DialogA1 = new ConfDialogA1(S2Config);
    ConfB = DialogA1;
    MainTW->addTab(ConfB, "Конфигурирование");
    connect(ConfB,SIGNAL(NewConfLoaded()),this,SLOT(Fill()));
    connect(ConfB,SIGNAL(LoadDefConf()),this,SLOT(SetDefConf()));
    CheckDialogA1 *chdlg = new CheckDialogA1;
    TuneDialogA1 *tdlg = new TuneDialogA1;
    connect(this,SIGNAL(FinishAll()),tdlg,SLOT(CancelTune()));
    tdlg->SetStartTuneButtonEnabled(!pc.Emul);
    TuneDialogA1DN *t2dlg = new TuneDialogA1DN;
    t2dlg->SetStartTuneButtonEnabled(!pc.Emul);
//    oscdialog *OscD = new oscdialog;
    downloaddialog *DownD = new downloaddialog;
//    fwupdialog *FwUpD = new fwupdialog;
    MainTW->addTab(tdlg, "Регулировка");
    MainTW->addTab(t2dlg, "Настройка своего ДН");
    MainTW->addTab(chdlg, "Измерения");
//    MainTW->addTab(OscD, "Осциллограммы");
    MainTW->addTab(DownD, "События");
    MainTW->addTab(new A1Dialog, "Поверка внешнего ДН/ТН");
//    MainTW->addTab(FwUpD, "Загрузка ВПО");
    if (pc.ModuleBsi.Hth & HTH_CONFIG) // нет конфигурации
        pc.ErMsg(ER_NOCONF);
    if (pc.ModuleBsi.Hth & HTH_REGPARS) // нет коэффициентов
        pc.ErMsg(ER_NOTUNECOEF);
    MainTW->repaint();
    MainTW->show();
    emit BsiRefresh();
}
