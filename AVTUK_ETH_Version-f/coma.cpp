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

#include "../check/checkdialogharmonickdv.h"
#include "../check/checkdialogharmonicktf.h"
#include "../check/checkdialogkdv.h"
#include "../check/checkdialogkiv.h"
#include "../check/checkdialogktf.h"
#include "../check/checkdialogvibrkdv.h"
#include "../config/confdialogkdv.h"
#include "../config/confdialogkiv.h"
#include "../config/confdialogktf.h"
#include "../dialogs/keypressdialog.h"
#include "../gen/logger.h"
#include "../widgets/wd_func.h"
#ifdef AVM_DEBUG
#include "../tune/tunedialogKIV.h"
#endif
#include "../dialogs/errordialog.h"
#include "../dialogs/settingsdialog.h"
#include "../gen/errorqueue.h"
#include "../gen/stdfunc.h"

#include <QApplication>
#include <QDir>
#include <QMenuBar>
#include <QProgressBar>
#include <QSplashScreen>
#include <QToolBar>
#include <QtGlobal>
#include <functional>

#ifdef _WIN32
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
void registerForDeviceNotification(Coma *ptr)
{
    DEV_BROADCAST_DEVICEINTERFACE devInt;
    ZeroMemory(&devInt, sizeof(devInt));
    devInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    devInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    devInt.dbcc_classguid = { 0x25dbce51, 0x6c8f, 0x4a72, 0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 };

    HDEVNOTIFY blub;
    blub = RegisterDeviceNotification((HDEVNOTIFY)ptr->winId(), &devInt, DEVICE_NOTIFY_WINDOW_HANDLE);
}
#endif

Coma::Coma(QWidget *parent) : QMainWindow(parent)
{
    QSplashScreen *splash = new QSplashScreen(QPixmap("images/2.1.x.png"));
    splash->show();
    splash->showMessage("Подготовка окружения...", Qt::AlignRight, Qt::white);
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
    timeDialog = nullptr;
    mainConfDialog = nullptr;
    confBDialog = confMDialog = nullptr;
    checkBDialog = checkMDialog = nullptr;
    AlarmStateAllDialog = nullptr;
    AvarAlarmKIVDialog = nullptr;
    WarnAlarmKIVDialog = nullptr;
    AvarAlarmKTFDialog = nullptr;
    WarnAlarmKTFDialog = nullptr;
#ifdef AVM_DEBUG
    tuneDialog = nullptr;
#endif
    HarmDialog = nullptr;
    VibrDialog = nullptr;
    corDialog = nullptr;
    S2Config = nullptr;
    S2ConfigForTune = nullptr;
    CurTabIndex = -1;
    for (int i = 0; i < 20; ++i)
    {
        PredAlarmEvents[i] = 0;
        AlarmEvents[i] = 0;
    }
    ActiveThreads = 0;
    Alarm = new AlarmClass(this);

    NewTimers();
    LoadSettings();

    splash->finish(this);
    splash->deleteLater();
    setStatusBar(WDFunc::NewSB(this));
}

Coma::~Coma()
{
    //    Disconnect();
}
void convertPixmap(size_t size, QAction *jourAct)
{
    const QIcon jourIcon("images/skull-and-bones.png");
    QPixmap pix = jourIcon.pixmap(QSize(40, 40), QIcon::Disabled);
    QPainter painter(&pix);
    painter.drawPixmap(QRect(20, 0, 20, 20), WDFunc::NewCircle(Qt::red, 20));
    QFont font(painter.font());
    font.setPixelSize(14);
    painter.setFont(font);
    if (size > 10)
        painter.drawText(QRect(20, 0, 20, 20), Qt::AlignCenter, "...");
    else
        painter.drawText(QRect(20, 0, 20, 20), Qt::AlignCenter, QString::number(size));
    painter.setPen(Qt::yellow);
    jourAct->setIcon(pix);
}

