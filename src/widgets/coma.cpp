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

#include "../avtuk/parseid9050.h"
#include "../avtuk/swjdialog.h"
#include "../avtuk/trendviewdialog.h"
#include "../dialogs/connectdialog.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/keypressdialog.h"
#include "../dialogs/settingsdialog.h"
#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/errorqueue.h"
#include "../gen/files.h"
#include "../gen/logger.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../interfaces/protocom.h"
#include "../interfaces/settingstypes.h"
#include "../widgets/aboutwidget.h"
#include "../widgets/etabwidget.h"
#include "../widgets/splashscreen.h"
#include "../widgets/styleloader.h"
#include "../widgets/wd_func.h"
#include "waitwidget.h"

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
    QString caption(QCoreApplication::applicationName());
    caption.append(" v. ").append(QCoreApplication::applicationVersion());
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
    lyout->addWidget(least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    setupMenubar();
}

QWidget *Coma::least()
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
    connect(MainTW, &QStackedWidget::currentChanged, this, &Coma::mainTWTabChanged);

    QFrame *line = new QFrame;
    lyout->addWidget(line);

    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBL2(this, "Обмен"));
    inlyout->addWidget(WDFunc::NewLBL2(this, "", "prb1lbl"));

    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prb1prb");
    prb->setOrientation(Qt::Horizontal);

    prb->setMaximumHeight(height() / 50);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);

    w->setLayout(lyout);
    return w;
}
void Coma::setupMenubar()
{
    QMenuBar *menubar = new QMenuBar(this);

    QMenu *menu = new QMenu(menubar);
    menu->setTitle("Главное");

    menu->addAction("Выход", this, &Coma::close);
    menu->addAction(QIcon(":/icons/tnstart.svg"), "Соединение", this, &Coma::prepareConnectDlg);
    menu->addAction(QIcon(":/icons/tnstop.svg"), "Разрыв соединения", this, &Coma::DisconnectAndClear);

    menubar->addMenu(menu);

    menubar->addAction("О программе", this, &Coma::getAbout);

    menubar->addSeparator();
    menu = new QMenu(menubar);
    menu->setTitle("Автономная работа");
    menu->addAction("Загрузка осциллограммы", this, &Coma::loadOsc);
    menu->addAction("Загрузка журнала переключений", this, &Coma::loadSwj);
    menubar->addMenu(menu);
    setMenuBar(menubar);
}

void Coma::prepareConnectDlg()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    action->setDisabled(true);
    // qDebug() << sender()->metaObject()->className();
    auto const &board = Board::GetInstance();
    if (board.connectionState() != Board::ConnectionState::Closed)
    {
        action->setEnabled(true);
        return;
    }
    if (!Reconnect)
    {

        QEventLoop loop;
        ConnectDialog *dlg = new ConnectDialog(this);
        connect(dlg, &ConnectDialog::accepted, this, [=](const ConnectStruct st) {
            dlg->close();
            startWork(st);
        });
        dlg->adjustSize();
        dlg->show();
        connect(dlg, &QDialog::destroyed, this, [=] { action->setEnabled(true); });
    }
    else
        action->setEnabled(true);

    // Stage3

    DisconnectAndClear();
}

void Coma::startWork(const ConnectStruct st)
{
    ConnectSettings = st;
    SaveSettings();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    setupConnection();
}

void Coma::loadOsc()
{
    QString filename = Files::ChooseFileForOpen(this, "Oscillogram files (*.osc)");
    LoadOscFromFile(filename);
}

