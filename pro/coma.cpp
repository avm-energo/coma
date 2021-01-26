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

#include "coma.h"

#include "../dialogs/connectdialog.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/keypressdialog.h"
#include "../dialogs/settingsdialog.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/errorqueue.h"
#include "../gen/logger.h"
#include "../gen/stdfunc.h"

#include <memory>
#ifndef AVM_DEBUG
#include "../interfaces/iec104.h"
#include "../interfaces/modbus.h"
#endif
#include "../interfaces/protocom.h"
#include "../interfaces/settingstypes.h"
#include "../widgets/aboutwidget.h"
#include "../widgets/etabwidget.h"
#include "../widgets/splashscreen.h"
#include "../widgets/styleloader.h"
#include "../widgets/wd_func.h"

#include <QApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QProgressBar>
#include <QToolBar>
#include <QtGlobal>
#include <functional>
#include <memory>

#ifdef _WIN32
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
void registerForDeviceNotification(QWidget *ptr)
{
    DEV_BROADCAST_DEVICEINTERFACE devInt;
    ZeroMemory(&devInt, sizeof(devInt));
    // GUID _guid1 = { 0x25dbce51, 0x6c8f, 0x4a72, { 0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 } };

    GUID _guid = { 0xa5dcbf10, 0x6530, 0x11d2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } };
    devInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);

    devInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    // With DEVICE_NOTIFY_ALL_INTERFACE_CLASSES this property ignores
    devInt.dbcc_classguid = _guid;

    HDEVNOTIFY blub;
    // NOTE Проверить со всеми модулями
    blub = RegisterDeviceNotification((HDEVNOTIFY)ptr->winId(), &devInt,
        DEVICE_NOTIFY_ALL_INTERFACE_CLASSES /*DBT_DEVTYP_OEM*/ /*DEVICE_NOTIFY_WINDOW_HANDLE*/);
}
#endif

Coma::Coma(QWidget *parent) : QMainWindow(parent)
{
    // Load settings before anything

    SplashScreen *splash = new SplashScreen(QPixmap("images/surgery.png"));
    splash->show();
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(StdFunc::GetHomeDir());
    if (!dir.exists())
        dir.mkpath(".");
    StdFunc::Init();
    qInfo("=== Log started ===\n");

#ifdef __linux__
    // linux code goes here
#elif _WIN32
    // Listen to device events
    registerForDeviceNotification(this);
#else

#endif

    Reconnect = false;

    newTimers();

    splash->finish(this);
    LoadSettings();
    splash->deleteLater();
    setStatusBar(WDFunc::NewSB(this));
}

Coma::~Coma()
{
}
void convertPixmap(size_t size, QAction *jourAct)
{
    const QIcon jourIcon(":/icons/tnfrosya.svg");
    QPixmap pix = jourIcon.pixmap(QSize(40, 40), QIcon::Disabled);
    QPainter painter(&pix);
    painter.drawPixmap(QRect(20, 0, 20, 20), WDFunc::NewCircle(Qt::red, 20));
    QFont font(painter.font());
    font.setPixelSize(14);
    painter.setFont(font);
    painter.setPen(Qt::white);
    if (size > 10)
        painter.drawText(QRect(20, 0, 20, 20), Qt::AlignCenter, "...");
    else
        painter.drawText(QRect(20, 0, 20, 20), Qt::AlignCenter, QString::number(size));
    jourAct->setIcon(pix);
}

QToolBar *Coma::createToolBar()
{
    QToolBar *tb = new QToolBar(this);
    tb->setContextMenuPolicy(Qt::PreventContextMenu);
    tb->setIconSize(QSize(40, 40));
    tb->addAction(QIcon(":/icons/tnstart.svg"), "Соединение", this, &Coma::prepareConnectDlg);
    tb->addAction(QIcon(":/icons/tnstop.svg"), "Разрыв соединения", this, &Coma::DisconnectAndClear);
    tb->addSeparator();
    tb->addAction(QIcon(":/icons/tnsettings.svg"), "Настройки", [this]() {
        SettingsDialog *dlg = new SettingsDialog;
        connect(dlg, &SettingsDialog::disableAlarmUpdate, AlarmW, &AlarmWidget::disableAlarm);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
        this->SaveSettings();
    });
    const QIcon jourIcon(":/icons/tnfrosya.svg");

    QAction *jourAct = new QAction(jourIcon, tr("&Журнал..."), this);
    jourAct->setShortcuts(QKeySequence::Open);
    jourAct->setStatusTip(tr("Открыть протокол работы"));

    connect(jourAct, &QAction::triggered, this, []() {
        ErrorDialog *dlg = new ErrorDialog;
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
    });
    connect(jourAct, &QAction::triggered, [jourAct, jourIcon] { jourAct->setIcon(jourIcon); });
    const auto &queue = ErrorQueue::GetInstance();
    connect(&queue, &ErrorQueue::errCounts, this, std::bind(&convertPixmap, std::placeholders::_1, jourAct),
        Qt::QueuedConnection);
    tb->addAction(jourAct);
    return tb;
}

