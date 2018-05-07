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
#include <QFileDialog>
#include "coma.h"
#include "../gen/commands.h"
#include "../check/checkdialog21.h"
#include "../check/checkdialog22.h"
#include "../check/checkdialog80.h"
#include "../check/checkdialog85.h"
#include "../check/checkdialoga1.h"
#include "../config/confdialog21.h"
#include "../config/confdialog22.h"
#include "../config/confdialog31.h"
#include "../config/confdialog35.h"
#include "../config/confdialog80.h"
#include "../config/confdialog85.h"
#include "../config/confdialoga1.h"
#include "../dialogs/a1dialog.h"
#include "../dialogs/fwupdialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/errordialog.h"
#include "../tune/tunedialog21.h"
#include "../tune/tunedialog22.h"
#include "../tune/tunedialog80.h"
#include "../tune/tunedialoga1.h"
#include "../tune/tunedialoga1dn.h"
#include "../widgets/etabwidget.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../widgets/waitwidget.h"
#include "../gen/log.h"

Coma::Coma(QWidget *parent)
    : MainWindow(parent)
{
/*    WaitWidget *w = new WaitWidget;
    w->SetMessage("Пожалуйста, подождите...");
    QTime tme;
    tme.start();
    WaitWidget::ww_struct ww;
    ww.isincrement = true;
    ww.isallowedtostop = true;
    ww.format = WaitWidget::WW_TIME;
    ww.initialseconds = 0;
    w->Init(ww);
    w->Start();
    while (tme.elapsed() < 100000)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    w->close(); */
    StartWindowSplashScreen->finish(this);
    Autonomous = false;
}

Coma::~Coma()
{
}

void Coma::SetMode(int mode)
{
    Mode = mode;
}

int Coma::GetMode()
{
    return Mode;
}

void Coma::Go(const QString &parameter)
{
    if (Mode != COMA_GENERALMODE)
    {
        pc.Emul = true;
//        Autonomous = true;
    }
    SetupUI();
    show();
    switch (Mode)
    {
    case COMA_AUTON_OSCMODE:
    {
        LoadOscFromFile(parameter);
        break;
    }
    case COMA_AUTON_PROTMODE:
    {
        StartA1Dialog(parameter);
        break;
    }
    case COMA_AUTON_SWJMODE:
    {
        LoadSwjFromFile(parameter);
        break;
    }
    default:
        break;
    }
}

void Coma::Emul2x()
{
    QMenu *menu = new QMenu(this);
    for (int i=0; i<MTBs.size(); ++i)
    {
        QMenu *inmenu = new QMenu(this);
        QString prefix = QString::number(MTBs.at(i), 16);
        inmenu->setTitle(prefix);
        for (int j=0; j<MTBs.size(); ++j)
        {
            QAction *act = new QAction(this);
            QString suffix = QString::number(MTBs.at(j), 16);
            QString tmps = prefix + suffix;
            act->setText(tmps);
            act->setObjectName(tmps);
            act->setIcon(QIcon(QPixmap::fromImage(*(WDFunc::TwoImages(prefix, suffix)))));
            connect(act,SIGNAL(triggered()),this,SLOT(StartEmul()));
            inmenu->addAction(act);
        }
        menu->addMenu(inmenu);
    }
    menu->popup(QCursor::pos());
}

