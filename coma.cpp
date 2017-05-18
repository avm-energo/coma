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

#include <QCoreApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QScrollBar>
#include <QStringListModel>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QProgressBar>
#include <QMenu>
#include <QMenuBar>
#include <QFont>
#include <QTextEdit>
#include <QToolBar>
#include <QSettings>
#include <QDialog>
#include <QSizePolicy>
#include <QCheckBox>
#include <QDir>
#include <QtSerialPort/QSerialPortInfo>
#include "coma.h"
#include "commands.h"
#include "widgets/mytabwidget.h"
#include "widgets/waitwidget.h"
#include "widgets/messagebox.h"
#include "widgets/s_tqspinbox.h"
#include "widgets/wd_func.h"
#include "dialogs/hiddendialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/errordialog.h"
#include "log.h"

Coma::Coma(QWidget *parent)
    : QMainWindow(parent)
{
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(pc.HomeDir);
    if (!dir.exists())
        dir.mkpath(".");
    ReconTry = 0;
    cn = new Canal;
    S2Config.clear();
    SetupUI();
    SetupMenubar();
    PrepareTimers();
    LoadSettings();
}

Coma::~Coma()
{
    SaveSettings();
}

void Coma::SetupUI()
{
    setWindowTitle(PROGNAME);
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
    QAction *ConnectAction = new QAction(this);
    ConnectAction->setToolTip("Соединение");
    ConnectAction->setIcon(QIcon(":/pic/play.png"));
    connect(ConnectAction,SIGNAL(triggered()),this,SLOT(Stage1_5()));
    QAction *DisconnectAction = new QAction(this);
    DisconnectAction->setToolTip("Разрыв соединения");
    DisconnectAction->setIcon(QIcon(":/pic/stop.png"));
    connect(DisconnectAction,SIGNAL(triggered()),this,SLOT(Disconnect()));
    QAction *EmulAAction = new QAction(this);
    EmulAAction->setToolTip("Запуск в режиме А");
    EmulAAction->setIcon(QIcon(":/pic/a.png"));
    connect(EmulAAction,SIGNAL(triggered()),this,SLOT(EmulA()));
    QAction *EmulEAction = new QAction(this);
    EmulEAction->setToolTip("Запуск в режиме Э");
    EmulEAction->setIcon(QIcon(":/pic/e.png"));
    connect(EmulEAction,SIGNAL(triggered()),this,SLOT(EmulE()));
    QAction *SettingsAction = new QAction(this);
    SettingsAction->setToolTip("Настройки");
    SettingsAction->setIcon(QIcon(":/pic/settings.png"));
    connect(SettingsAction,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    tb->addAction(ConnectAction);
    tb->addAction(DisconnectAction);
    tb->addSeparator();
    tb->addAction(EmulAAction);
    tb->addAction(EmulEAction);
    tb->addSeparator();
    tb->addAction(SettingsAction);
    QAction *act = new QAction(this);
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

    if (pc.result)
        MessageBox2::information(this, "Внимание", "Не найден файл с сообщениями об ошибках!");

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

void Coma::SetupMenubar()
{
    QMenuBar *MainMenuBar = new QMenuBar;
    QString tmps = "QMenuBar {background-color: "+QString(MAINWINCLR)+";}"\
            "QMenuBar::item {background-color: "+QString(MAINWINCLR)+";}";
    MainMenuBar->setStyleSheet(tmps);
    QMenu *MainMenu = new QMenu;
    MainMenu->setTitle("Главное");
    QAction *act = new QAction(this);
    act->setText("Выход");
    connect(act,SIGNAL(triggered()),this,SLOT(Exit()));
    MainMenu->addAction(act);
    act = new QAction(this);
    act->setText("Соединение");
    act->setIcon(QIcon(":/pic/play.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
    MainMenu->addAction(act);
    act = new QAction(this);
    act->setText("Разрыв соединения");
    act->setIcon(QIcon(":/pic/stop.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Disconnect()));
    MainMenu->addAction(act);
    MainMenuBar->addMenu(MainMenu);

    QMenu *menu = new QMenu;
    menu->setTitle("Запуск...");
    act = new QAction(this);
    act->setText("...в режиме А");
    act->setIcon(QIcon(":/pic/a.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(EmulA()));
    menu->addAction(act);
    act = new QAction(this);
    act->setIcon(QIcon(":/pic/e.png"));
    act->setText("...в режиме Э");
    connect(act,SIGNAL(triggered()),this,SLOT(EmulE()));
    menu->addAction(act);
    MainMenuBar->addMenu(menu);

    menu = new QMenu;
    menu->setTitle("Разработка");
    WriteSNAction = new QAction(this);
    WriteSNAction->setText("Запись Hidden Block");
    connect(WriteSNAction,SIGNAL(triggered()),this,SLOT(OpenBhbDialog()));
    menu->addAction(WriteSNAction);
    MainMenuBar->addMenu(menu);

    menu = new QMenu;
    menu->setTitle("Настройки");
    act = new QAction(this);
    act->setText("Настройки");
    act->setIcon(QIcon(":/pic/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    menu->addAction(act);
    MainMenuBar->addMenu(menu);

    act = new QAction(this);
    act->setText("О программе");
    connect(act,SIGNAL(triggered()),this,SLOT(GetAbout()));
    MainMenuBar->addAction(act);
    setMenuBar(MainMenuBar);
}

void Coma::LoadSettings()
{
    QSettings *sets = new QSettings ("EvelSoft","COMA");
    pc.MIPASDU = sets->value("mip/asdu","206").toInt();
    pc.MIPIP = sets->value("mip/ip","172.16.30.11").toString();
    pc.Port = sets->value("Port", "COM1").toString();
    pc.ErrWindowDelay = sets->value("ErrWindowDelay","5").toInt();
    pc.ShowErrWindow = sets->value("ShowErrWindow","1").toBool();
}

void Coma::SaveSettings()
{
    QSettings *sets = new QSettings ("EvelSoft","COMA");
    sets->setValue("mip/asdu",pc.MIPASDU);
    sets->setValue("mip/ip",pc.MIPIP);
    sets->setValue("Port", pc.Port);
    sets->setValue("ErrWindowDelay", pc.ErrWindowDelay);
    sets->setValue("ShowErrWindow", pc.ShowErrWindow);
}

void Coma::PrepareTimers()
{
    QTimer *MouseTimer = new QTimer;
    connect(MouseTimer,SIGNAL(timeout()),this,SLOT(MouseMove()));
    MouseTimer->start(50);
}

void Coma::Stage1()
{
    int i;
    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QComboBox *portscb = new QComboBox;
    connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetPort(QString)));
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
    if (info.size() == 0)
    {
        pc.ErMsg(USB_NOCOMER);
        return;
    }
    QStringListModel *tmpmodel = new QStringListModel;
    QStringList tmpsl;
    for (i = 0; i < info.size(); i++)
        tmpsl << info.at(i).portName();
    SetPort(tmpsl.at(0));
    tmpmodel->setStringList(tmpsl);
    portscb->setModel(tmpmodel);
    lyout->addWidget(portscb);

    QPushButton *nextL = new QPushButton("Далее");
    connect(nextL,SIGNAL(clicked()),this,SLOT(Stage1_5()));
    lyout->addWidget(nextL);
    dlg->setLayout(lyout);
    dlg->exec();
}

void Coma::Stage1_5()
{
    QDialog *dlg = this->findChild<QDialog *>("connectdlg");
    if (dlg != 0)
        dlg->close();
    pc.PrbMessage = "Загрузка данных...";

    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
    if (info.size() == 0)
    {
        pc.ErMsg(USB_NOCOMER);
        Stage1();
        return;
    }
    bool PortFound = false;
    for (int i = 0; i < info.size(); i++)
    {
        if (info.at(i).portName() == pc.Port)
        {
            PortFound = true;
            cn->info = info.at(i);
        }
    }
    if (!PortFound)
    {
        pc.ErMsg(USB_COMER);
        Stage1();
        return;
    }
    cn->baud = 115200;
    if (!cn->Connect())
    {
        Stage1();
        return;
    }
    connect(cn,SIGNAL(oscerasesize(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    connect(cn,SIGNAL(osceraseremaining(quint32)),this,SLOT(SetProgressBar(quint32)));
    connect(cn,SIGNAL(incomingdatalength(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    connect(cn,SIGNAL(bytesreceived(quint32)),this,SLOT(SetProgressBar(quint32)));
    connect(cn,SIGNAL(sendend()),this,SLOT(DisableProgressBar()));
    connect(cn,SIGNAL(readbytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(cn,SIGNAL(writebytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    Stage2();
}

void Coma::SetPort(QString str)
{
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
    for (int i = 0; i < info.size(); i++)
    {
        if (info.at(i).portName() == str)
        {
            pc.Port = str;
            return;
        }
    }
}

void Coma::Stage2()
{
    if (CN_GetBsi(&pc.ModuleBsi, sizeof(publicclass::Bsi)) != NOERROR)
    {
        cn->Disconnect();
        Stage1();
    }
    pc.MType = ((pc.ModuleBsi.MTypeB & 0x000000FF) << 8) | (pc.ModuleBsi.MTypeM & 0x000000FF);
    pc.ModuleTypeString = "АВ-ТУК-";
    pc.ModuleTypeString.append(QString::number(pc.MType, 16));
    if ((pc.ModuleBsi.SerialNumB == 0xFFFFFFFF) || ((pc.ModuleBsi.SerialNumM == 0xFFFFFFFF) && (pc.ModuleBsi.MTypeM != MTM_00)) || \
            (pc.ModuleBsi.SerialNum == 0xFFFFFFFF)) // серийный номер не задан, выдадим предупреждение
        OpenBhbDialog();
    FillBsi();
    Stage3();
}

void Coma::FillBsi()
{
    WDFunc::SetLEData(this, "mtypele", pc.ModuleTypeString);
    WDFunc::SetLEData(this, "hwverble", pc.VerToStr(pc.ModuleBsi.HwverB));
    WDFunc::SetLEData(this, "hwvermle", pc.VerToStr(pc.ModuleBsi.HwverM));
    WDFunc::SetLEData(this, "fwverle", pc.VerToStr(pc.ModuleBsi.Fwver));
    WDFunc::SetLEData(this, "cfcrcle", "0x"+QString::number(static_cast<uint>(pc.ModuleBsi.Cfcrc), 16));
    WDFunc::SetLEData(this, "rstle", "0x"+QString::number(pc.ModuleBsi.Rst, 16));

    // расшифровка Hth
    for (int i = 0; i < MAXERRORFLAGNUM; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("hth"+QString::number(i));
        if (lbl == 0)
            return;
        quint32 tmpui = (0x00000001 << i) & pc.ModuleBsi.Hth;
        if (tmpui)
            lbl->setStyleSheet("QLabel {background-color: rgba(255,10,10,255); color: rgba(255,255,255,255);}");
        else
            lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");
    }
    WDFunc::SetLEData(this, "rstcountle", QString::number(pc.ModuleBsi.RstCount, 16));
    WDFunc::SetLEData(this, "cpuidle", QString::number(pc.ModuleBsi.UIDHigh, 16)+QString::number(pc.ModuleBsi.UIDMid, 16)+QString::number(pc.ModuleBsi.UIDLow, 16));
    WDFunc::SetLEData(this, "snle", QString::number(pc.ModuleBsi.SerialNum, 16));
    WDFunc::SetLEData(this, "snble", QString::number(pc.ModuleBsi.SerialNumB, 16));
    WDFunc::SetLEData(this, "snmle", QString::number(pc.ModuleBsi.SerialNumM, 16));
}

void Coma::ClearBsi()
{
    WDFunc::SetLEData(this, "mtypele", "");
    WDFunc::SetLEData(this, "hwverble", "");
    WDFunc::SetLEData(this, "hwvermle", "");
    WDFunc::SetLEData(this, "fwverle", "");
    WDFunc::SetLEData(this, "cfcrcle", "");
    WDFunc::SetLEData(this, "rstle", "");

    // расшифровка Hth
    for (int i = 0; i < 32; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("hth"+QString::number(i));
        if (lbl == 0)
        {
            DBGMSG;
            return;
        }
        lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");
    }
    WDFunc::SetLEData(this, "rstcountle", "");
    WDFunc::SetLEData(this, "cpuidle", "");
    WDFunc::SetLEData(this, "snle", "");
    WDFunc::SetLEData(this, "snble", "");
    WDFunc::SetLEData(this, "snmle", "");
}

void Coma::OpenBhbDialog()
{
    HiddenDialog *dlg = new HiddenDialog(HiddenDialog::BYMY);
    pc.BoardBBhb.HWVer = pc.ModuleBsi.HwverB;
    pc.BoardBBhb.ModSerialNum = pc.ModuleBsi.SerialNum;
    pc.BoardBBhb.SerialNum = pc.ModuleBsi.SerialNumB;
    pc.BoardBBhb.MType = pc.ModuleBsi.MTypeB;
    pc.BoardMBhb.MType = pc.ModuleBsi.MTypeM;
    pc.BoardMBhb.HWVer = pc.ModuleBsi.HwverM;
    pc.BoardMBhb.SerialNum = pc.ModuleBsi.SerialNumM;
    pc.BoardMBhb.ModSerialNum = 0xFFFFFFFF;
    dlg->Fill(); // заполняем диалог из недавно присвоенных значений
    dlg->exec();
    if (CN_GetBsi(&pc.ModuleBsi, sizeof(publicclass::Bsi)) != NOERROR)
    {
        cn->Disconnect();
        Stage1();
    }
}

void Coma::Stage3()
{
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    DownDialog = new downloaddialog;
    FwUpDialog = new fwupdialog;
    OscDialog = new oscdialog;
    MainConfDialog = new ConfDialog(S2Config);
    MainTW->addTab(MainConfDialog, "Конфигурирование\nОбщие");
    switch(pc.ModuleBsi.MTypeB)
    {
    case MTB_21:
        ConfDialog21 *Dialog21 = new ConfDialog21(S2Config, true);
        MainTW->addTab(Dialog21, "Конфигурирование\nБазовая");
        ConfB = Dialog21;
        break;
    }
    switch(pc.ModuleBsi.MTypeM)
    {
    case MTM_21:
        ConfDialog21 *Dialog21 = new ConfDialog21(S2Config, false);
        MainTW->addTab(Dialog21, "Конфигурирование\nМезонин");
        ConfM = Dialog21;
        break;
    }
    connect(ConfB,SIGNAL(NewConfLoaded()),this,SLOT(Fill()));
    connect(ConfM,SIGNAL(NewConfLoaded()),this,SLOT(Fill()));
    connect(ConfB,SIGNAL(LoadDefConf()),this,SLOT(SetDefConf()));
    connect(ConfM,SIGNAL(LoadDefConf()),this,SLOT(SetDefConf()));
/*    if ((pc.ModuleBsi.MTypeB > 0x1F) && (pc.ModuleBsi.MTypeB < 0x30))
    {
        ATuneDialog = new a_tunedialog;
        ACheckDialog = new a_checkdialog;
        MainTW->addTab(ATuneDialog, "Настройка");
        MainTW->addTab(ACheckDialog, "Проверка");
        MainTW->addTab(OscDialog, "Осциллограммы");
        MainTW->addTab(DownDialog, "События");
        MainTW->addTab(FwUpDialog, "Загрузка ВПО");
        connect(Dialog21,SIGNAL(BsiIsNeedToBeAcquiredAndChecked()),this,SLOT(Stage2()));
    }
    if ((pc.ModuleBsi.MTypeB > 0x7F) && (pc.ModuleBsi.MTypeB < 0x90))
    {
        EConfDialog = new confdialog_80;
        ETuneDialog = new e_tunedialog;
        ECheckDialog = new e_checkdialog;
        MainTW->addTab(EConfDialog, "Конфигурирование");
        MainTW->addTab(ETuneDialog, "Настройка");
        MainTW->addTab(ECheckDialog, "Проверка");
        MainTW->addTab(OscDialog, "Осциллограммы");
        MainTW->addTab(DownDialog, "События");
        MainTW->addTab(FwUpDialog, "Загрузка ВПО");
        connect(EConfDialog,SIGNAL(BsiIsNeedToBeAcquiredAndChecked()),this,SLOT(Stage2()));
    } */
    if (pc.ModuleBsi.Hth & HTH_CONFIG) // нет конфигурации
        pc.ErMsg(ER_NOCONF);
    if (pc.ModuleBsi.Hth & HTH_REGPARS) // нет коэффициентов
        pc.ErMsg(ER_NOTUNECOEF);
    MainTW->repaint();
    MainTW->show();
}

void Coma::Disconnect()
{
    if (!pc.Emul)
        cn->Disconnect();
    ClearBsi();
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    while (MainTW->count())
    {
        QWidget *wdgt = MainTW->widget(0);
        MainTW->removeTab(0);
        delete wdgt;
    }
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != 0)
        MainTE->clear();
    MainTW->hide();
    pc.Emul = false;
}

void Coma::GetAbout()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *l2yout = new QVBoxLayout;
    QLabel *lbl = new QLabel("КОМплекс АВ-ТУК (КОМА)");
    l2yout->addWidget(lbl);
    QString tmps = "Версия "+QString(PROGNAME);
    lbl = new QLabel(tmps);
    l2yout->addWidget(lbl);
    lbl = new QLabel("накорябано Ёвелем");
    l2yout->addWidget(lbl);
    lbl = new QLabel("в 2015-2017 гг.");
    l2yout->addWidget(lbl);
    l2yout->addStretch(10);
    lbl = new QLabel;
    QPixmap pmp;
    pmp.load(":/pic/evel.png");
    lbl->setPixmap(pmp);
    lbl->setMaximumSize(64,64);
    hlyout->addWidget(lbl,1);
    hlyout->setAlignment(lbl,Qt::AlignTop);
    hlyout->addLayout(l2yout,100);
    lyout->addLayout(hlyout,1);
    QPushButton *pb = new QPushButton("Ага");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->exec();
}

void Coma::Exit()
{
    this->close();
}

void Coma::EmulA()
{
    if (pc.Emul) // если уже в режиме эмуляции, выход
        return;
    pc.ModuleBsi.MTypeB = MTB_21;
    pc.ModuleBsi.MTypeM = MTM_21;
    pc.ModuleBsi.SerialNum = 0x12345678;
    pc.ModuleBsi.Hth = 0x00;
    pc.Emul = true;
    Stage3();
}

void Coma::EmulE()
{
    if (pc.Emul) // если уже в режиме эмуляции, выход
        return;
    pc.ModuleBsi.MTypeB = MTB_80;
    pc.ModuleBsi.MTypeM = MTM_82;
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("emuledlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("Выберите тип модуля:");
    hlyout->addWidget(lbl);
    QStringListModel *mdl = new QStringListModel;
    QStringList sl = QStringList() << "8081 (0I6U)" << "8082 (3I3U)" << "8083 (6I0U)";
    mdl->setStringList(sl);
    QComboBox *cb = new QComboBox;
    cb->setModel(mdl);
    cb->setObjectName("extxn");
    hlyout->addWidget(cb);
    lyout->addLayout(hlyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartEmulE()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    QSizePolicy fixed(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dlg->setSizePolicy(fixed);
    dlg->exec();
}

void Coma::StartEmulE()
{
    QDialog *dlg = this->findChild<QDialog *>("emuledlg");
    QComboBox *cb = this->findChild<QComboBox *>("extxn");
    if ((dlg == 0) || (cb == 0))
    {
        DBGMSG;
        return;
    }
    pc.ModuleBsi.MTypeB = MTB_80;
    switch (cb->currentIndex())
    {
    case 0:
        pc.ModuleBsi.MTypeM = MTM_81;
        break;
    case 1:
        pc.ModuleBsi.MTypeB = MTM_82;
        break;
    case 2:
        pc.ModuleBsi.MTypeB = MTM_83;
        break;
    default:
        DBGMSG;
        return;
        break;
    }
    dlg->close();

    Bsi_block.SerialNum = 0x12345678;
    Bsi_block.Hth = 0x00;
    pc.Emul = true;
    Stage3();
}

void Coma::StartSettingsDialog()
{
    SettingsDialog *dlg = new SettingsDialog;
    dlg->exec();
}

void Coma::UpdateMainTE(QByteArray ba)
{
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    QString tmpString;
    if (MainTE != 0)
    {
        for (int i = 0; i < ba.size(); i++)
            tmpString.append(ByteToHex(ba.at(i)));
        MainTE->append(tmpString);
        tmpString = MainTE->toPlainText();
        if (tmpString.size() > C_TE_MAXSIZE)
            MainTE->setPlainText(tmpString.right(C_TE_MAXSIZE));
        MainTE->verticalScrollBar()->setValue(MainTE->verticalScrollBar()->maximum()); // перемещение "ползунка" принудительно вниз
    }
}

QString Coma::ByteToHex(quint8 hb)
{
    QString tmpString;
    quint8 halfbyte = hb & 0xF0;
    halfbyte >>= 4;
    tmpString.append(QString::number(halfbyte, 16));
    halfbyte = hb & 0x0F;
    tmpString.append(QString::number(halfbyte, 16));
    return tmpString;
}

// ############################## всплывающие окошки ###############################

void Coma::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (!SWHide)
    {
        QWidget *sww = this->findChild<QWidget *>("slidew");
        if (sww == 0)
            return;
        sww->setGeometry(QRect(width()-sww->width(), 0, sww->width(), height()));
    }
}

void Coma::MouseMove()
{
    QPoint curPos = mapFromGlobal(QCursor::pos());
    QWidget *sww = this->findChild<QWidget *>("slidew");
    if (sww == 0)
        return;
    if ((abs(curPos.x() - width()) < 10) && (curPos.y() > 0) && (curPos.y() < height()))
    {
        if (SWHide)
            ShowOrHideSlideSW();
    }
    else if ((abs(curPos.x() - width()) > sww->width()) && (curPos.y() > 0) && (curPos.y() < height()))
    {
        if (!SWHide)
            ShowOrHideSlideSW();
    }
}

void Coma::ShowOrHideSlideSW()
{
    QWidget *w = this->findChild<QWidget *>("slidew");
    if (w == 0)
        return;
    if (w->isHidden())
        w->show();
    if (SWHide)
        w->setGeometry(SWGeometry);
    QPropertyAnimation *ani = new QPropertyAnimation(w, "geometry");
    ani->setDuration(500);
    QRect startRect(width(), 30, 0, height()-30);
    QRect endRect(width() - w->width(), 30, w->width(), height()-30);
    if (SWHide)
    {
        ani->setStartValue(startRect);
        ani->setEndValue(endRect);
    }
    else
    {
        ani->setStartValue(endRect);
        ani->setEndValue(startRect);
    }
    ani->start();
    SWHide = !SWHide;
}

void Coma::ShowErrorDialog()
{
    ErrorDialog *dlg = new ErrorDialog;
    for (int i=0; i<pc.ermsgpool.size(); ++i)
        dlg->AddErrMsg(pc.ermsgpool.at(i));
    dlg->exec();
}

void Coma::SetProgressBar(quint32 cursize)
{
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if (prb != 0)
        prb->setValue(cursize);
    WDFunc::SetLBLText(this, "prblbl",pc.PrbMessage + QString::number(cursize) + " из " + QString::number(PrbSize));
    if (cursize >= PrbSize)
        DisableProgressBar();
}

void Coma::SetProgressBarSize(quint32 size)
{
    PrbSize = size;
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if (prb == 0)
    {
        DBGMSG;
        return;
    }
    WDFunc::SetLBLText(this, "prblbl",pc.PrbMessage + QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(PrbSize);
    prb->setEnabled(true);
}

void Coma::DisableProgressBar()
{
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if (prb == 0)
    {
        DBGMSG;
        return;
    }
    prb->setEnabled(false);
    WDFunc::SetLBLText(this, "prblbl","",false);
}

void Coma::SetDefConf()
{
    MainConfDialog->SetMainDefConf();
    ConfB->SetDefConf();
    ConfM->SetDefConf();
    Fill();
    MessageBox2::information(this, "Успешно", "Задана конфигурация по умолчанию");
}

void Coma::Fill()
{
    MainConfDialog->Fill();
    ConfB->Fill();
    ConfM->Fill();
}