QToolBar *Coma::createToolBar()
{
    QToolBar *tb = new QToolBar(this);
    tb->setContextMenuPolicy(Qt::PreventContextMenu);
    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(40, 40));
    tb->addAction(QIcon("images/play.png"), "Соединение", this, &Coma::StartWork);
    tb->addAction(QIcon("images/stop.png"), "Разрыв соединения", this, &Coma::DisconnectAndClear);
    tb->addSeparator();
    tb->addAction(QIcon("images/settings.png"), "Настройки", [this]() {
        SettingsDialog *dlg = new SettingsDialog;
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
        this->SaveSettings();
    });
    const QIcon jourIcon("images/skull-and-bones.png");

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
    setWindowTitle(PROGCAPTION);
    QString tmps = "QMainWindow {background-color: " + QString(Colors::MAINWINCLR) + ";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(1050, 700));
    QWidget *wdgt = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout(wdgt);
    QHBoxLayout *hlyout = new QHBoxLayout;

    hlyout->addWidget(createToolBar());

    AlarmW = new AlarmWidget(Alarm);
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

    ETabWidget *MainTW = new ETabWidget;
    MainTW->setObjectName("maintw");
    MainTW->setTabPosition(QTabWidget::West);
    inlyout->addWidget(MainTW, 60);
    MainTW->hide();

    QFrame *line = new QFrame;
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    line->setFrameStyle(QFrame::Sunken | QFrame::HLine);
    lyout->addWidget(line);

    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBLT(this, "Обмен"));
    inlyout->addWidget(WDFunc::NewLBLT(this, "", "prb1lbl"));

    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prb1prb");
    prb->setOrientation(Qt::Horizontal);
    // prb->setMinimumWidth(50);
    prb->setMaximumHeight(height() / 50);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);

    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBLT(this, "Отсчёт"));
    inlyout->addWidget(WDFunc::NewLBLT(this, "", "prb2lbl"));

    prb = new QProgressBar;
    prb->setObjectName("prb2prb");
    prb->setOrientation(Qt::Horizontal);
    // prb->setMinimumWidth(50);
    prb->setMaximumHeight(height() / 50);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    w->setLayout(lyout);
    return w;
}
void Coma::SetupMenubar()
{
    QMenuBar *menubar = new QMenuBar(this);
    QString tmps = "QMenuBar {background-color: " + QString(Colors::MAINWINCLRA1)
        + ";}"
          "QMenuBar::item {background-color: "
        + QString(Colors::MAINWINCLRA1) + ";}";
    menubar->setStyleSheet(tmps);
    QMenu *menu = new QMenu(this);
    menu->setTitle("Главное");

    menu->addAction("Выход", this, &Coma::close);
    menu->addAction(QIcon("images/play.png"), "Соединение", this, &Coma::StartWork);
    menu->addAction(QIcon("images/stop.png"), "Разрыв соединения", this, &Coma::DisconnectAndClear);

    menubar->addMenu(menu);

    menubar->addAction("О программе", this, &Coma::GetAbout);

    menubar->addSeparator();

    setMenuBar(menubar);
}

void Coma::addConfTab(ETabWidget *MainTW, QString str)
{
    MainTW->addTab(corDialog, str);
    corDialog->corDIndex = MainTW->indexOf(corDialog);
    if (Board::GetInstance().interfaceType() == Board::InterfaceType::RS485)
        ChModbus->CorIndex = corDialog->corDIndex;
}

void Coma::setupDialogs(ETabWidget *MainTW)
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
}