void Coma::SetupUI()
{
    QString caption(PROGNAME);
    caption.append(" v. ").append(COMAVERSION);
    setWindowTitle(caption);
    setMinimumSize(QSize(1050, 700));
    QWidget *wdgt = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout(wdgt);
    QHBoxLayout *hlyout = new QHBoxLayout;

    hlyout->addWidget(createToolBar());

    AlarmW = new AlarmWidget(this);

    // AlarmW->setMaximumWidth(780);
    hlyout->addWidget(AlarmW, Qt::AlignCenter);

    lyout->addLayout(hlyout);
    lyout->addWidget(Least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    SetupMenubar();
}

QWidget *Coma::Least()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addStretch(500);

    QHBoxLayout *inlyout = new QHBoxLayout;
    lyout->addLayout(inlyout);

    MainTW = new QStackedWidget(this);
    MainTW->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    MainLW = new QListWidget(this);

    inlyout->addWidget(MainLW);
    inlyout->addWidget(MainTW);

    MainTW->hide();
    MainLW->hide();
    connect(MainTW, &QStackedWidget::currentChanged, this, &Coma::MainTWTabChanged);

    QFrame *line = new QFrame;
    lyout->addWidget(line);

    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBL2(this, "Обмен"));
    inlyout->addWidget(WDFunc::NewLBL2(this, "", "prb1lbl"));

    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prb1prb");
    prb->setOrientation(Qt::Horizontal);
    // prb->setMinimumWidth(50);
    prb->setMaximumHeight(height() / 50);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);

    //    inlyout = new QHBoxLayout;
    //    inlyout->addWidget(WDFunc::NewLBLT2(this, "Отсчёт"));
    //    inlyout->addWidget(WDFunc::NewLBLT2(this, "", "prb2lbl"));

    //    prb = new QProgressBar;
    //    prb->setObjectName("prb2prb");
    //    prb->setOrientation(Qt::Horizontal);
    //    // prb->setMinimumWidth(50);
    //    prb->setMaximumHeight(height() / 50);
    //    inlyout->addWidget(prb);
    //    lyout->addLayout(inlyout);
    w->setLayout(lyout);
    return w;
}
void Coma::SetupMenubar()
{
    QMenuBar *menubar = new QMenuBar(this);

    QMenu *menu = new QMenu(this);
    menu->setTitle("Главное");

    menu->addAction("Выход", this, &Coma::close);
    menu->addAction(QIcon(":/icons/tnstart.svg"), "Соединение", this, &Coma::prepareConnectDlg);
    menu->addAction(QIcon(":/icons/tnstop.svg"), "Разрыв соединения", this, &Coma::DisconnectAndClear);

    menubar->addMenu(menu);

    menubar->addAction("О программе", this, &Coma::GetAbout);

    menubar->addSeparator();

    setMenuBar(menubar);
}

// void Coma::addConfTab(ETabWidget *MainTW, QString str)
//{
//    MainTW->addTab(corDialog, str);
//    corDialog->corDIndex = MainTW->indexOf(corDialog);
//    if (Board::GetInstance().interfaceType() == Board::InterfaceType::RS485)
//        ChModbus->CorIndex = corDialog->corDIndex;
//}

/*void Coma::setupDialogs(ETabWidget *MainTW)
{
    auto const &board = Board::GetInstance();
    QString str;
    QList<QDialog *> dialogs = MainTW->findChildren<QDialog *>();
    for (const auto &dialog : dialogs)
        qDebug() << dialog;
    dialogs = findChildren<QDialog *>();
    for (const auto &dialog : dialogs)
        qDebug() << dialog;

    auto children = this->children();
    for (const auto &item : children)
    {
        qDebug() << item;
    }
    str = (!children.contains(checkMDialog)) ? "Текущие параметры" : "Текущие параметры\nБазовая";
    // if (checkBDialog != nullptr)
    if (children.contains(checkBDialog))
    {
        checkBDialog->setMinimumHeight(500);
        MainTW->addTab(checkBDialog, str);
        CheckIndex = MainTW->indexOf(checkBDialog);
        if (board.interfaceType() == Board::InterfaceType::RS485)
            ChModbus->CheckIndex = CheckIndex;
    }

    str = (!children.contains(checkBDialog)) ? "Текущие параметры" : "Текущие параметры\nМезонин";
    // if (checkMDialog != nullptr)
    if (children.contains(checkMDialog))
        MainTW->addTab(checkMDialog, str);

    // if (HarmDialog != nullptr)
    if (children.contains(HarmDialog))
    {
        MainTW->addTab(HarmDialog, "Гармоники");
        CheckHarmIndex = MainTW->indexOf(HarmDialog);
        if (board.interfaceType() == Board::InterfaceType::RS485)
            ChModbus->CheckHarmIndex = CheckHarmIndex;
    }

    // if (VibrDialog != nullptr)
    if (children.contains(VibrDialog))
    {
        MainTW->addTab(VibrDialog, "Вибрации");
        CheckVibrIndex = MainTW->indexOf(VibrDialog);
        if (board.interfaceType() == Board::InterfaceType::RS485)
            ChModbus->CheckHarmIndex = CheckVibrIndex;
    }

    // if (confBDialog != nullptr)
    if (children.contains(confBDialog))
    {
        str = (!children.contains(confMDialog)) ? "Конфигурирование" : "Конфигурирование\nБазовая";
        MainTW->addTab(confBDialog, str);
        if (confBDialog->IsNeededDefConf)
        {
            confBDialog->SetDefConf();
            QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию", QMessageBox::Ok);
            // QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");
        }
        connect(confBDialog, &AbstractConfDialog::NewConfToBeLoaded, this, &Coma::Fill);
        connect(confBDialog, &AbstractConfDialog::DefConfToBeLoaded, this, &Coma::SetDefConf);
    }

    // if (confMDialog != nullptr)
    if (children.contains(confMDialog))
    {
        str = (!children.contains(confBDialog)) ? "Конфигурирование" : "Конфигурирование\nМезонин";
        MainTW->addTab(confMDialog, str);

        ConfIndex = MainTW->indexOf(confMDialog);

        if (confMDialog->IsNeededDefConf)
        {
            confMDialog->SetDefConf();
            QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию", QMessageBox::Ok);
        }
        connect(confMDialog, &AbstractConfDialog::NewConfToBeLoaded, this, &Coma::Fill);
        connect(confMDialog, &AbstractConfDialog::DefConfToBeLoaded, this, &Coma::SetDefConf);
    }
    if (children.contains(timeDialog))
    {
        MainTW->addTab(timeDialog, "Время");
        TimeIndex = MainTW->indexOf(timeDialog);
        if (board.interfaceType() == Board::InterfaceType::RS485)
            ChModbus->TimeIndex = TimeIndex;
        connect(TimeTimer, &QTimer::timeout, timeDialog, &MNKTime::slot2_timeOut);
    }

    if (children.contains(corDialog))
    {
        switch (board.typeB())
        {
        case Config::MTB_A2:
            switch (board.typeM())
            {
            case Config::MTM_84:
                addConfTab(MainTW, "Начальные значения");
                break;

            case Config::MTM_87:
                addConfTab(MainTW, "Старение изоляции");
                break;
            }
            break;
        case Config::MTB_A3:
            switch (board.typeM())
            {
            case Config::MTM_87:
                addConfTab(MainTW, "Наработка");
                break;
            }

            break;
        }
    }
} */

