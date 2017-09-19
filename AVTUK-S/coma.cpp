/*
 * COMA is the "COMplex for AV-TUK", i.e. the program to make a communication
 * with the modules of automatic controller AV-TUK by AVM-Energo LLC
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

#include <QLabel>
#include <QProgressBar>
#include <QMenu>
#include <QMenuBar>
#include <QTextEdit>
#include <QToolBar>
#include <QSettings>
#include <QDialog>
#include <QCursor>
#include "coma.h"
#include "../commands.h"
#include "../check/checkdialog21.h"
#include "../check/checkdialog80.h"
#include "../check/checkdialoga1.h"
#include "../config/confdialog21.h"
#include "../config/confdialog80.h"
#include "../config/confdialoga1.h"
#include "../dialogs/fwupdialog.h"
#include "../dialogs/downloaddialog.h"
#include "../dialogs/oscdialog.h"
#include "../dialogs/hiddendialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/errordialog.h"
#include "../tune/tunedialog21.h"
#include "../tune/tunedialog80.h"
#include "../tune/tunedialoga1.h"
#include "../widgets/mytabwidget.h"
#include "../widgets/waitwidget.h"
#include "../widgets/messagebox.h"
#include "../widgets/s_tqspinbox.h"
#include "../widgets/wd_func.h"
#include "../log.h"

Coma::Coma(QWidget *parent)
    : MainWindow(parent)
{
    SetupMenubar();
    SetupUI();
}

Coma::~Coma()
{
}

void Coma::Emul2x()
{
    QMenu *menu = new QMenu(this);
    QAction *act = new QAction(this);
    act->setText("2100");
    act->setObjectName("2100");
    act->setIcon(QIcon(QPixmap::fromImage(*(WDFunc::TwoImages("21", "00")))));
    connect(act,SIGNAL(triggered()),this,SLOT(StartEmul()));
    menu->addAction(act);

    act = new QAction(this);
    act->setText("2121");
    act->setObjectName("2121");
    act->setIcon(QIcon(QPixmap::fromImage(*(WDFunc::TwoImages("21", "21")))));
    connect(act,SIGNAL(triggered()),this,SLOT(StartEmul()));
    menu->addAction(act);

    menu->popup(QCursor::pos());
}

void Coma::SetupUI()
{
    setWindowTitle(PROGCAPTION);
    QString tmps = "QMainWindow {background-color: "+QString(MAINWINCLR)+";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(800, 600));
    QWidget *wdgt = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
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
    connect(act,SIGNAL(triggered()),this,SLOT(DisconnectAndClear()));
    tb->addAction(act);
    tb->addSeparator();
    act = new QAction(this);
    act->setToolTip("Эмуляция 21");
    act->setIcon(QIcon(":/pic/2x.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Emul2x()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Эмуляция 8x");
    act->setIcon(QIcon(":/pic/8x.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Emul8x()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Эмуляция A1");
    act->setIcon(QIcon(":/pic/a1.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(EmulA1()));
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

    for (int i = (MAXERRORFLAGNUM-1); i >= 0; i--)
    {
        QLabel *lbl = new QLabel(Hth().at(i));
        lbl->setObjectName("hth"+QString::number(i));
        lbl->setToolTip(HthToolTip().at(i));
        lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);" \
                           "background: 0px; margin: 0px; spacing: 0; padding: 0px;}");
        hlyout->addWidget(lbl);
    }
    lyout->addLayout(hlyout);

    QHBoxLayout *inlyout = new QHBoxLayout;
    MyTabWidget *MainTW = new MyTabWidget;
    MainTW->setObjectName("maintw");
    MainTW->setTabPosition(QTabWidget::West);
    inlyout->addWidget(MainTW, 60);
    MainTW->hide();

    lyout->addLayout(inlyout, 90);
    lyout->addStretch(500);

    QFrame *line = new QFrame;
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    line->setFrameStyle(QFrame::Sunken | QFrame::HLine);
    lyout->addWidget(line);
    inlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel;
    lbl->setObjectName("prblbl");
    inlyout->addWidget(lbl);
    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prbprb");
    prb->setOrientation(Qt::Horizontal);
    prb->setMinimumWidth(500);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);

    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);

/*    if (pc.result)
        MessageBox2::information(this, "Внимание", "Не найден файл с сообщениями об ошибках!"); */

    QWidget *SlideWidget = new QWidget(this);
    SlideWidget->setObjectName("slidew");
    SlideWidget->setStyleSheet("QWidget {background-color: rgba(110,234,145,255);}");
    QVBoxLayout *slyout = new QVBoxLayout;

    WDFunc::AddLabelAndLineedit(slyout, "Модуль АВТУК-", "mtypele");
    WDFunc::AddLabelAndLineedit(slyout, "Аппаратная версия базовой платы:", "hwverble");
    WDFunc::AddLabelAndLineedit(slyout, "Аппаратная версия мезонинной платы:", "hwvermle");
    WDFunc::AddLabelAndLineedit(slyout, "Серийный номер модуля:", "snle");
    WDFunc::AddLabelAndLineedit(slyout, "Серийный номер базовой платы:", "snble");
    WDFunc::AddLabelAndLineedit(slyout, "Серийный номер мезонинной платы:", "snmle");
    WDFunc::AddLabelAndLineedit(slyout, "Версия ПО:", "fwverle");
    WDFunc::AddLabelAndLineedit(slyout, "КС конфигурации:", "cfcrcle");
    WDFunc::AddLabelAndLineedit(slyout, "Последний сброс:", "rstle");
    WDFunc::AddLabelAndLineedit(slyout, "Количество сбросов:", "rstcountle");
    WDFunc::AddLabelAndLineedit(slyout, "ИД процессора:", "cpuidle", true);
    QTextEdit *MainTE = new QTextEdit;
    MainTE->setObjectName("mainte");
    slyout->addWidget(MainTE, 40);

    SlideWidget->setLayout(slyout);
    SlideWidget->setMinimumWidth(250);
    SlideWidget->hide();
    SWGeometry = SlideWidget->geometry();
    SWHide = true;
}