void Coma::StartWork()
{
    auto const &board = Board::GetInstance();
    if (board.connectionState() != Board::ConnectionState::Closed)
        return;
    AlarmStateAllDialog = new AlarmStateAll(this);
    if (!Reconnect)
    {
        QEventLoop loop;
        Cancelled = false;
        ConnectDialog *dlg = new ConnectDialog;
        connect(dlg, &ConnectDialog::Accepted, [this](ConnectDialog::ConnectStruct *st) {
            this->ConnectSettings = *st;
            emit CloseConnectDialog();
        });
        connect(dlg, &ConnectDialog::Cancelled, [this]() {
            this->Cancelled = true;
            emit CloseConnectDialog();
        });
        connect(this, &Coma::CloseConnectDialog, dlg, &ConnectDialog::close);
        connect(this, &Coma::CloseConnectDialog, &loop, &QEventLoop::quit);
        dlg->show();
        loop.exec();
        if (Cancelled)
        {
            // qCritical(logCritical(), ("Отмена подключения"));
            // qCritical(("Отмена подключения"));
            ERMSG("Отмена подключения");
            return;
        }
        // S2ConfigForTune->clear();
        SaveSettings();
    }

    // Stage3

    DisconnectAndClear();

    Board::GetInstance().setTypeB(0);
    Board::GetInstance().setTypeM(0);
    S2Config = new QVector<S2::DataRec>;
    S2ConfigForTune = new QVector<S2::DataRec>;
    CurTabIndex = -1;
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == nullptr)
    {
        // qCritical(logCritical(), ("MainTW is empty"));
        ERMSG("MainTW is empty");
        return;
    }
    connect(MainTW, &ETabWidget::tabClicked, this, &Coma::MainTWTabClicked);

    Connect();
    QElapsedTimer tmr;
    tmr.start();
    while ((board.typeM() == 0) && (tmr.elapsed() < INTERVAL::WAIT) && !Cancelled)
        QCoreApplication::processEvents();
    if (board.typeM() == 0)
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось соединиться с прибором", QMessageBox::Ok);
        DisconnectAndClear();
        ERMSG("Не получили BSI, нет соединения");
        //            Disconnect();
        return;
    }
    Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
    quint16 serialNumber = Board::GetInstance().type();
    QString deviceName = QVariant::fromValue(Board::DeviceModel(serialNumber)).toString();
    QMessageBox::information(this, "Связь установлена", "Удалось установить связь с " + deviceName, QMessageBox::Ok);
    Reconnect = true;

    PrepareDialogs();

    setupDialogs(MainTW);

    if (board.interfaceType() != Board::InterfaceType::RS485)
        MainTW->addTab(jourDialog, "Журналы");

    if (ModuleBSI::Health() & HTH_CONFIG) // нет конфигурации
        qCritical() << QVariant::fromValue(Error::Msg::ER_NOCONF).toString();
    if (ModuleBSI::Health() & HTH_REGPARS) // нет коэффициентов
        qCritical() << QVariant::fromValue(Error::Msg::ER_NOTUNECOEF).toString();
    if (board.interfaceType() == Board::InterfaceType::USB)
    {
        fwUpDialog = new fwupdialog;
        MainTW->addTab(fwUpDialog, "Загрузка ВПО");
    }

#ifdef AVM_DEBUG
    if (tuneDialog != nullptr)
        MainTW->addTab(tuneDialog, "Регулировка");
#endif
    MainTW->addTab(infoDialog, "О приборе");
    infoDialog->FillBsi();

    MainTW->repaint();
    MainTW->show();
    AlrmTimer->start();
    INFOMSG("MainTW created");
    if (board.interfaceType() == Board::InterfaceType::USB)
        BdaTimer->start();
    auto *msgSerialNumber = statusBar()->findChild<QLabel *>("SerialNumber");
    msgSerialNumber->setText(QString::number(ModuleBSI::SerialNum(BT_NONE), 16));
}