void Coma::prepareConnectDlg()
{
    auto const &board = Board::GetInstance();
    if (board.connectionState() != Board::ConnectionState::Closed)
        return;
    if (!Reconnect)
    {
        QEventLoop loop;
        ConnectDialog *dlg = new ConnectDialog;
        connect(dlg, &ConnectDialog::Accepted, this, [=](const ConnectStruct st) {
            dlg->close();
            startWork(st);
        });
        dlg->show();
    }

    // Stage3

    DisconnectAndClear();
}

void Coma::startWork(const ConnectStruct st)
{
    auto const &board = Board::GetInstance();
    ConnectSettings = st;
    SaveSettings();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    Connect();
}

/*void Coma::setupQConnections()
{
    connect(AlarmW, &AlarmWidget::AlarmButtonPressed, AlarmStateAllDialog, &QDialog::show);
    connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, m_Module->getWarn(), &QDialog::show);
    connect(AlarmW, &AlarmWidget::ModuleAlarmButtonPressed, m_Module->getAlarm(), &QDialog::show);
    setupConnections();

        switch (Board::GetInstance().typeB())
        {
        case Config::MTB_A2:
        {
            switch (Board::GetInstance().typeM())
            {
            case Config::MTM_84:
            {
                checkBDialog = new CheckDialogKIV(BoardTypes::BT_BASE, this);

                // S2Config->clear();
                if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
                    confMDialog = new ConfDialogKIV(S2Config, this);

                corDialog = new CorDialog(this);

                WarnKIVDialog = new WarnAlarmKIV(Alarm, AlarmW);
                connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, WarnKIVDialog, &QDialog::show);
                connect(Alarm, &AlarmClass::SetWarnAlarmColor, WarnKIVDialog, &AbstractWarnAlarm::Update);

                AlarmKIVDialog = new AvarAlarmKIV(Alarm, AlarmW);
                connect(AlarmW, &AlarmWidget::ModuleAlarmButtonPressed, AlarmKIVDialog, &QDialog::show);
                connect(Alarm, &AlarmClass::SetAlarmColor, AlarmKIVDialog, &AbstractAvarAlarm::Update);

                connect(Alarm, SIGNAL(SetWarnAlarmColor(QList<bool>)), checkBDialog,
       SLOT(SetWarnAlarmColor(QList<bool>))); connect(Alarm, SIGNAL(SetAlarmColor(QList<bool>)), checkBDialog,
       SLOT(SetAlarmColor(QList<bool>)));

                break;
            }
            case Config::MTM_87:
            {
                HarmDialog = new CheckDialogHarmonicKTF(BoardTypes::BT_BASE);
                connect(HarmTimer, &QTimer::timeout, HarmDialog, &EAbstractCheckDialog::USBUpdate);

                // S2Config->clear();
                if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
                    confMDialog = new ConfDialogKTF(S2Config, this);

                checkBDialog = new CheckDialogKTF(BoardTypes::BT_BASE, this);

                corDialog = new CorDialogKTF(this);

                WarnKTFDialog = new WarnAlarmKTF(Alarm, AlarmW);
                connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, WarnKTFDialog, &QDialog::show);
                connect(Alarm, &AlarmClass::SetWarnAlarmColor, WarnKTFDialog, &AbstractWarnAlarm::Update);

                AlarmKTFDialog = new AvarAlarmKTF(Alarm, AlarmW);
                connect(AlarmW, &AlarmWidget::ModuleAlarmButtonPressed, AlarmKTFDialog, &QDialog::show);
                connect(Alarm, &AlarmClass::SetAlarmColor, AlarmKTFDialog, &AbstractAvarAlarm::Update);

                break;
            }
            }
            break;
        }
        case Config::MTB_A3:
            switch (Board::GetInstance().typeM())
            {
            case Config::MTM_87:
                checkBDialog = new CheckDialogKDV(BoardTypes::BT_BASE, this);

                HarmDialog = new CheckDialogHarmonicKDV(BoardTypes::BT_BASE, this);
                connect(HarmTimer, &QTimer::timeout, HarmDialog, &EAbstractCheckDialog::USBUpdate);

                VibrDialog = new CheckDialogVibrKDV(BoardTypes::BT_BASE, this);
                connect(VibrTimer, &QTimer::timeout, VibrDialog, &EAbstractCheckDialog::USBUpdate);

                // S2Config->clear();
                if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
                    confMDialog = new ConfDialogKDV(S2Config, this);

                corDialog = new CorDialogKDV(this);

                break;
            }

            break;
        }

        connect(this, &Coma::ClearBsi, infoDialog, &InfoDialog::ClearBsi);
        connect(AlarmW, SIGNAL(SetWarnAlarmColor(QList<bool>)), checkBDialog, SLOT(SetWarnAlarmColor(QList<bool>)));
        connect(AlarmW, SIGNAL(SetAlarmColor(QList<bool>)), checkBDialog, SLOT(SetAlarmColor(QList<bool>)));

        switch (Board::GetInstance().interfaceType())
        {
        case Board::InterfaceType::USB:
        {
            NewTimersBda();
            break;
        }
        case Board::InterfaceType::Ethernet:
        {
            connect(Ch104, SIGNAL(Floatsignalsready(IEC104Thread::FlSignals104 *)), checkBDialog,
                SLOT(UpdateFlData(IEC104Thread::FlSignals104 *)));
            connect(Ch104, SIGNAL(Sponsignalsready(IEC104Thread::SponSignals *)), checkBDialog,
                SLOT(UpdateSponData(IEC104Thread::SponSignals *)));

            connect(timeDialog, &MNKTime::ethTimeRequest, Ch104, &IEC104::InterrogateTimeGr15);
            connect(Ch104, &IEC104::Bs104signalsready, timeDialog, &MNKTime::FillTimeFrom104);
            connect(timeDialog, &MNKTime::ethWriteTimeToModule, Ch104, &IEC104::com51WriteTime);

            connect(corDialog, &AbstractCorDialog::SendCom45, Ch104, &IEC104::Com45);
            connect(corDialog, &AbstractCorDialog::SendCom50, Ch104, &IEC104::Com50);
            connect(corDialog, &AbstractCorDialog::CorReadRequest, Ch104, &IEC104::CorReadRequest);
            connect(Ch104, &IEC104::SendMessageOk, corDialog, &AbstractCorDialog::MessageOk);
            connect(Ch104, &IEC104::Floatsignalsready, corDialog, &AbstractCorDialog::UpdateFlCorData);

            connect(confMDialog, &AbstractConfDialog::ReadConfig, Ch104, &IEC104::SelectFile);
            connect(Ch104, &IEC104::SendS2fromiec104, confMDialog, &AbstractConfDialog::FillConf);
            connect(confMDialog, &AbstractConfDialog::writeConfFile, Ch104, &IEC104::FileReady);
            connect(Ch104, &IEC104::SendConfMessageOk, confMDialog, &AbstractConfDialog::WriteConfMessageOk);
            break;
        }
        case Board::InterfaceType::RS485:
        {
            connect(ChModbus, SIGNAL(ModbusState(ConnectionStates)), checkBDialog,
                SLOT(onModbusStateChanged(ConnectionStates)));
            connect(ChModbus, SIGNAL(SignalsReceived(QList<ModBus::SignalStruct>)), checkBDialog,
                SLOT(UpdateModBusData(QList<ModBus::SignalStruct>)));

            connect(timeDialog, &MNKTime::modBusTimeRequest, ChModbus, &ModBus::ReadTime);
            connect(ChModbus, &ModBus::TimeSignalsReceived, timeDialog, &MNKTime::FillTimeFromModBus);
            connect(timeDialog, &MNKTime::modbusWriteTimeToModule, ChModbus, &ModBus::WriteTime);
            connect(ChModbus, &ModBus::TimeReadError, timeDialog, &MNKTime::ErrorRead);
            connect(ChModbus, &ModBus::TimeWritten, timeDialog, &MNKTime::TimeWritten);

            connect(ChModbus, &ModBus::ErrorRead, corDialog, &AbstractCorDialog::ErrorRead);
            connect(ChModbus, &ModBus::CorSignalsReceived, corDialog, &AbstractCorDialog::ModBusUpdateCorData);
            connect(ChModbus, &ModBus::CorSignalsWritten, corDialog, &AbstractCorDialog::MessageOk);
            connect(corDialog, &AbstractCorDialog::RS485WriteCorBd, ChModbus,
                &ModBus::ModWriteCor); //, int*)));
            connect(corDialog, &AbstractCorDialog::RS485ReadCorBd, ChModbus, &ModBus::ModReadCor);
            break;
        }
        default:
            break;
        }
} */