void Coma::LoadOscFromFile(const QString &filename)
{
    // leaks
    EOscillogram *OscFunc = new EOscillogram();

    if (Files::LoadFromFile(filename, OscFunc->BA) == Error::NoError)
    {
        // leaks
        TrendViewDialog *dlg = new TrendViewDialog(OscFunc->BA);
        OscFunc->setParent(dlg);
        // leaks
        TrendViewModel *mdl = new TrendViewModel;
        OscFunc->ProcessOsc(mdl);
        mdl->xmax = (static_cast<float>(mdl->Len / 2));
        mdl->xmin = -mdl->xmax;
        dlg->TrendModel = mdl;

        switch (mdl->idOsc)
        {
        case MT_ID85:
        {

            dlg->SetAnalogNames(mdl->tmpav_85);
            dlg->SetDigitalNames(mdl->tmpdv_85);
            dlg->SetDigitalColors(mdl->dcolors_85);
            dlg->SetAnalogColors(mdl->acolors_85);
            dlg->SetDiscreteDescriptions(mdl->ddescr_85);
            dlg->SetAnalogDescriptions(mdl->adescr_85);
            dlg->SetRanges(mdl->xmin, mdl->xmax, -200, 200);
            break;
        }
        case MT_ID80:
        {
            mdl->tmpdv_80.clear();
            dlg->SetAnalogNames(mdl->tmpav_80);
            dlg->SetDigitalNames(mdl->tmpdv_80);
            dlg->SetDigitalColors(mdl->dcolors_80);
            dlg->SetAnalogColors(mdl->acolors_80);
            dlg->SetRanges(mdl->xmin, mdl->xmax, -200, 200);
            break;
        }

        case MT_ID21:
        {
            // период отсчётов - 20 мс, длительность записи осциллограммы 10 сек, итого 500 точек по 4 байта на каждую
            // mdl->tmpav_21 << QString::number(mdl->idOsc); // пока сделано для одного канала в осциллограмме
            // TrendViewModel *TModel = new TrendViewModel(QStringList(), tmpav, *len);
            // dlg->SetModel(TModel);
            dlg->SetAnalogColors(mdl->acolors_21);
            dlg->SetAnalogNames(mdl->tmpav_21);
            dlg->SetAnalogDescriptions(mdl->adescr_21);
            // 10000 мс, 20 мА (сделать автонастройку в зависимости от конфигурации по данному каналу)
            dlg->SetRanges(0, 10000, -20, 20);

            break;
        }

        case ID_OSC_CH0:
        case ID_OSC_CH0 + 1:
        case ID_OSC_CH0 + 2:
        case ID_OSC_CH0 + 3:
        case ID_OSC_CH0 + 4:
        case ID_OSC_CH0 + 5:
        case ID_OSC_CH0 + 6:
        case ID_OSC_CH0 + 7:
        {

            dlg->SetAnalogNames(mdl->tmpav_85);
            dlg->SetDigitalNames(mdl->tmpdv_85);
            dlg->SetDigitalColors(mdl->dcolors_85);
            dlg->SetAnalogColors(mdl->acolors_85);
            dlg->SetRanges(mdl->xmin, mdl->xmax, -200, 200);
            break;
        }
        }
        dlg->setModal(false);
        dlg->SetupPlots();
        dlg->SetupUI();
        //  dlg->setModal(false);
        dlg->PlotShow();
        dlg->show();
    }
    else
        delete OscFunc;
}

void Coma::loadSwj()
{
    QString filename = Files::ChooseFileForOpen(this, "Switch journal files (*.swj)");
    loadSwjFromFile(filename);
}