void Coma::setupQConnections()
{
    connect(AlarmW, &AlarmWidget::AlarmButtonPressed, AlarmStateAllDialog, &QDialog::show);

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

            WarnAlarmKIVDialog = new WarnAlarmKIV(Alarm, AlarmW);
            connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, WarnAlarmKIVDialog, &QDialog::show);
            connect(Alarm, &AlarmClass::SetWarnAlarmColor, WarnAlarmKIVDialog, &AbstractWarnAlarm::Update);

            AvarAlarmKIVDialog = new AvarAlarmKIV(Alarm, AlarmW);
            connect(AlarmW, &AlarmWidget::ModuleAlarmButtonPressed, AvarAlarmKIVDialog, &QDialog::show);
            connect(Alarm, &AlarmClass::SetAlarmColor, AvarAlarmKIVDialog, &AbstractAvarAlarm::Update);

            connect(Alarm, SIGNAL(SetWarnAlarmColor(QList<bool>)), checkBDialog, SLOT(SetWarnAlarmColor(QList<bool>)));
            connect(Alarm, SIGNAL(SetAlarmColor(QList<bool>)), checkBDialog, SLOT(SetAlarmColor(QList<bool>)));

#ifdef AVM_DEBUG
            tuneDialog = new TuneDialogKIV(S2ConfigForTune);
#endif
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

            WarnAlarmKTFDialog = new WarnAlarmKTF(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, WarnAlarmKTFDialog, &QDialog::show);
            connect(Alarm, &AlarmClass::SetWarnAlarmColor, WarnAlarmKTFDialog, &AbstractWarnAlarm::Update);

            AvarAlarmKTFDialog = new AvarAlarmKTF(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleAlarmButtonPressed, AvarAlarmKTFDialog, &QDialog::show);
            connect(Alarm, &AlarmClass::SetAlarmColor, AvarAlarmKTFDialog, &AbstractAvarAlarm::Update);

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
}

void Coma::PrepareDialogs()
{
    infoDialog = new InfoDialog(this);
    jourDialog = new JournalDialog(Ch104);
    timeDialog = new MNKTime(this);

    setupQConnections();
}

void Coma::CloseDialogs()
{

    AlrmTimer->stop();

    QList<QDialog *> widgets = findChildren<QDialog *>();

    for (auto &i : widgets)
    {
        qDebug() << i;
        i->close();
        i->deleteLater();
    }
}

void Coma::New104()
{
    Ch104 = new IEC104(S2Config);
    connect(this, &Coma::StopCommunications, Ch104, &IEC104::StopAllThreads);
    connect(Ch104, &IEC104::Finished, [this]() { ActiveThreads &= ~THREAD::P104; });
    // connect(Ch104,SIGNAL(Sponsignalsready(IEC104Thread::SponSignals*)),this,SLOT(UpdatePredAlarmEvents(IEC104Thread::SponSignals*)));
    connect(Ch104, &IEC104::SetDataSize, this, &Coma::SetProgressBar1Size);
    connect(Ch104, &IEC104::SetDataCount, this, &Coma::SetProgressBar1);
    connect(Ch104, &IEC104::ReconnectSignal, this, &Coma::ReConnect);
    connect(Ch104, &IEC104::Sponsignalsready, Alarm, &AlarmClass::UpdateAlarm104);
    connect(Ch104, &IEC104::Bs104signalsready, this, qOverload<IEC104Thread::BS104Signals *>(&Coma::FillBSI));
}

void Coma::NewModbus()
{
    ChModbus = new ModBus;
    connect(this, &Coma::StopCommunications, ChModbus, &ModBus::Stop);
    connect(ChModbus, &ModBus::Finished, [this]() { ActiveThreads &= ~THREAD::MODBUS; });
    //  connect(ChModbus,SIGNAL(CoilSignalsReady(ModBus::Coils)), this,
    //  SLOT(ModBusUpdatePredAlarmEvents(ModBus::Coils)));
    connect(ChModbus, &ModBus::ReconnectSignal, this, &Coma::ReConnect);
    connect(ChModbus, &ModBus::CoilSignalsReady, Alarm, &AlarmClass::UpdateAlarmModBus);

    connect(ChModbus, &ModBus::BsiFromModbus, this,
        qOverload<QList<ModBus::BSISignalStruct>, unsigned int>(&Coma::FillBSI));
}