void Coma::PrepareDialogs()
{
    m_Module = ModulePointer(Module::createModule(BdaTimer, AlarmW));
}

void Coma::CloseDialogs()
{
    //    Q_ASSERT(m_Module != nullptr);
    //    m_Module->closeDialogs();
    //    if (AlarmStateAllDialog != nullptr)
    //    {
    //        AlrmTimer->stop();
    //        AlarmStateAllDialog->close();
    //        AlarmStateAllDialog = nullptr;
    //    }

    //    AlrmTimer->stop();

    //    QList<QDialog *> widgets = findChildren<QDialog *>();

    //    for (auto &i : widgets)
    //    {
    //        qDebug() << i;
    //        i->close();
    //        i->deleteLater();
    //    }
}

void Coma::New104()
{
    // Ch104 = new IEC104;
    // connect(this, &Coma::StopCommunications, Ch104, &IEC104::stop);
    //    connect(Ch104, &IEC104::Finished, [this]() { ActiveThreads &= ~THREAD::P104; });
    // connect(Ch104, &IEC104::Finished, [this]() { ActiveThreads = false; });
    // connect(Ch104,SIGNAL(Sponsignalsready(IEC104Thread::SponSignals*)),this,SLOT(UpdatePredAlarmEvents(IEC104Thread::SponSignals*)));
    //    connect(Ch104, &IEC104::SetDataSize, this, &Coma::SetProgressBar1Size);
    //    connect(Ch104, &IEC104::SetDataCount, this, &Coma::SetProgressBar1);
    // connect(Ch104, &BaseInterface::reconnect, this, &Coma::ReConnect);
    //    connect(Ch104, &IEC104::Sponsignalsready, Alarm, &AlarmClass::UpdateAlarm104);
    //    connect(Ch104, &IEC104::Bs104signalsready, this, qOverload<IEC104Thread::BS104Signals *>(&Coma::FillBSI));
}