void Coma::loadSwjFromFile(const QString &filename)
{
    QByteArray ba;
    // bool haveosc;
    int SWJRSize = sizeof(S2DataTypes::SWJournalRecordStruct);
    qDebug() << SWJRSize << sizeof(S2DataTypes::SWJINFStruct);
    // return;
    // int GBOSize = sizeof(EOscillogram::GBoStruct);
    float value;
    QString str, tmps;
    EOscillogram *OscFunc = new EOscillogram(this);
    if (Files::LoadFromFile(filename, OscFunc->BA) == Error::NoError)
    {
        QStringList phase = { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };

        if (OscFunc->BA.size() < (SWJRSize))
        {
            QMessageBox::warning(this, "Ошибка", "Некорректная структура файла журнала");
            return;
        }
        S2DataTypes::SWJournalRecordStruct SWJ;
        size_t tmpi = static_cast<size_t>(SWJRSize);
        memcpy(&SWJ, &(OscFunc->BA.data()[0]), tmpi); // копируем информацию о переключении
        S2DataTypes::SWJINFStruct swjr;
        swjr.FileLength = OscFunc->BA.size();
        // EOscillogram::GBoStruct gbos;
        // tmpi = static_cast<size_t>(GBOSize);
        // memcpy(&gbos, &(ba.data()[SWJRSize]), tmpi); // копируем информацию об осциллограмме
        /* ba.remove(0, SWJRSize); // оставляем только саму осциллограмму
        if (ba.isEmpty()) // осциллограммы в журнале нет
            haveosc = false;
        else
            haveosc = true;*/
        // dlg->Init(swjr, haveosc, gbos);

        // if (!ba.isEmpty())
        //   dlg->LoadOsc(ba);

        QVBoxLayout *vlyout = new QVBoxLayout;
        QGridLayout *glyout = new QGridLayout;

        SWJDialog *dlg = new SWJDialog(OscFunc, SWJDialog::SWJ_MODE_OFFLINE);
        dlg->GetSwjOscData();

        glyout->addWidget(WDFunc::NewLBL(this, "Номер"), 0, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Дата, время"), 0, 1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "Аппарат"), 0, 2, 1, 2);
        glyout->addWidget(WDFunc::NewLBL(this, "Переключение"), 0, 4, 1, 2);
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.Num)), 1, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, TimeFunc::UnixTime64ToString(OscFunc->SWJRecord.Time)), 1, 1, 1, 1);
        QStringList tmpsl = QStringList() << "CB"
                                          << "G"
                                          << "D";
        if (OscFunc->SWJRecord.TypeA == 1)
            tmps = tmpsl.at(0); //: "N/A";
        else if (OscFunc->SWJRecord.TypeA == 2)
            tmps = tmpsl.at(1);
        else if (OscFunc->SWJRecord.TypeA == 4)
            tmps = tmpsl.at(2);
        else
            tmps = "N/A";
        glyout->addWidget(WDFunc::NewLBLT(this, tmps), 1, 2, 1, 1);
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.NumA)), 1, 3, 1, 1);
        tmps = (OscFunc->SWJRecord.Options & 0x00000001) ? "ВКЛЮЧЕНИЕ" : "ОТКЛЮЧЕНИЕ";
        glyout->addWidget(WDFunc::NewLBLT(this, tmps), 1, 4, 1, 2);
        glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутации:"), 3, 0, 1, 4);

        quint32 tmpi32 = (OscFunc->SWJRecord.Options >> 1) & 0x03;
        if (tmpi32)
        {
            if (tmpi32 == 2)
                tmps = "Несинхронная от АВМ-СК";
            else if (tmpi32 == 3)
                tmps = "Синхронная от АВМ-СК";
        }
        else
        {
            tmps = "Несинхронная от внешнего устройства";
        }
        glyout->addWidget(WDFunc::NewLBLT(this, tmps), 3, 4, 1, 1);

        glyout->addWidget(WDFunc::NewLBL(this, "Результат переключения:"), 4, 0, 1, 4);
        tmps = (OscFunc->SWJRecord.OpResult) ? "НЕУСПЕШНО" : "УСПЕШНО";
        glyout->addWidget(WDFunc::NewLBLT(this, tmps), 4, 4, 1, 1);

        glyout->addWidget(WDFunc::NewLBL(this, "Коммутируемые фазы:"), 5, 0, 1, 4);
        for (int i = 0; i < 4; i++)
        {
            if (((OscFunc->SWJRecord.Options >> 3) == i))
            {
                tmps = phase.at(i);
            }
        }
        glyout->addWidget(WDFunc::NewLBLT(this, tmps), 5, 4, 1, 1);

        glyout->addWidget(WDFunc::NewLBL(this, "Напряжение питания цепей соленоидов, В:"), 6, 0, 1, 4);

        if (OscFunc->SWJRecord.SupplyVoltage == std::numeric_limits<float>::max())
            glyout->addWidget(WDFunc::NewLBLT(this, "-"), 6, 4, 1, 1);
        else
            glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.SupplyVoltage)), 6, 4, 1, 1);

        glyout->addWidget(WDFunc::NewLBL(this, "Температура окружающей среды, Град:"), 7, 0, 1, 4);

        if (OscFunc->SWJRecord.Tokr == std::numeric_limits<float>::max())
            glyout->addWidget(WDFunc::NewLBLT(this, "-"), 7, 4, 1, 1);
        else
            glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.Tokr)), 7, 4, 1, 1);

        if (swjr.FileLength)
        {
            glyout->addWidget(WDFunc::NewLBL(this, "Осциллограмма:"), 8, 0, 1, 4);
            QPushButton *pb = new QPushButton("Открыть осциллограмму");
            // pb->setIcon(QIcon("images/osc.png"));
            connect(pb, &QPushButton::clicked, this, [dlg] {
                dlg->dlg->PlotShow();
                dlg->dlg->show();
            });
            glyout->addWidget(pb, 8, 4, 1, 1);
        }
        else
        {
            QPixmap *pm = new QPixmap("images/hr.png");
            glyout->addWidget(WDFunc::NewLBL(this, "", "", "", pm), 8, 4, 1, 1);
        }
        vlyout->addLayout(glyout);
        vlyout->addStretch(10);
        glyout = new QGridLayout;
        QStringList sl = QStringList() << "Действующее значение тока в момент коммутации, А"
                                       << "Действующее значение напряжения в момент коммутации, кВ"
                                       << "Собственное время коммутации, мс"
                                       << "Полное время коммутации, мс"
                                       << "Время перемещения главного контакта, мс"
                                       << "Время горения дуги, мс"
                                       << "Время безоперационного простоя к моменту коммутации, ч"
                                       << "Погрешность синхронной коммутации, мс"
                                       << "Температура внутри привода, Град"
                                       << "Давление в гидросистеме привода, Па";
        glyout->addWidget(WDFunc::NewLBL(this, "Измеренное значение"), 0, 0, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "A"), 0, 1, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "B"), 0, 2, 1, 1);
        glyout->addWidget(WDFunc::NewLBL(this, "C"), 0, 3, 1, 1);
        glyout->setColumnStretch(0, 10);
        int row = 1;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
            glyout->addWidget(
                WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.I[i], 'f', 1)), row, i + 1, 1, 1);
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
            glyout->addWidget(
                WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.U[i], 'f', 1)), row, i + 1, 1, 1);
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.OwnTime[i]);
            value = value / 100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.FullTime[i]);
            value = value / 100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.MovTime[i]);
            value = value / 100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.ArchTime[i]);
            value = value / 100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.IdleTime[i]);
            value = value / 100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.Inaccuracy[i]);
            value = value / 100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
        {
            value = OscFunc->SWJRecord.Tins[i];
            if (value == std::numeric_limits<float>::max())
                glyout->addWidget(WDFunc::NewLBLT(this, "-"), row, i + 1, 1, 1);
            else
                glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row - 1)), row, 0, 1, 1);
        for (int i = 0; i < 3; ++i)
        {
            value = OscFunc->SWJRecord.Phyd[i];
            if (value == std::numeric_limits<float>::max())
                glyout->addWidget(WDFunc::NewLBLT(this, "-"), row, i + 1, 1, 1);
            else
                glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)), row, i + 1, 1, 1);
        }
        vlyout->addLayout(glyout);
        // QPushButton *pb = new QPushButton("Сохранить журнал в файл");
        // connect(pb,SIGNAL(clicked(bool)),this,SLOT(SWJDialog::SaveSWJ()));
        // vlyout->addWidget(pb);
        // setLayout(vlyout);
        /*lyout = new QVBoxLayout;
        lyout->addWidget(TuneTW);
        setLayout(lyout);*/
        dlg->setLayout(vlyout);
        dlg->show();
    }
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
}