void Coma::NewUSB()
{
    connect(this, &Coma::StopCommunications, &EProtocom::GetInstance(), &EProtocom::Disconnect);
    connect(EProtocom::GetInstance().workerThread(), &QThread::finished, [=]() { ActiveThreads &= ~THREAD::USB; });
    connect(&EProtocom::GetInstance(), &EProtocom::SetDataSize, this, &Coma::SetProgressBar1Size);
    connect(&EProtocom::GetInstance(), &EProtocom::SetDataCount, this, &Coma::SetProgressBar1);
    connect(&EProtocom::GetInstance(), &EProtocom::ShowError,
        [this](const QString &msg) { QMessageBox::critical(this, "Ошибка", msg, QMessageBox::Ok); });
}

void Coma::NewTimers()
{
    TimeTimer = new QTimer(this);
    TimeTimer->setInterval(1000);

    BdaTimer = new QTimer(this);
    BdaTimer->setInterval(ANMEASINT);

    HarmTimer = new QTimer(this);
    HarmTimer->setInterval(ANMEASINT);

    VibrTimer = new QTimer(this);
    VibrTimer->setInterval(ANMEASINT);

    AlrmTimer = new QTimer(this);
    AlrmTimer->setInterval(10000);

    ReceiveTimer = new QTimer(this);
    ReceiveTimer->setInterval(ANMEASINT);
    connect(ReceiveTimer, &QTimer::timeout, this, &Coma::FileTimeOut);

    ReconnectTimer = new QTimer(this);
    ReconnectTimer->setInterval(INTERVAL::RECONNECT);
    ReconnectTimer->setSingleShot(true);
    connect(ReconnectTimer, &QTimer::timeout, this, &Coma::AttemptToRec);
}

void Coma::NewTimersBda()
{
    connect(BdaTimer, &QTimer::timeout, Alarm, &AlarmClass::UpdateAlarmUSB);
    //   connect(BdaTimer, &QTimer::timeout, AlarmStateAllDialog, &AlarmStateAll::UpdateHealth);

    auto checkDialogs = this->findChildren<EAbstractCheckDialog *>();
    for (const auto &dialog : checkDialogs)
    {
        qDebug() << dialog;
        connect(BdaTimer, &QTimer::timeout, dialog, &EAbstractCheckDialog::USBUpdate);
    }
    if (AlarmStateAllDialog != nullptr)
        connect(AlrmTimer, &QTimer::timeout, [&]() { AlarmStateAllDialog->UpdateHealth(ModuleBSI::ModuleBsi.Hth); });
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
        if (msgType == WM_DEVICECHANGE)
        {
            if (BdaTimer->isActive())
                BdaTimer->stop();
            if (AlrmTimer->isActive())
                AlrmTimer->stop();
            EProtocom::GetInstance().usbStateChanged(message);
            if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected
                && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
            {
                BdaTimer->start();
                AlrmTimer->start();
            }
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
    if (Reconnect)
    {
        // QDialog *dlg = new QDialog;

        INFOMSG("Reconnect()");
        TimeTimer->stop();
        if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
        {
            qDebug() << "call Disconnect";
            Disconnect();
            emit ClearBsi();
            ClearTW();
            ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
            if (MainTW == nullptr)
            {
                ERMSG("Ошибка открытия файла");
                return;
            }
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
}

void Coma::AttemptToRec()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    SaveSettings();
    QApplication::restoreOverrideCursor();
    StartWork();
}

void Coma::ConnectMessage()
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    // if (MainInterface == I_USB)
    if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
        msgBox.setText("Связь с " + ModuleBSI::ModuleTypeString + " установлена");
    else
        msgBox.setText("Связь с " + ConnectSettings.name + " установлена");
    msgBox.show();
    StdFunc::Wait(INTERVAL::WAIT);
    msgBox.close();
}

void Coma::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + PROGNAME + "/";
    QSharedPointer<QSettings> sets = QSharedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());
}