void Coma::NewModbus()
{
    // ChModbus = new ModBus;
    // connect(this, &Coma::StopCommunications, ChModbus, &ModBus::stop);
    //    connect(ChModbus, &ModBus::Finished, [this]() { ActiveThreads &= ~THREAD::MODBUS; });
    // connect(ChModbus, &ModBus::Finished, [this]() { ActiveThreads = false; });
    //  connect(ChModbus,SIGNAL(CoilSignalsReady(ModBus::Coils)), this,
    //  SLOT(ModBusUpdatePredAlarmEvents(ModBus::Coils)));
    // connect(ChModbus, &BaseInterface::reconnect, this, &Coma::ReConnect);
    //    connect(ChModbus, &ModBus::CoilSignalsReady, Alarm, &AlarmClass::UpdateAlarmModBus);

    //    connect(ChModbus, &ModBus::BsiFromModbus, this,
    //        qOverload<QList<ModBus::BSISignalStruct>, unsigned int>(&Coma::FillBSI));
}

void Coma::newTimers()
{
    BdaTimer = new QTimer(this);
    BdaTimer->setInterval(1000);

    AlrmTimer = new QTimer(this);
    AlrmTimer->setInterval(5000);
    AlrmTimer->start();

    //    ReconnectTimer = new QTimer(this);
    //    ReconnectTimer->setInterval(INTERVAL::RECONNECT);
    //    ReconnectTimer->setSingleShot(true);
    //    connect(ReconnectTimer, &QTimer::timeout, this, &Coma::AttemptToRec);
}

void Coma::setupConnections()
{

    connect(&DataManager::GetInstance(), &DataManager::responseReceived, this, &Coma::update);
    //    for (auto *d : m_Module->dialogs())
    //    {
    //        connect(d, &UWidget::setGeneralProgressBarCount, this, &Coma::setGeneralProgressBarCount);
    //        connect(d, &UWidget::setGeneralProgressBarSize, this, &Coma::setGeneralProgressBarSize);
    //    }
}