void Coma::prepare()
{

    auto const &board = Board::GetInstance();
    quint16 deviceType = board.type();
    QString deviceName = QVariant::fromValue(Modules::Model(deviceType)).toString();
    QMessageBox::information(this, "Связь установлена", "Установлена связь с " + deviceName, QMessageBox::Ok);
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

void Coma::setMode(int mode)
{
    Mode = mode;
}

void Coma::go(const QString &parameter)
{
    Q_UNUSED(parameter)
    if (Mode != COMA_GENERALMODE)
        StdFunc::SetEmulated(true);
    SetupUI();
    show();
}

void Coma::reconnect()
{
    qInfo(__PRETTY_FUNCTION__);
    if (!Reconnect)
        return;

    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
    {
        qDebug() << "call Disconnect";
        disconnect();
        clearWidgets();
        MainTW->hide();
        StdFunc::SetEmulated(false);
    }

    QMessageBox msgBox;
    msgBox.setText("Связь разорвана.\nПопытка переподключения");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.show();
    msgBox.button(QMessageBox::Ok)->animateClick(3000);
    attemptToRec();
}

void Coma::attemptToRec()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    SaveSettings();
    QApplication::restoreOverrideCursor();
    prepareConnectDlg();
}

void Coma::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"
        + QCoreApplication::applicationName() + "/";
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    StyleLoader::GetInstance().attach();
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());
}