void Coma::SaveSettings()
{
    QSharedPointer<QSettings> sets = QSharedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    sets->setValue("Homedir", StdFunc::GetHomeDir());
}

void Coma::ClearTW()
{
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == nullptr)
    {
        ERMSG("Пустой MainTW");
        return;
    }

    while (MainTW->count())
    {
        QWidget *wdgt = MainTW->widget(0);
        MainTW->removeTab(0);
        wdgt->deleteLater();
    }
}

// Error::Msg Coma::CheckPassword()
//{
//    PasswordValid = false;
//    StdFunc::ClearCancel();
//    QEventLoop PasswordLoop;
//    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
//    connect(dlg, &KeyPressDialog::Finished, this, &Coma::PasswordCheck);
//    connect(this, &Coma::PasswordChecked, &PasswordLoop, &QEventLoop::quit);
//    dlg->show();
//    PasswordLoop.exec();
//    if (StdFunc::IsCancelled())
//    {
//        ERMSG("Отмена ввода пароля");
//        return Error::Msg::GeneralError;
//    }

//    if (!PasswordValid)
//    {
//        ERMSG("Пароль введён неверно");
//        QMessageBox::critical(this, "Неправильно", "Пароль введён неверно", QMessageBox::Ok);
//        return Error::Msg::GeneralError;
//    }
//    return Error::Msg::NoError;
//}

void Coma::setConf(unsigned char typeConf)
{
    switch (typeConf)
    {
    case 0x01:
        if (mainConfDialog != nullptr)
            mainConfDialog->SetDefConf();
        break;
    case 0x02:
        if (confBDialog != nullptr)
            confBDialog->SetDefConf();
        break;
    case 0x03:
        if (confMDialog != nullptr)
            confMDialog->SetDefConf();
        break;
    default:
        break;
    }
}

void Coma::Fill()
{
    if (mainConfDialog != nullptr)
        mainConfDialog->Fill();
    if (confBDialog != nullptr)
        confBDialog->Fill();
    if (confMDialog != nullptr)
        confMDialog->Fill();
}

void Coma::FillBSI(IEC104Thread::BS104Signals *sig)
{
    unsigned int signum;
    int startadr = 0;

    memcpy(&startadr, &(sig->BS.SigAdr[0]), sizeof(sig->BS.SigAdr));
    signum = sig->SigNumber;
    qInfo() << "FillBSIe(): address=" << QString::number(startadr);

    if ((signum < sizeof(ModuleBSI::ModuleBsi)) && (startadr >= BSIREG && startadr <= BSIENDREG))
    {
        for (size_t i = 0; i < signum; ++i)
            memcpy(((quint32 *)(&ModuleBSI::ModuleBsi) + (i + startadr - 1)), (((quint32 *)(&sig->BS.SigVal) + 4 * i)),
                sizeof(sig->BS.SigVal));
        Board::GetInstance().setTypeB(ModuleBSI::ModuleBsi.MTypeB);
        Board::GetInstance().setTypeM(ModuleBSI::ModuleBsi.MTypeM);
    }
    if (AlarmStateAllDialog != nullptr)
        AlarmStateAllDialog->UpdateHealth(ModuleBSI::ModuleBsi.Hth);
}

void Coma::FillBSI(QList<ModBus::BSISignalStruct> sig, unsigned int sigsize)
{
    int startadr = sig.at(0).SigAdr;

    INFOMSG("FillBSIm()");

    if ((sigsize < sizeof(ModuleBSI::ModuleBsi)) && (startadr == BSIREG))
    {
        for (size_t i = 0; i < sigsize; ++i)
            memcpy(((quint32 *)(&ModuleBSI::ModuleBsi) + (i + startadr - 1)), &sig.at(i).Val, sizeof(sig.at(i).Val));
        Board::GetInstance().setTypeB(ModuleBSI::ModuleBsi.MTypeB);
        Board::GetInstance().setTypeM(ModuleBSI::ModuleBsi.MTypeM);
    }
    if (AlarmStateAllDialog != nullptr)
        AlarmStateAllDialog->UpdateHealth(ModuleBSI::ModuleBsi.Hth);
}