void Coma::prepare()
{
    //    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    //    Q_ASSERT(MainTW != nullptr);
    //    if (MainTW == nullptr)
    //    {
    //        qCritical("No MainTW in widgets list");
    //        return;
    //    }
    auto const &board = Board::GetInstance();
    QMessageBox::information(
        this, "Связь установлена", "Установлена связь с " + Board::GetInstance().moduleName(), QMessageBox::Ok);
    Reconnect = true;

    PrepareDialogs();
    setupConnections();
    // нет конфигурации
    if (board.noConfig())
        qCritical() << Error::Msg::NoConfError;
    // нет коэффициентов
    if (board.noRegPars())
        qCritical() << Error::Msg::NoTuneError;

    QList<UDialog *> dlgs = m_Module->dialogs();
    Q_ASSERT(MainTW->count() == 0);
    for (auto *d : dlgs)
    {
        auto *item = new QListWidgetItem(d->getCaption(), MainLW);
        item->setSizeHint(QSize(0, height() / 20));
        item->setTextAlignment(Qt::AlignCenter);
        MainLW->addItem(item);
        MainTW->addWidget(d);
    }

    connect(MainLW, &QListWidget::currentRowChanged, MainTW, &QStackedWidget::setCurrentIndex);

    MainTW->show();
    MainLW->show();
    qDebug() << MainTW->width() << width();
    MainLW->setMinimumWidth(width() / 6);
    MainLW->setMaximumWidth(width() / 5);
    AlrmTimer->start();
    qInfo() << NAMEOF(MainTW) << "created";

    BdaTimer->start();
    auto *msgSerialNumber = statusBar()->findChild<QLabel *>("SerialNumber");
    msgSerialNumber->setText(QString::number(board.serialNumber(Board::BaseMezzAdd), 16));
    auto *msgModel = statusBar()->findChild<QLabel *>("Model");
    msgModel->setText(board.moduleName());
}

bool Coma::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result)
#ifdef __linux
    Q_UNUSED(message)
#endif
    if (eventType == "windows_generic_MSG")
    {
#ifdef _WIN32
        MSG *msg = static_cast<MSG *>(message);
        int msgType = msg->message;
        if (msgType != WM_DEVICECHANGE)
            return false;
        emit sendMessage(message);

        if (BdaTimer->isActive())
            BdaTimer->stop();
        if (AlrmTimer->isActive())
            AlrmTimer->stop();
        if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
        {
            BdaTimer->start();
            AlrmTimer->start();
        }
#endif
    }

    return false;
}

void Coma::SetMode(int mode)
{
    Mode = mode;
}

void Coma::Go(const QString &parameter)
{
    Q_UNUSED(parameter)
    if (Mode != COMA_GENERALMODE)
        StdFunc::SetEmulated(true);
    SetupUI();
    show();
}

void Coma::ReConnect()
{
    qInfo(__PRETTY_FUNCTION__);
    if (!Reconnect)
        return;

    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
    {
        qDebug() << "call Disconnect";
        Disconnect();
        clearWidgets();
        MainTW->hide();
        StdFunc::SetEmulated(false);
    }

    QMessageBox msgBox;
    msgBox.setText("Связь разорвана.\nПопытка переподключения");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.show();
    msgBox.button(QMessageBox::Ok)->animateClick(3000);
    AttemptToRec();
}

void Coma::AttemptToRec()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    SaveSettings();
    QApplication::restoreOverrideCursor();
    prepareConnectDlg();
}

void Coma::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + PROGNAME + "/";
    QSharedPointer<QSettings> sets = QSharedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    StyleLoader::GetInstance().attach();
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());
}

void Coma::SaveSettings()
{
    QSharedPointer<QSettings> sets = QSharedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    sets->setValue("Homedir", StdFunc::GetHomeDir());
}

void Coma::clearWidgets()
{
    while (MainTW->count())
    {
        QWidget *wdgt = MainTW->widget(0);
        MainTW->removeWidget(wdgt);
        wdgt->deleteLater();
    }
    MainLW->clear();
    MainTW->hide();
    MainLW->hide();
}

// void Coma::setConf(unsigned char typeConf)
//{
//    switch (typeConf)
//    {
//    case 0x01:
//        if (mainConfDialog != nullptr)
//            mainConfDialog->SetDefConf();
//        break;
//    case 0x02:
//        if (confBDialog != nullptr)
//            confBDialog->SetDefConf();
//        break;
//    case 0x03:
//        if (confMDialog != nullptr)
//            confMDialog->SetDefConf();
//        break;
//    default:
//        break;
//    }
//}

// void Coma::Fill()
//{
//    if (mainConfDialog != nullptr)
//        mainConfDialog->Fill();
//    if (confBDialog != nullptr)
//        confBDialog->Fill();
//    if (confMDialog != nullptr)
//        confMDialog->Fill();
//}

// void Coma::FillBSI(IEC104Thread::BS104Signals *sig)
//{
//    unsigned int signum;
//    int startadr = 0;

//    memcpy(&startadr, &(sig->BS.SigAdr[0]), sizeof(sig->BS.SigAdr));
//    signum = sig->SigNumber;
//    INFOMSG("FillBSIe(): address=" + QString::number(startadr));

//    if ((signum < sizeof(ModuleBSI::ModuleBsi)) && (startadr >= BSIREG && startadr <= BSIENDREG))
//    {
//        for (size_t i = 0; i < signum; ++i)
//            memcpy(((quint32 *)(&ModuleBSI::ModuleBsi) + (i + startadr - 1)), (((quint32 *)(&sig->BS.SigVal) + 4 *
//            i)),
//                sizeof(sig->BS.SigVal));
//        Board::GetInstance().setTypeB(ModuleBSI::ModuleBsi.MTypeB);
//        Board::GetInstance().setTypeM(ModuleBSI::ModuleBsi.MTypeM);
//    }
//    if (AlarmStateAllDialog != nullptr)
//        AlarmStateAllDialog->UpdateHealth(ModuleBSI::ModuleBsi.Hth);
//}