void Coma::Emul8x()
{
    QMenu *menu = new QMenu(this);
    for (int i=81; i<=85; ++i)
    {
        QString tmps = QString::number(i);
        QAction *act = new QAction(this);
        act->setText(tmps);
        if (i < 84)
        {
            act->setObjectName("80"+tmps);
            act->setIcon(QIcon(QPixmap::fromImage(*(WDFunc::TwoImages("80", tmps)))));
        }
        else
        {
            act->setObjectName(tmps+tmps);
            act->setIcon(QIcon(QPixmap::fromImage(*(WDFunc::TwoImages(tmps, tmps)))));
        }
        connect(act,SIGNAL(triggered()),this,SLOT(StartEmul()));
        menu->addAction(act);
    }
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
    QAction *act;
    setMinimumHeight(650);

    QHBoxLayout *hlyout = new QHBoxLayout;
    QToolBar *tb = new QToolBar;
    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(20,20));
    if (!Autonomous)
    {
        act = new QAction(this);
        act->setToolTip("Соединение");
        act->setIcon(QIcon("images/play.png"));
        connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
        tb->addAction(act);
    }
    act = new QAction(this);
    act->setToolTip("Разрыв соединения");
    act->setIcon(QIcon("images/stop.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(DisconnectAndClear()));
    tb->addAction(act);
    tb->addSeparator();
#if PROGSIZE >= PROGSIZE_FULL
    AddUIToToolbar(tb);
#endif
    if (Autonomous)
        AddUIToToolbar(tb);
    act = new QAction(this);
    act->setToolTip("Настройки");
    act->setIcon(QIcon("images/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Протокол ошибок");
    act->setIcon(QIcon("images/skull-and-bones.png"));
    connect(act,SIGNAL(triggered(bool)),this,SLOT(ShowErrorDialog()));
    tb->addAction(act);
    hlyout->addWidget(tb);
    hlyout->addWidget(HthWidget());
    lyout->addLayout(hlyout);
    lyout->addWidget(Least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
#if PROGSIZE >= PROGSIZE_LARGE
        SetSlideWidget();
#endif
    SetupMenubar();
}

void Coma::AddUIToToolbar(QToolBar *tb)
{
    QAction *act = new QAction(this);
    act->setToolTip("Эмуляция 21");
    act->setIcon(QIcon("images/2x.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Emul2x()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Эмуляция 8x");
    act->setIcon(QIcon("images/8x.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Emul8x()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Эмуляция A1");
    act->setIcon(QIcon("images/a1.png"));
//    quint16 MType = MTB_A1;
//    MType = (MType << 8) | MTM_00;
    act->setObjectName(QString::number(MTB_A1, 16)); // для слота StartEmul
    connect(act,SIGNAL(triggered()),this,SLOT(StartEmul()));
    tb->addAction(act);
    tb->addSeparator();
}

void Coma::AddActionsToMenuBar(QMenuBar *menubar)
{
    QMenu *menu = new QMenu;
    menu->setTitle("Модуль А1");
    QAction *act = new QAction(this);
    act->setText("Протокол из файла (A1)");
    connect(act,SIGNAL(triggered()),this,SLOT(ProtocolFromFile()));
    menu->addAction(act);
    menubar->addMenu(menu);
}

void Coma::Stage3()
{
    QString str;
    ConfB = ConfM = 0;
    CheckB = CheckM = 0;
    if (!Autonomous)
        TuneB = TuneM = 0;
    ClearTW();
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == 0)
        return;

    InfoDialog *idlg = new InfoDialog;
    connect(this,SIGNAL(BsiRefresh()),idlg,SLOT(FillBsi()));
    connect(this,SIGNAL(ClearBsi()),idlg,SLOT(ClearBsi()));
    MainTW->addTab(idlg, "Информация");
    if (pc.ModuleBsi.MTypeB < 0xA0) // диапазон модулей АВ-ТУК
    {
        MainConfDialog = new ConfDialog(S2Config);
        MainTW->addTab(MainConfDialog, "Конфигурирование\nОбщие");
    }
    PrepareDialogs();
    if (ConfB != 0)
    {
        str = (ConfM == 0) ? "Конфигурирование" : "Конфигурирование\nБазовая";
        MainTW->addTab(ConfB, str);
        if (!Autonomous)
        {
            connect(ConfB,SIGNAL(NewConfLoaded()),this,SLOT(Fill()));
            connect(ConfB,SIGNAL(LoadDefConf()),this,SLOT(SetDefConf()));
        }
    }
    if (ConfM != 0)
    {
        str = (ConfB == 0) ? "Конфигурирование" : "Конфигурирование\nМезонин";
        MainTW->addTab(ConfM, str);
        if (!Autonomous)
        {
            connect(ConfM,SIGNAL(NewConfLoaded()),this,SLOT(Fill()));
            connect(ConfM,SIGNAL(LoadDefConf()),this,SLOT(SetDefConf()));
        }
    }
    str = (TuneM == 0) ? "Регулировка" : "Регулировка\nБазовая";
    if (TuneB != 0)
        MainTW->addTab(TuneB, str);
    str = (TuneB == 0) ? "Регулировка" : "Регулировка\nМезонин";
    if (TuneM != 0)
        MainTW->addTab(TuneM, str);
    str = (CheckM == 0) ? "Проверка" : "Проверка\nБазовая";
    if (CheckB != 0)
        MainTW->addTab(CheckB, str);
    str = (CheckB == 0) ? "Проверка" : "Проверка\nМезонин";
    if (CheckM != 0)
        MainTW->addTab(CheckM, str);
    if ((pc.ModuleBsi.MTypeB << 8) == MTB_A1)
    {
        MainTW->addTab(new TuneDialogA1DN, "Настройка своего ДН");
        MainTW->addTab(new A1Dialog, "Поверка внешнего ДН/ТН");
    }
    OscD = new OscDialog(OscFunc);
    fwupdialog *FwUpD = new fwupdialog;
    MainTW->addTab(OscD, "Осциллограммы");
    if (SwjD != 0)
        MainTW->addTab(SwjD, "Журнал переключений");
    MainTW->addTab(FwUpD, "Загрузка ВПО");
    if (pc.ModuleBsi.Hth & HTH_CONFIG) // нет конфигурации
        pc.ErMsg(ER_NOCONF);
    if (pc.ModuleBsi.Hth & HTH_REGPARS) // нет коэффициентов
        pc.ErMsg(ER_NOTUNECOEF);
    MainTW->repaint();
    MainTW->show();
    emit BsiRefresh();
}

void Coma::PrepareDialogs()
{
    quint32 MTypeB = pc.ModuleBsi.MTypeB << 8;
    switch(MTypeB)
    {
    case MTB_21:
    {
        ConfB = new ConfDialog21(S2Config, true);
        TuneB = new TuneDialog21(BT_BASE);
        CheckB = new CheckDialog21(BT_BASE);
        break;
    }
    case MTB_22:
    {
        ConfB = new ConfDialog22(S2Config, true);
        TuneB = new TuneDialog22(BT_BASE);
        CheckB = new CheckDialog22(BT_BASE);
        break;
    }
    case MTB_31:
    {
        ConfB = new ConfDialog31(S2Config, true);
        break;
    }
    case MTB_35:
    {
        ConfB = new ConfDialog35(S2Config, true);
        break;
    }
    case MTB_80:
    {
//        ConfB = new ConfDialog80(S2Config);
//        TuneB = new TuneDialog80(S2Config);
        CheckB = new CheckDialog80(BT_BASE);
        break;
    }
    case MTB_A1:
    {
        ConfB = new ConfDialogA1(S2Config);
        CheckB = new CheckDialogA1(BT_BASE);
        TuneB = new TuneDialogA1;
        break;
    }
    default:
        break;
    }
    switch(pc.ModuleBsi.MTypeM)
    {
    case MTM_21:
    {
        ConfM = new ConfDialog21(S2Config, false);
        CheckM = new CheckDialog21(BT_MEZONIN);
        TuneM = new TuneDialog21(BT_MEZONIN);
        break;
    }
    case MTM_22:
    {
        ConfM = new ConfDialog22(S2Config, false);
        CheckM = new CheckDialog22(BT_MEZONIN);
        TuneM = new TuneDialog22(BT_MEZONIN);
        break;
    }
    case MTM_31:
    {
        ConfM = new ConfDialog31(S2Config, true);
        break;
    }
    case MTM_35:
    {
        ConfM = new ConfDialog35(S2Config, true);
        break;
    }
    case MTM_81:
    case MTM_82:
    case MTM_83:
    {
        ConfM = new ConfDialog80(S2Config);
        TuneM = new TuneDialog80(S2Config);
        break;
    }
    case MTM_85:
    {
        ConfM = new ConfDialog85(S2Config);
        CheckM = new CheckDialog85(BT_BASE);
        SwjD = new SwitchJournalDialog;
        break;
    }
    default: // 0x00
        break;
    }
}

void Coma::StartA1Dialog(const QString &filename)
{
    A1Dialog *adlg = new A1Dialog(filename);
    delete adlg;
}

void Coma::ProtocolFromFile()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getOpenFileName(this, "Открыть файл", pc.HomeDir, "PKDN verification files (*.vrf)", Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    dlg->close();
    StartA1Dialog(filename);
}