// void Coma::PasswordCheck(QString psw)
//{
//    if (psw == "se/7520a")
//        PasswordValid = true;
//    else
//        PasswordValid = false;
//    emit PasswordChecked();
//}

void Coma::SetProgressBar1Size(int size)
{
    fileSize = size;
    SetProgressBarSize(1, size);
}

void Coma::SetProgressBar1(int cursize)
{
    curfileSize = cursize;
    ReceiveTimer->stop();
    ReceiveTimer->setInterval(5000);
    SetProgressBar(1, cursize);
    ReceiveTimer->start();
}

void Coma::FileTimeOut()
{
    QString prbname = "prb1prb";
    QString lblname = "prb1lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == nullptr)
    {
        // qCritical(logCritical(), ("Пустой prb"));
        qDebug("Пустой prb");
        return;
    }
    WDFunc::SetLBLText(this, lblname, StdFunc::PrbMessage() + QString::number(0), false);

    ReceiveTimer->stop();
    if (fileSize != curfileSize && Board::GetInstance().interfaceType() != Board::InterfaceType::USB)
        QMessageBox::information(this, "Ошибка", "Ошибка", QMessageBox::Ok);
}

void Coma::SetProgressBar2Size(int size)
{
    SetProgressBarSize(2, size);
}

void Coma::SetProgressBar2(int cursize)
{
    SetProgressBar(2, cursize);
}

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
    WDFunc::SetLBLText(this, lblname, StdFunc::PrbMessage() + QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(size);
}

void Coma::SetProgressBar(int prbnum, int cursize)
{
    QString prbname = "prb" + QString::number(prbnum) + "prb";
    QString lblname = "prb" + QString::number(prbnum) + "lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb != nullptr)
    {
        prb->setValue(cursize);
        WDFunc::SetLBLText(
            this, lblname, StdFunc::PrbMessage() + QString::number(cursize) + " из " + QString::number(prb->maximum()));
    }
}

void Coma::GetAbout()
{
    setWindowIcon(QPixmap("images/avm-energo.png"));
    QMessageBox::about(this, PROGCAPTION,
        "ООО \"АВМ-Энерго\" \n"
        "2015-2020 гг.\n"
        "info@avmenergo.ru");
}

void Coma::Disconnect()
{
    INFOMSG("Disconnect()");
    AlarmW->Clear();
    if (!StdFunc::IsInEmulateMode())
    {
        if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
        {
            BdaTimer->stop();
            if (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
                EProtocom::GetInstance().Disconnect();
        }
        else
        {
            emit StopCommunications();
            while (ActiveThreads) // wait for all threads to finish
                QCoreApplication::processEvents();
        }
        Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
    }
}

void Coma::Connect()
{
    auto const &board = Board::GetInstance();
    Error::Msg res;
    switch (board.interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        NewUSB();
        res = Commands::Connect();
        if (res != Error::Msg::NoError)
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось установить связь", QMessageBox::Ok);
            QApplication::restoreOverrideCursor();
            ERMSG("cn: can't connect");
            return;
        }
        res = ModuleBSI::SetupBSI();
        if (res != Error::Msg::NoError)
        {
            if (res == Error::Msg::ResEmpty)
            {
                QMessageBox::critical(this, "Ошибка", "Неизвестный тип модуля", QMessageBox::Ok);
                ERMSG("Unknown module type");
                return;
            }
            QMessageBox::critical(this, "Ошибка", "Не удалось установить связь", QMessageBox::Ok);
            ERMSG("BSI read error");
            return;
        }
        ActiveThreads |= THREAD::USB;
        Board::GetInstance().setTypeB(ModuleBSI::GetMType(BoardTypes::BT_BASE));
        Board::GetInstance().setTypeM(ModuleBSI::GetMType(BoardTypes::BT_MEZONIN));
        break;
    }
    case Board::InterfaceType::Ethernet:
    {
        New104();
        if (!Ch104->Working())
            Ch104->Connect(ConnectSettings.iec104st);
        ActiveThreads |= THREAD::P104;
        break;
    }
    case Board::InterfaceType::RS485:
    {
        NewModbus();
        res = ChModbus->Connect(ConnectSettings.serialst);
        if (res != Error::Msg::NoError)
        {
            ERMSG("Modbus not connected");
            return;
        }
        ChModbus->BSIrequest();
        ActiveThreads |= THREAD::MODBUS;
        break;
    }
    default:
        qFatal("Connection type error");
    }
}