// void Coma::FillBSI(QList<ModBus::BSISignalStruct> sig, unsigned int sigsize)
//{
//    int startadr = sig.at(0).SigAdr;

//    INFOMSG("FillBSIm()");

//    if ((sigsize < sizeof(ModuleBSI::ModuleBsi)) && (startadr == BSIREG))
//    {
//        for (size_t i = 0; i < sigsize; ++i)
//            memcpy(((quint32 *)(&ModuleBSI::ModuleBsi) + (i + startadr - 1)), &sig.at(i).Val, sizeof(sig.at(i).Val));
//        Board::GetInstance().setTypeB(ModuleBSI::ModuleBsi.MTypeB);
//        Board::GetInstance().setTypeM(ModuleBSI::ModuleBsi.MTypeM);
//    }
//    if (AlarmStateAllDialog != nullptr)
//        AlarmStateAllDialog->UpdateHealth(ModuleBSI::ModuleBsi.Hth);
//}

// void Coma::PasswordCheck(QString psw)
//{
//    if (psw == "se/7520a")
//        PasswordValid = true;
//    else
//        PasswordValid = false;
//    emit PasswordChecked();
//}

// void Coma::SetProgressBar1Size(int size)
//{
//    //    fileSize = size;
//    SetProgressBarSize(1, size);
//}

// void Coma::SetProgressBar1Count(int count)
//{
//    //    m_curFileCount = count;
//    //    ReceiveTimer->stop();
//    //    ReceiveTimer->setInterval(5000);
//    SetProgressBarCount(1, count);
//    //    ReceiveTimer->start();
//}

// void Coma::FileTimeOut()
//{
//    QString prbname = "prb1prb";
//    QString lblname = "prb1lbl";
//    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
//    if (prb == nullptr)
//    {
//        // qCritical(logCritical(), ("Пустой prb"));
//        qDebug("Пустой prb");
//        return;
//    }
//    WDFunc::SetLBLText(this, lblname, StdFunc::PrbMessage() + QString::number(0), false);

//    ReceiveTimer->stop();
//    if (fileSize != curfileSize && Board::GetInstance().interfaceType() != Board::InterfaceType::USB)
//        QMessageBox::information(this, "Ошибка", "Ошибка", QMessageBox::Ok);
//}

// void Coma::setGeneralProgressBarSize(quint32 size)
//{
//    SetProgressBarSize(2, size);
//    SetProgressBarCount(2, 0);
//}

// void Coma::setGeneralProgressBarCount(quint32 count)
//{
//    SetProgressBarCount(2, count);
//}

void Coma::SetProgressBarSize(int prbnum, int size)
{
    QString prbname = "prb" + QString::number(prbnum) + "prb";
    QString lblname = "prb" + QString::number(prbnum) + "lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == nullptr)
    {
        qDebug("Пустой prb");
        return;
    }
    WDFunc::SetLBLText(this, lblname, QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(size);
}

void Coma::SetProgressBarCount(int prbnum, int count)
{
    QString prbname = "prb" + QString::number(prbnum) + "prb";
    QString lblname = "prb" + QString::number(prbnum) + "lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb != nullptr)
    {
        prb->setValue(count);
        WDFunc::SetLBLText(this, lblname, QString::number(count) + " из " + QString::number(prb->maximum()));
    }
}

void Coma::GetAbout()
{
    AboutWidget *w = new AboutWidget;
    w->show();
}

void Coma::Disconnect()
{
    qInfo(__PRETTY_FUNCTION__);
    BdaTimer->stop();
    AlarmW->clear();
    if (StdFunc::IsInEmulateMode())
        return;

    BaseInterface::iface()->stop();

    //     emit StopCommunications();
    // while (ActiveThreads) // wait for all threads to finish
    //    QCoreApplication::processEvents();
    //  }
    // Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
}

void Coma::Connect()
{
    auto const &board = Board::GetInstance();
    connect(&DataManager::GetInstance(), &DataManager::bitStringReceived, &Board::GetInstance(), &Board::update);
    BaseInterface::InterfacePointer device;
    switch (board.interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        device = BaseInterface::InterfacePointer(new Protocom());
        break;
    }
    case Board::InterfaceType::Ethernet:
    {
#ifndef AVM_DEBUG
        device = BaseInterface::InterfacePointer(new IEC104());
#endif
        break;
    }
    case Board::InterfaceType::RS485:
    {
#ifndef AVM_DEBUG
        device = BaseInterface::InterfacePointer(new ModBus());
#endif
        break;
    }
    default:
        qFatal("Connection type error");
    }
    BaseInterface::setIface(std::move(device));

    connect(BaseInterface::iface(), &BaseInterface::stateChanged, [](const BaseInterface::State state) {
        using State = BaseInterface::State;
        switch (state)
        {
        case State::Run:
            Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
            break;
        case State::Stop:
            Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
            break;
        case State::Wait:
            Board::GetInstance().setConnectionState(Board::ConnectionState::AboutToFinish);
            break;
        default:
            break;
        }
    });

    auto connection = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    *connection = connect(&board, &Board::readyRead, this, [=]() {
        QObject::disconnect(*connection);
        QApplication::restoreOverrideCursor();
        prepare();
    });
    if (DataManager::queueSize() != 0)
        DataManager::clearQueue();
    if (!BaseInterface::iface()->start(ConnectSettings))
    {
        QObject::disconnect(*connection);
        QMessageBox::critical(this, "Ошибка", "Не удалось установить связь", QMessageBox::Ok);
        QApplication::restoreOverrideCursor();
        qCritical() << "Cannot connect" << Error::GeneralError;

        return;
    }
    QTimer timer;
    timer.setSingleShot(true);
    timer.start(INTERVAL::WAIT);
    connect(&timer, &QTimer::timeout, this, [=] {
        if (Board::GetInstance().type() != 0)
            return;
        QObject::disconnect(*connection);
        QMessageBox::critical(this, "Ошибка", "Не удалось соединиться с прибором", QMessageBox::Ok);
        DisconnectAndClear();
        qCritical() << "Cannot connect" << Error::Timeout;
        QApplication::restoreOverrideCursor();
    });

    BaseInterface::iface()->reqBSI();
}