void Coma::AddActionsToMenuBar(QMenuBar *menubar)
{
    QMenu *menu = new QMenu;
    menu->setTitle("Запуск...");
    QAction *act = new QAction(this);
    act->setText("...в режиме А");
    act->setIcon(QIcon(":/pic/a.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(EmulA()));
    menu->addAction(act);
    act = new QAction(this);
    act->setIcon(QIcon(":/pic/e.png"));
    act->setText("...в режиме Э");
    connect(act,SIGNAL(triggered()),this,SLOT(EmulE()));
    menu->addAction(act);
    menubar->addMenu(menu);
}

void Coma::Stage3()
{
    ClearTW();
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
/*    DownDialog = new downloaddialog;
    FwUpDialog = new fwupdialog;
    OscDialog = new oscdialog; */
/*    switch(pc.ModuleBsi.MTypeB)
    {
    case MTB_21:
    {
        MainConfDialog = new ConfDialog(S2Config);
        MainTW->addTab(MainConfDialog, "Конфигурирование\nОбщие");
        ConfDialog21 *Dialog21 = new ConfDialog21(S2Config, true);
        ConfB = Dialog21;
        TuneDialog21 *TD21 = new TuneDialog21;
        TuneD = TD21;
        break;
    }
    case MTB_80:
    {
        MainConfDialog = new ConfDialog(S2Config);
        MainTW->addTab(MainConfDialog, "Конфигурирование\nОбщие");
        ConfDialog80 *D80 = new ConfDialog80(S2Config);
        ConfB = D80;
        TuneDialog80 *TD80 = new TuneDialog80(S2Config);
        TuneD = TD80;
        CheckDialog80 *CD80 = new CheckDialog80;
        CheckD = CD80;
        break;
    }
    case MTB_A1:
    {
        ConfDialogA1 *DialogA1 = new ConfDialogA1(S2Config);
        ConfB = DialogA1;
        CheckDialogA1 *CDA1 = new CheckDialogA1;
        CheckD = CDA1;
        TuneDialogA1 *TDA1 = new TuneDialogA1;
        TuneD = TDA1;
        break;
    }
    }
    switch(pc.ModuleBsi.MTypeM)
    {
    case MTM_21:
    {
        ConfDialog21 *Dialog21 = new ConfDialog21(S2Config, false);
        ConfM = Dialog21;
        break;
    }
    case MTM_81:
    case MTM_82:
    case MTM_83:
//        ConfDialog80 *Dialog80 = new ConfDialog80(S2Config);
    {
        ConfM = new ConfDialog80(S2Config);
        TuneD = new TuneDialog80(S2Config);
        break;
    }
    default: // 0x00
        break;
    }
    if (ConfB != 0)
    {
        MainTW->addTab(ConfB, "Конфигурирование\nБазовая");
        connect(ConfB,SIGNAL(NewConfLoaded()),this,SLOT(Fill()));
        connect(ConfB,SIGNAL(LoadDefConf()),this,SLOT(SetDefConf()));
    }
    if (ConfM != 0)
    {
        MainTW->addTab(ConfM, "Конфигурирование\nМезонин");
        connect(ConfM,SIGNAL(NewConfLoaded()),this,SLOT(Fill()));
        connect(ConfM,SIGNAL(LoadDefConf()),this,SLOT(SetDefConf()));
    }
    if (TuneD != 0)
        MainTW->addTab(TuneD, "Регулировка");
    if (CheckD != 0)
        MainTW->addTab(CheckD, "Проверка");
    oscdialog *OscD = new oscdialog;
    downloaddialog *DownD = new downloaddialog;
    fwupdialog *FwUpD = new fwupdialog;
    MainTW->addTab(OscD, "Осциллограммы");
    MainTW->addTab(DownD, "События");
    MainTW->addTab(FwUpD, "Загрузка ВПО");
    if (pc.ModuleBsi.Hth & HTH_CONFIG) // нет конфигурации
        pc.ErMsg(ER_NOCONF);
    if (pc.ModuleBsi.Hth & HTH_REGPARS) // нет коэффициентов
        pc.ErMsg(ER_NOTUNECOEF); */
    MainTW->repaint();
    MainTW->show();
}