void Coma::DisconnectAndClear()
{
    INFOMSG("DisconnectAndClear()");
    TimeTimer->stop();
    if (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
    {
        AlarmW->Clear();
        Disconnect();
        CloseDialogs();
        emit ClearBsi();
        ClearTW();
        ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
        if (MainTW == nullptr)
        {
            ERMSG("Пустой MainTW");
            return;
        }
        if (S2Config)
        {
            S2Config->clear();
            delete S2Config;
        }
        if (S2ConfigForTune)
        {
            S2ConfigForTune->clear();

            delete S2ConfigForTune;
        }
        // Проверить после отключения алармов
        // if (Reconnect)
        //    QMessageBox::information(this, "Разрыв связи", "Связь разорвана", QMessageBox::Ok, QMessageBox::Ok);
        //        else
        //            QMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");
        MainTW->hide();
        StdFunc::SetEmulated(false);
    }

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
        StdFunc::Cancel();
    QMainWindow::keyPressEvent(e);
}

void Coma::MainTWTabClicked(int tabindex)
{
    if (tabindex == CurTabIndex) // to prevent double function invocation by doubleclicking on tab
        return;
    CurTabIndex = tabindex;

    if (Board::GetInstance().interfaceType() == Board::InterfaceType::RS485)
        ChModbus->Tabs(tabindex);
    if (corDialog != nullptr)
        corDialog->GetCorBd(tabindex);
    //    if (checkBDialog != nullptr || HarmDialog != nullptr || VibrDialog != nullptr)
    //    {
    //        if (tabindex == CheckIndex || tabindex == CheckHarmIndex || tabindex == CheckVibrIndex)
    //            BdaTimer->start();
    //        else
    //            BdaTimer->stop();
    //    }

    if (checkBDialog != nullptr)
    {
        if (tabindex == CheckIndex)
            BdaTimer->start();
        else
            BdaTimer->stop();
    }
    if (HarmDialog != nullptr)
    {
        if (tabindex == CheckHarmIndex)
            HarmTimer->start();
        else
            HarmTimer->stop();
    }

    if (VibrDialog != nullptr)
    {
        if (tabindex == CheckVibrIndex)
            VibrTimer->start();
        else
            VibrTimer->stop();
    }

    if (timeDialog != nullptr)
    {
        if (tabindex == TimeIndex)
            TimeTimer->start();
        else
            TimeTimer->stop();
    }
    else
        TimeTimer->stop();

    if (confMDialog != nullptr)
    {
        if (tabindex == ConfIndex)
            confMDialog->ReadConf();
    }
}

void Coma::SetDefConf()
{
    for (unsigned char i = 0x00; i != 0x03;)
        setConf(++i);
    Fill();
    QMessageBox::information(this, "Успешно", "Конфигурация по умолчанию", QMessageBox::Ok);
}

void Coma::closeEvent(QCloseEvent *event)
{
    DisconnectAndClear();
    event->accept();
}