void Coma::DisconnectAndClear()
{
    qInfo(__PRETTY_FUNCTION__);
    const auto &board = Board::GetInstance();
    if (board.connectionState() == Board::ConnectionState::Closed)
        return;

    AlarmW->clear();
    Disconnect();
    m_Module->closeDialogs();

    clearWidgets();

    if (Reconnect)
        QMessageBox::information(this, "Разрыв связи", "Связь разорвана", QMessageBox::Ok, QMessageBox::Ok);
    else
        QMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");

    StdFunc::SetEmulated(false);

    Reconnect = false;
}

void Coma::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
}

void Coma::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        StdFunc::cancel();
    QMainWindow::keyPressEvent(e);
}

void Coma::MainTWTabChanged(int tabindex)
{
    // Q_ASSERT(m_Module != nullptr);
    m_Module->parentTWTabChanged(tabindex);
    //    if (tabindex == CurTabIndex) // to prevent double function invocation by doubleclicking on tab
    //        return;
    //    CurTabIndex = tabindex;

    //    if (Board::GetInstance().interfaceType() == Board::InterfaceType::RS485)
    //        ChModbus->Tabs(tabindex);
    //    if (corDialog != nullptr)
    //        corDialog->GetCorBd(tabindex);
    //    //    if (checkBDialog != nullptr || HarmDialog != nullptr || VibrDialog != nullptr)
    //    //    {
    //    //        if (tabindex == CheckIndex || tabindex == CheckHarmIndex || tabindex == CheckVibrIndex)
    //    //            BdaTimer->start();
    //    //        else
    //    //            BdaTimer->stop();
    //    //    }

    //    if (checkBDialog != nullptr)
    //    {
    //        if (tabindex == CheckIndex)
    //            BdaTimer->start();
    //        else
    //            BdaTimer->stop();
    //    }
    //    if (HarmDialog != nullptr)
    //    {
    //        if (tabindex == CheckHarmIndex)
    //            HarmTimer->start();
    //        else
    //            HarmTimer->stop();
    //    }

    //    if (VibrDialog != nullptr)
    //    {
    //        if (tabindex == CheckVibrIndex)
    //            VibrTimer->start();
    //        else
    //            VibrTimer->stop();
    //    }

    //    if (timeDialog != nullptr)
    //    {
    //        if (tabindex == TimeIndex)
    //            TimeTimer->start();
    //        else
    //            TimeTimer->stop();
    //    }
    //    else
    //        TimeTimer->stop();

    //    if (confMDialog != nullptr)
    //    {
    //        if (tabindex == ConfIndex)
    //            confMDialog->ReadConf();
    //    }
}

// void Coma::SetDefConf()
//{
//    m_Module->setDefConf();
//    //    for (unsigned char i = 0x00; i != 0x03;)
//    //        setConf(++i);
//    //    Fill();
//    QMessageBox::information(this, "Успешно", "Конфигурация по умолчанию", QMessageBox::Ok);
//}

void Coma::update(const DataTypes::GeneralResponseStruct &rsp)
{
    //    DataTypes::GeneralResponseStruct rs;
    //    if (DataManager::getResponse(DataTypes::GeneralResponseTypes::DataCount, rs) != Error::Msg::ResEmpty)
    if (rsp.type == DataTypes::GeneralResponseTypes::DataCount)
        SetProgressBarCount(1, rsp.data);
    //        SetProgressBar1Count(rsp.data);
    //    if (DataManager::getResponse(DataTypes::GeneralResponseTypes::DataSize, rs) != Error::Msg::ResEmpty)
    if (rsp.type == DataTypes::GeneralResponseTypes::DataSize)
        //        SetProgressBar1Size(rsp.data);
        SetProgressBarSize(1, rsp.data);
    //    if (ModuleBSI::update())
    //    {
    //        if (AlarmStateAllDialog != nullptr)
    //            AlarmStateAllDialog->UpdateHealth(ModuleBSI::ModuleBsi.Hth);
    //    }
    // send alarms query
    //    Warn *w = m_Module->getWarn();
    //    m_iface->reqAlarms(w->m_startWarnAddress, 32);
}

void Coma::closeEvent(QCloseEvent *event)
{
    DisconnectAndClear();
    event->accept();
}