void Coma::SaveSettings()
{
    auto sets = std::unique_ptr<QSettings>(new QSettings);
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

void Coma::getAbout()
{
    AboutWidget *w = new AboutWidget;
    w->show();
}

void Coma::disconnect()
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

void Coma::setupConnection()
{
    auto const &board = Board::GetInstance();

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
    WaitWidget *ww = new WaitWidget;
    auto connection = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    *connection = connect(&board, &Board::readyRead, this, [=]() {
        ww->Stop();
        QObject::disconnect(*connection);
        QApplication::restoreOverrideCursor();
        prepare();
    });

    ww->setObjectName("ww");
    WaitWidget::ww_struct wws = { true, false, WaitWidget::WW_TIME,
        15 }; // isallowedtostop = true, isIncrement = false, format: mm:ss, 30 minutes
    ww->Init(wws);
    ww->SetMessage("Пожалуйста, подождите");
    // QEventLoop loop;
    //  connect(ww, &WaitWidget::finished, &loop, &QEventLoop::quit);
    ww->Start();
    // loop.exec();
    if (!BaseInterface::iface()->start(ConnectSettings))
    {
        ww->Stop();
        QObject::disconnect(*connection);
        QMessageBox::critical(this, "Ошибка", "Не удалось установить связь", QMessageBox::Ok);
        QApplication::restoreOverrideCursor();
        qCritical() << "Cannot connect" << Error::GeneralError;

        return;
    }
    //    QTimer timer;
    //    timer.setSingleShot(true);
    //    timer.start(INTERVAL::WAIT);
    //    connect(&timer, &QTimer::timeout, this, [=] {
    //        if (Board::GetInstance().type() != 0)
    //            return;
    //        QObject::disconnect(*connection);
    //        QMessageBox::critical(this, "Ошибка", "Не удалось соединиться с прибором", QMessageBox::Ok);
    //        DisconnectAndClear();
    //        qCritical() << "Cannot connect" << Error::Timeout;
    //        QApplication::restoreOverrideCursor();
    //    });
    DataManager::clearQueue();
    BaseInterface::iface()->reqBSI();
}

void Coma::DisconnectAndClear()
{
    qInfo(__PRETTY_FUNCTION__);
    const auto &board = Board::GetInstance();
    if (board.connectionState() == Board::ConnectionState::Closed)
        return;

    AlarmW->clear();
    disconnect();
    m_Module->closeDialogs();

    clearWidgets();

    // BUG Segfault
    //    if (Reconnect)
    //        QMessageBox::information(this, "Разрыв связи", "Связь разорвана", QMessageBox::Ok, QMessageBox::Ok);
    //    else
    //        QMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");

    StdFunc::SetEmulated(false);

    Reconnect = false;
}

void Coma::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
}

void Coma::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        StdFunc::cancel();
    QMainWindow::keyPressEvent(e);
}

void Coma::mainTWTabChanged(int tabindex)
{
    // Q_ASSERT(m_Module != nullptr);
    m_Module->parentTWTabChanged(tabindex);
}

//}

void Coma::update(const DataTypes::GeneralResponseStruct &rsp)
{

    if (rsp.type == DataTypes::GeneralResponseTypes::DataCount)
        SetProgressBarCount(1, rsp.data);

    if (rsp.type == DataTypes::GeneralResponseTypes::DataSize)
        //        SetProgressBar1Size(rsp.data);
        SetProgressBarSize(1, rsp.data);
}

void Coma::closeEvent(QCloseEvent *event)
{
    DisconnectAndClear();
    event->accept();
}
