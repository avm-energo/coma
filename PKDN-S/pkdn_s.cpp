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
#include "pkdn_s.h"
#include "../commands.h"
#include "../check/checkdialoga1.h"
#include "../config/confdialoga1.h"
#include "../dialogs/fwupdialog.h"
#include "../dialogs/downloaddialog.h"
#include "../dialogs/oscdialog.h"
#include "../dialogs/hiddendialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/errordialog.h"
#include "../tune/tunedialoga1.h"
#include "../widgets/mytabwidget.h"
#include "../widgets/waitwidget.h"
#include "../widgets/messagebox.h"
#include "../widgets/s_tqspinbox.h"
#include "../widgets/wd_func.h"
#include "../log.h"

pkdn_s::pkdn_s(QWidget *parent)
    : QMainWindow(parent)
{
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(pc.HomeDir);
    if (!dir.exists())
        dir.mkpath(".");
    cn = new Canal;
    S2Config.clear();
    SetupUI();
    SetupMenubar();
    PrepareTimers();
    LoadSettings();
    connect(cn,SIGNAL(oscerasesize(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    connect(cn,SIGNAL(osceraseremaining(quint32)),this,SLOT(SetProgressBar(quint32)));
    connect(cn,SIGNAL(incomingdatalength(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    connect(cn,SIGNAL(bytesreceived(quint32)),this,SLOT(SetProgressBar(quint32)));
    connect(cn,SIGNAL(sendend()),this,SLOT(DisableProgressBar()));
    connect(cn,SIGNAL(readbytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(cn,SIGNAL(writebytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(this,SIGNAL(Retry()),this,SLOT(Stage1()));
}

pkdn_s::~pkdn_s()
{
    SaveSettings();
}

void pkdn_s::SetupUI()
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
    act->setIcon(QIcon(":/pic/a1.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(EmulA1()));
    tb->addAction(act);
    tb->addSeparator();
    // пока непонятно, что настраивать
/*    act = new QAction(this);
    act->setToolTip("Настройки");
    act->setIcon(QIcon(":/pic/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    tb->addAction(act); */
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

    slyout->addWidget(WDFunc::NewLBL(this, "ПКДН"));
    WDFunc::AddLabelAndLineedit(slyout, "Серийный номер устройства:", "snble");
    WDFunc::AddLabelAndLineedit(slyout, "Аппаратная версия платы:", "hwverble");
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

void pkdn_s::SetupMenubar()
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

/*    act = new QAction(this);
    act->setText("Запуск в режиме эмуляции");
    act->setIcon(QIcon(":/pic/a1.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(EmulA1()));
    MainMenuBar->addAction(act); */

    QMenu *menu = new QMenu;
    menu->setTitle("Секретные операции");
    WriteSNAction = new QAction(this);
    WriteSNAction->setText("Работа с Hidden Block");
    connect(WriteSNAction,SIGNAL(triggered()),this,SLOT(OpenBhbDialog()));
    menu->addAction(WriteSNAction);
    MainMenuBar->addMenu(menu);

    // пока непонятно, что настраивать
/*    menu = new QMenu;
    menu->setTitle("Настройки");
    act = new QAction(this);
    act->setText("Настройки");
    act->setIcon(QIcon(":/pic/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    menu->addAction(act);
    MainMenuBar->addMenu(menu); */

    act = new QAction(this);
    act->setText("О программе");
    connect(act,SIGNAL(triggered()),this,SLOT(GetAbout()));
    MainMenuBar->addAction(act);
    setMenuBar(MainMenuBar);
}

void pkdn_s::LoadSettings()
{
    QSettings *sets = new QSettings ("EvelSoft","pkdn_s");
    pc.Port = sets->value("Port", "COM1").toString();
}

void pkdn_s::SaveSettings()
{
    QSettings *sets = new QSettings ("EvelSoft","pkdn_s");
    sets->setValue("Port", pc.Port);
}

void pkdn_s::PrepareTimers()
{
    QTimer *MouseTimer = new QTimer;
    connect(MouseTimer,SIGNAL(timeout()),this,SLOT(MouseMove()));
    MouseTimer->start(50);
}

void pkdn_s::Stage1()
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
    connect(nextL, SIGNAL(clicked(bool)),dlg,SLOT(close()));
    lyout->addWidget(nextL);
    dlg->setLayout(lyout);
    dlg->exec();
}

void pkdn_s::Stage1_5()
{
    if (!cn->Connected)
    {
        pc.PrbMessage = "Загрузка данных...";

        QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
        if (info.size() == 0)
        {
            MessageBox2::error(this, "Ошибка", "В системе нет последовательных портов");
            pc.ErMsg(USB_NOCOMER);
            emit Retry();
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
            MessageBox2::error(this, "Ошибка", "Порт не найден");
            pc.ErMsg(USB_COMER);
            emit Retry();
            return;
        }
        cn->baud = 115200;
        if (!cn->Connect())
        {
            MessageBox2::error(this, "Ошибка", "Связь по данному порту не может быть установлена");
            emit Retry();
            return;
        }
    }
    Stage2();
}

void pkdn_s::SetPort(QString str)
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

void pkdn_s::Stage2()
{
    if (CN_GetBsi(&pc.ModuleBsi, sizeof(publicclass::Bsi)) != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Блок Bsi не может быть прочитан, связь потеряна");
        cn->Disconnect();
        emit Retry();
        return;
    }
    pc.MType = ((pc.ModuleBsi.MTypeB & 0x000000FF) << 8) | (pc.ModuleBsi.MTypeM & 0x000000FF);
    pc.ModuleTypeString = "ПКДН-";
    pc.ModuleTypeString.append(QString::number(pc.MType, 16));
    if ((pc.ModuleBsi.SerialNumB == 0xFFFFFFFF) || ((pc.ModuleBsi.SerialNumM == 0xFFFFFFFF) && (pc.ModuleBsi.MTypeM != MTM_00)) || \
            (pc.ModuleBsi.SerialNum == 0xFFFFFFFF)) // серийный номер не задан, выдадим предупреждение
        OpenBhbDialog();
    FillBsi();
    Stage3();
}

void pkdn_s::FillBsi()
{
    WDFunc::SetLEData(this, "hwverble", pc.VerToStr(pc.ModuleBsi.HwverB));
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
    WDFunc::SetLEData(this, "snble", QString::number(pc.ModuleBsi.SerialNum, 16));
}

void pkdn_s::ClearBsi()
{
    WDFunc::SetLEData(this, "hwverble", "");
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
    WDFunc::SetLEData(this, "snble", "");
}

void pkdn_s::OpenBhbDialog()
{
    if (!cn->Connected)
    {
        MessageBox2::information(this, "Подтверждение", "Для работы данной функции необходимо сначала установить связь с модулем");
        return;
    }
    HiddenDialog *dlg = new HiddenDialog(HiddenDialog::PKDN);
    pc.BoardBBhb.HWVer = pc.ModuleBsi.HwverB;
    pc.BoardBBhb.ModSerialNum = pc.ModuleBsi.SerialNum;
    pc.BoardBBhb.SerialNum = pc.ModuleBsi.SerialNumB;
    pc.BoardBBhb.MType = pc.ModuleBsi.MTypeB;
    dlg->Fill(); // заполняем диалог из недавно присвоенных значений
    dlg->exec();
    if (CN_GetBsi(&pc.ModuleBsi, sizeof(publicclass::Bsi)) != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Блок Bsi не может быть прочитан, связь потеряна");
        cn->Disconnect();
        emit Retry();
    }
}

void pkdn_s::Stage3()
{
    ClearTW();
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
/*    DownDialog = new downloaddialog;
    FwUpDialog = new fwupdialog;
    OscDialog = new oscdialog; */
    ConfDialogA1 *DialogA1 = new ConfDialogA1(S2Config);
    ConfB = DialogA1;
    MainTW->addTab(ConfB, "Конфигурирование");
    connect(ConfB,SIGNAL(NewConfLoaded()),this,SLOT(Fill()));
    connect(ConfB,SIGNAL(LoadDefConf()),this,SLOT(SetDefConf()));
    CheckD = new CheckDialogA1;
    TuneD = new TuneDialogA1;
    oscdialog *OscD = new oscdialog;
    downloaddialog *DownD = new downloaddialog;
    fwupdialog *FwUpD = new fwupdialog;
    MainTW->addTab(TuneD, "Регулировка");
    MainTW->addTab(CheckD, "Проверка");
    MainTW->addTab(OscD, "Осциллограммы");
    MainTW->addTab(DownD, "События");
    MainTW->addTab(FwUpD, "Загрузка ВПО");
    if (pc.ModuleBsi.Hth & HTH_CONFIG) // нет конфигурации
        pc.ErMsg(ER_NOCONF);
    if (pc.ModuleBsi.Hth & HTH_REGPARS) // нет коэффициентов
        pc.ErMsg(ER_NOTUNECOEF);
    MainTW->repaint();
    MainTW->show();
}

void pkdn_s::ClearTW()
{
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
}

void pkdn_s::Disconnect()
{
    if (!pc.Emul)
        cn->Disconnect();
    ClearBsi();
    ClearTW();
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    MainTW->hide();
    pc.Emul = false;
}

void pkdn_s::GetAbout()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *l2yout = new QVBoxLayout;
    QLabel *lbl = new QLabel("ПКДН-Сервис");
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

void pkdn_s::Exit()
{
    this->close();
}

void pkdn_s::EmulA1()
{
//    if ((pc.Emul) || (cn->Connected)) // если уже в режиме эмуляции или работаем с модулем, выход
    if (pc.Emul)
        return;
    pc.ModuleBsi.MTypeB = MTB_A1;
    pc.ModuleBsi.MTypeM = MTM_00;
    StartEmulxx();
}

void pkdn_s::StartEmulxx()
{
    if (cn->Connected)
        Disconnect();
    pc.ModuleBsi.SerialNum = 0x12345678;
    pc.ModuleBsi.Hth = 0x00;
    pc.Emul = true;
    Stage3();
}

void pkdn_s::StartSettingsDialog()
{
    // пока непонятно, что тут настраивать
/*    SettingsDialog *dlg = new SettingsDialog;
    dlg->exec(); */
}

void pkdn_s::UpdateMainTE(QByteArray ba)
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

QString pkdn_s::ByteToHex(quint8 hb)
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

void pkdn_s::resizeEvent(QResizeEvent *e)
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

void pkdn_s::MouseMove()
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

void pkdn_s::ShowOrHideSlideSW()
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

void pkdn_s::ShowErrorDialog()
{
    ErrorDialog *dlg = new ErrorDialog;
    dlg->exec();
}

void pkdn_s::SetProgressBar(quint32 cursize)
{
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if (prb != 0)
        prb->setValue(cursize);
    WDFunc::SetLBLText(this, "prblbl",pc.PrbMessage + QString::number(cursize) + " из " + QString::number(PrbSize));
    if (cursize >= PrbSize)
        DisableProgressBar();
}

void pkdn_s::SetProgressBarSize(quint32 size)
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

void pkdn_s::DisableProgressBar()
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

void pkdn_s::SetDefConf()
{
    ConfB->SetDefConf();
    Fill();
    MessageBox2::information(this, "Успешно", "Задана конфигурация по умолчанию");
}

void pkdn_s::Fill()
{
    ConfB->Fill();
}
