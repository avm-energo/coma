#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QDir>
#include <QMenu>
#include <QApplication>
#include <QTimer>
#include <QMenuBar>
#include <QEventLoop>
#include <QScrollBar>
#include <QProgressBar>
#include <QSettings>
#include <QCursor>
#include <QStringListModel>
#include <QStandardPaths>
#include <QPropertyAnimation>
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>
#include "mainwindow.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "commands.h"
#endif
#include "../widgets/wd_func.h"
#include "../widgets/etabwidget.h"
#include "../widgets/emessagebox.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/hiddendialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/keypressdialog.h"
#include "../gen/error.h"
#include "../gen/colors.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../widgets/etablemodel.h"
#include "../widgets/etableview.h"
#include "../dialogs/a1dialog.h"
#include "../dialogs/trendviewdialog.h"
#include "../gen/timefunc.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QPixmap StartWindowSplashPixmap("images/2.1.x.png");
    StartWindowSplashScreen = new QSplashScreen(StartWindowSplashPixmap);
    StartWindowSplashScreen->show();
    StartWindowSplashScreen->showMessage("Подготовка окружения...", Qt::AlignRight, Qt::white);
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(StdFunc::GetHomeDir());
    if (!dir.exists())
        dir.mkpath(".");
    StdFunc::Init();
    Error::Init();
    S2Config.clear();
    S2ConfigForTune.clear();
    MainConfDialog = nullptr;
    ConfB = ConfM = nullptr;
#ifndef MODULE_A1
    OscD = nullptr;
    SwjD = nullptr;
#endif
#if PROGSIZE >= PROGSIZE_LARGE
    PrepareTimers();
#endif
    LoadSettings();
#if PROGSIZE != PROGSIZE_EMUL
#ifdef USBENABLE
    cn = new EUsbHid;
    connect(cn,SIGNAL(Retry()),this,SLOT(ShowConnectDialog()));
#else
#ifdef COMPORTENABLE
    cn = new EUsbCom;
    connect(cn,SIGNAL(Retry()),this,SLOT(ShowConnectDialog()));
#endif
#endif
    connect(cn,SIGNAL(SetDataSize(int)),this,SLOT(SetProgressBar1Size(int)));
    connect(cn,SIGNAL(SetDataCount(int)),this,SLOT(SetProgressBar1(int)));
    connect(cn,SIGNAL(readbytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(cn,SIGNAL(writebytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(cn, SIGNAL(ShowError(QString)), this, SLOT(ShowErrorMessageBox(QString)));
    connect(this,SIGNAL(Retry()),this,SLOT(Stage1_5()));
#endif
#ifndef MODULE_A1
    OscFunc = new EOscillogram;
#endif
    StartWindowSplashScreen->finish(this);
}

MainWindow::~MainWindow()
{
#if PROGSIZE != PROGSIZE_EMUL
    Disconnect();
#endif
}

void MainWindow::SetMode(int mode)
{
    Mode = mode;
}

int MainWindow::GetMode()
{
    return Mode;
}

void MainWindow::Go(const QString &parameter)
{
    if (Mode != COMA_GENERALMODE)
    {
        StdFunc::SetEmulated(true);
        Autonomous = true;
    }
    SetupUI();
    show();
    switch (Mode)
    {
#ifndef MODULE_A1
    case COMA_AUTON_OSCMODE:
    {
        LoadOscFromFile(parameter);
        break;
    }
    case COMA_AUTON_SWJMODE:
    {
        LoadSwjFromFile(parameter);
        break;
    }
#endif
    case COMA_AUTON_PROTMODE:
    {
        StartA1Dialog(parameter);
        break;
    }
    default:
        break;
    }
}

QWidget *MainWindow::HthWidget()
{
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");
    QHBoxLayout *hlyout = new QHBoxLayout;
    for (int i = (MAXERRORFLAGNUM-1); i >= 0; i--)
        hlyout->addWidget(WDFunc::NewLBLT(w, Hth().at(i), "hth"+QString::number(i), \
                                          "QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);" \
                                          "background: 0px; margin: 0px; spacing: 0; padding: 0px;}", HthToolTip().at(i)));
    w->setLayout(hlyout);
    connect(this,SIGNAL(BsiRefresh()), this, SLOT(UpdateHthWidget()));
    return w;
}

QWidget *MainWindow::Least()
{
    QWidget *w = new QWidget;
//    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *inlyout = new QHBoxLayout;
    ETabWidget *MainTW = new ETabWidget;
    MainTW->setObjectName("maintw");
    MainTW->setTabPosition(QTabWidget::West);
//    MainTW->tabBar()->setStyleSheet("QTabBar::tab {background-color: yellow;}");
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
    inlyout->addWidget(WDFunc::NewLBLT(this, "Обмен"));
    inlyout->addWidget(WDFunc::NewLBLT(this, "", "prb1lbl"));
    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prb1prb");
    prb->setOrientation(Qt::Horizontal);
    prb->setMinimumWidth(500);
    prb->setMaximumHeight(10);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBLT(this, "Отсчёт"));
    inlyout->addWidget(WDFunc::NewLBLT(this, "", "prb2lbl"));
    prb = new QProgressBar;
    prb->setObjectName("prb2prb");
    prb->setOrientation(Qt::Horizontal);
    prb->setMinimumWidth(500);
    prb->setMaximumHeight(10);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    w->setLayout(lyout);
    return w;
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::SetSlideWidget()
{
    QWidget *SlideWidget = new QWidget(this);
    SlideWidget->setWindowFlags(Qt::FramelessWindowHint);
    SlideWidget->setObjectName("slidew");
    SlideWidget->setStyleSheet("QWidget {background-color: rgba(110,234,145,255);}");
    QVBoxLayout *slyout = new QVBoxLayout;
    QCheckBox *chb = WDFunc::NewChB(this, "teenablechb", "Включить протокол");
    connect(chb,SIGNAL(toggled(bool)),this,SLOT(SetTEEnabled(bool)));
    slyout->addWidget(chb, 0, Qt::AlignLeft);
    QTextEdit *MainTE = new QTextEdit;
    MainTE->setObjectName("mainte");
    MainTE->setReadOnly(true);
    MainTE->setUndoRedoEnabled(false);
    MainTE->setWordWrapMode(QTextOption::WrapAnywhere);
    MainTE->document()->setMaximumBlockCount(C_TE_MAXSIZE);
    slyout->addWidget(MainTE, 40);
    SlideWidget->setLayout(slyout);
    SlideWidget->setMinimumWidth(250);
    SlideWidget->hide();
    SWGeometry = SlideWidget->geometry();
    SWHide = true;
}
#endif

void MainWindow::SetupMenubar()
{
    QMenuBar *menubar = new QMenuBar;
    QString tmps = "QMenuBar {background-color: "+QString(MAINWINCLRA1)+";}"\
            "QMenuBar::item {background-color: "+QString(MAINWINCLRA1)+";}";
    menubar->setStyleSheet(tmps);
    QMenu *menu = new QMenu;
    menu->setTitle("Главное");
    QAction *act = new QAction(this);
    act->setText("Выход");
    connect(act,SIGNAL(triggered()),this,SLOT(close()));
    menu->addAction(act);
    if (!Autonomous)
    {
        act = new QAction(this);
        act->setText("Соединение");
        act->setIcon(QIcon("images/play.png"));
        connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
        menu->addAction(act);
        act = new QAction(this);
        act->setText("Разрыв соединения");
        act->setIcon(QIcon("images/stop.png"));
        connect(act,SIGNAL(triggered()),this,SLOT(DisconnectAndClear()));
        menu->addAction(act);
    }
    menubar->addMenu(menu);
#if PROGSIZE >= PROGSIZE_LARGE
    menu = new QMenu;
    menu->setTitle("Секретные операции");
    act = new QAction(this);
    act->setText("Работа с Hidden Block");
    connect(act,SIGNAL(triggered()),this,SLOT(OpenBhbDialog()));
    menu->addAction(act);
    menubar->addMenu(menu);
#endif

    menu = new QMenu;
    menu->setTitle("Настройки");
    act = new QAction(this);
    act->setText("Настройки");
    act->setIcon(QIcon("images/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    menu->addAction(act);
    menubar->addMenu(menu);

    act = new QAction(this);
    act->setText("О программе");
    connect(act,SIGNAL(triggered()),this,SLOT(GetAbout()));
    menubar->addAction(act);

    menubar->addSeparator();
#ifndef MODULE_A1
    menu = new QMenu;
    menu->setObjectName("autonomousmenu");
    menu->setTitle("Автономная работа");
    act = new QAction(this);
    act->setText("Загрузка осциллограммы");
    connect(act,SIGNAL(triggered()),this,SLOT(LoadOsc()));
    menu->addAction(act);
    act = new QAction(this);
    act->setText("Загрузка журнала переключений");
    connect(act,SIGNAL(triggered()),this,SLOT(LoadSWJ()));
    menu->addAction(act);
    menubar->addMenu(menu);
#endif
    setMenuBar(menubar);
    AddActionsToMenuBar(menubar);
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::PrepareTimers()
{
    QTimer *MouseTimer = new QTimer;
    connect(MouseTimer,SIGNAL(timeout()),this,SLOT(MouseMove()));
    MouseTimer->start(50);
}
#endif

void MainWindow::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"+PROGNAME+"/";
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());
#if PROGSIZE != PROGSIZE_EMUL
    EAbstractProtocomChannel::SetWriteUSBLog(sets->value("WriteLog", "0").toBool());
#endif
    TEEnabled = sets->value("TEEnabled", "0").toBool();
}

void MainWindow::SaveSettings()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    sets->setValue("Homedir", StdFunc::GetHomeDir());
#if PROGSIZE != PROGSIZE_EMUL
    sets->setValue("WriteLog", EAbstractProtocomChannel::IsWriteUSBLog());
#endif
    sets->setValue("TEEnabled", TEEnabled);
}

void MainWindow::ClearTW()
{
    S2Config.clear();
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == nullptr)
        return;
    while (MainTW->count())
    {
        QWidget *wdgt = MainTW->widget(0);
        MainTW->removeTab(0);
        wdgt->deleteLater();
    }
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != nullptr)
        MainTE->clear();
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::ShowOrHideSlideSW()
{
    QWidget *w = this->findChild<QWidget *>("slidew");
    if (w == nullptr)
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
#endif
#if PROGSIZE != PROGSIZE_EMUL
int MainWindow::CheckPassword()
{
    ok = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(PasswordCheck(QString)));
    connect(this,SIGNAL(PasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    if (!ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}
#endif
#ifndef MODULE_A1
void MainWindow::LoadOscFromFile(const QString &filename)
{
    quint32 len = 0;

    if (Files::LoadFromFile(filename, OscFunc->BA) == Files::ER_NOERROR)
    {
        TrendViewDialog *dlg = new TrendViewDialog(OscFunc->BA);
        TrendViewModel *mdl = new TrendViewModel(QStringList(), QStringList(), len);
        OscFunc->ProcessOsc(mdl);
        mdl->xmax = (static_cast<float>(mdl->Len/2));
        mdl->xmin = -mdl->xmax;
        dlg->TrendModel = mdl;

        switch(mdl->idOsc)
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
            mdl->tmpav_21 << QString::number(mdl->idOsc); // пока сделано для одного канала в осциллограмме
            //TrendViewModel *TModel = new TrendViewModel(QStringList(), tmpav, *len);
            //dlg->SetModel(TModel);
            dlg->SetAnalogNames(mdl->tmpav_21);
            dlg->SetRanges(0, 10000, -20, 20); // 10000 мс, 20 мА (сделать автонастройку в зависимости от конфигурации по данному каналу)

           break;
         }

         case ID_OSC_CH0:
         case ID_OSC_CH0+1:
         case ID_OSC_CH0+2:
         case ID_OSC_CH0+3:
         case ID_OSC_CH0+4:
         case ID_OSC_CH0+5:
         case ID_OSC_CH0+6:
         case ID_OSC_CH0+7:
         {

           dlg->SetAnalogNames(mdl->tmpav_85);
           dlg->SetDigitalNames(mdl->tmpdv_85);
           dlg->SetDigitalColors(mdl->dcolors_85);
           dlg->SetAnalogColors(mdl->acolors_85);
           dlg->SetRanges(mdl->xmin, mdl->xmax, -200, 200);
           break;
         }


        }

            dlg->SetupPlots();
            dlg->SetupUI();
            dlg->setModal(false);
    }
}

void MainWindow::LoadSwjFromFile(const QString &filename)
{
    QByteArray ba;
    //bool haveosc;
    int SWJRSize = sizeof(SWJDialog::SWJournalRecordStruct);
    //int GBOSize = sizeof(EOscillogram::GBoStruct);
    float value;
    QString str;


    if (Files::LoadFromFile(filename, OscFunc->BA) == Files::ER_NOERROR)
    {
        QStringList phase = {"фазы А, В, С","фаза А","фаза В","фаза С"};

        if (OscFunc->BA.size() < (SWJRSize))
        {
            EMessageBox::error(this, "Ошибка", "Некорректная структура файла журнала");
            return;
        }
        SWJDialog::SWJournalRecordStruct SWJ;
        size_t tmpi = static_cast<size_t>(SWJRSize);
        memcpy(&SWJ, &(OscFunc->BA.data()[0]), tmpi); // копируем информацию о переключении
        SWJDialog::SWJINFStruct swjr;
        swjr.FileLength = OscFunc->BA.size();
        //EOscillogram::GBoStruct gbos;
        //tmpi = static_cast<size_t>(GBOSize);
        //memcpy(&gbos, &(ba.data()[SWJRSize]), tmpi); // копируем информацию об осциллограмме
       /* ba.remove(0, SWJRSize); // оставляем только саму осциллограмму
        if (ba.isEmpty()) // осциллограммы в журнале нет
            haveosc = false;
        else
            haveosc = true;*/
        //dlg->Init(swjr, haveosc, gbos);

        //if (!ba.isEmpty())
        //   dlg->LoadOsc(ba);

        QVBoxLayout *vlyout = new QVBoxLayout;
        QGridLayout *glyout = new QGridLayout;

        dlg = new SWJDialog(OscFunc, SWJDialog::SWJ_MODE_OFFLINE);
        dlg->GetSwjOscData();

        glyout->addWidget(WDFunc::NewLBL(this, "Номер"), 0,0,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "Дата, время"),0,1,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "Аппарат"),0,2,1,2);
        glyout->addWidget(WDFunc::NewLBL(this, "Переключение"),0,4,1,2);
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.Num)), 1,0,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, TimeFunc::UnixTime64ToString(OscFunc->SWJRecord.Time)),1,1,1,1);
        QStringList tmpsl = QStringList() << "D" << "G" << "CB";
        QString tmps = (OscFunc->SWJRecord.TypeA < tmpsl.size()) ? tmpsl.at(OscFunc->SWJRecord.TypeA) : "N/A";
        glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,2,1,1);
        glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.NumA)),1,3,1,1);
        tmps = (OscFunc->SWJRecord.Options & 0x00000001) ? "ВКЛЮЧЕНИЕ" : "ОТКЛЮЧЕНИЕ";
        glyout->addWidget(WDFunc::NewLBLT(this, tmps),1,4,1,2);
        glyout->addWidget(WDFunc::NewLBL(this, "Тип коммутации:"),3,0,1,4);

        quint32 tmpi32 = (OscFunc->SWJRecord.Options >> 1) & 0x03;
        if (tmpi32)
        {
            if (tmpi32 == 2)
            tmps = "Несинхронная от АВ-ТУК";
            else if (tmpi32 == 3)
            tmps = "Синхронная от АВ-ТУК";

        }
        else
        {
            tmps = "Несинхронная от внешнего устройства";
        }
        glyout->addWidget(WDFunc::NewLBLT(this, tmps),3,4,1,1);

        glyout->addWidget(WDFunc::NewLBL(this, "Результат переключения:"),4,0,1,4);
        tmps = (OscFunc->SWJRecord.OpResult)  ? "НЕУСПЕШНО" : "УСПЕШНО";
        glyout->addWidget(WDFunc::NewLBLT(this, tmps),4,4,1,1);

        glyout->addWidget(WDFunc::NewLBL(this, "Коммутируемые фазы:"),5,0,1,4);
        for(int i = 0; i < 4; i++)
        {
            if(((OscFunc->SWJRecord.Options >> 3) == i))
            {
               tmps = phase.at(i);
            }
        }
        glyout->addWidget(WDFunc::NewLBLT(this, tmps),5,4,1,1);
        if (swjr.FileLength)
        {
            glyout->addWidget(WDFunc::NewLBL(this, "Осциллограмма:"),6,0,1,4);
            QPushButton *pb = new QPushButton("Открыть осциллограмму");
            //pb->setIcon(QIcon("images/osc.png"));
            connect(pb,SIGNAL(clicked()),this,SLOT(ShowOsc()));
            glyout->addWidget(pb,6,4,1,1);
        }
        else
        {
            QPixmap *pm = new QPixmap("images/hr.png");
            glyout->addWidget(WDFunc::NewLBL(this, "", "", "", pm),6,4,1,1);
        }
        vlyout->addLayout(glyout);
        vlyout->addStretch(10);
        glyout = new QGridLayout;
        QStringList sl = QStringList() << "Мгновенное значение тока в момент коммутации, А" << \
                                          "Мгновенное значение напряжения в момент коммутации, кВ" << \
                                          "Собственное время коммутации, мс" << "Полное время коммутации, мс" << \
                                          "Время перемещения главного контакта, мс" << "Время горения дуги, мс" << \
                                          "Время безоперационного простоя к моменту коммутации, ч"; /*<< \
                                          "Погрешность синхронной коммутации, мс";*/
        glyout->addWidget(WDFunc::NewLBL(this, "Измеренное значение"),0,0,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "A"),0,1,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "B"),0,2,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "C"),0,3,1,1);
        glyout->setColumnStretch(0, 10);
        int row = 1;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
        for (int i=0; i<3; ++i)
            glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.I[i], 'f', 1)),row,i+1,1,1);
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
        for (int i=0; i<3; ++i)
            glyout->addWidget(WDFunc::NewLBLT(this, QString::number(OscFunc->SWJRecord.U[i], 'f', 1)),row,i+1,1,1);
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
        for (int i=0; i<3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.OwnTime[i]);
            value = value/100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)),row,i+1,1,1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
        for (int i=0; i<3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.FullTime[i]);
            value = value/100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)),row,i+1,1,1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
        for (int i=0; i<3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.MovTime[i]);
            value = value/100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)),row,i+1,1,1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
        for (int i=0; i<3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.ArchTime[i]);
            value = value/100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)),row,i+1,1,1);
        }
        ++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
        for (int i=0; i<3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.IdleTime[i]);
            value = value/100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)),row,i+1,1,1);
        }
        /*++row;
        glyout->addWidget(WDFunc::NewLBL(this, sl.at(row-1)),row,0,1,1);
        for (int i=0; i<3; ++i)
        {
            value = static_cast<float>(OscFunc->SWJRecord.Inaccuracy[i]);
            value = value/100;
            glyout->addWidget(WDFunc::NewLBLT(this, str.setNum(value, 'f', 2)),row,i+1,1,1);
        }*/
        vlyout->addLayout(glyout);
        //QPushButton *pb = new QPushButton("Сохранить журнал в файл");
        //connect(pb,SIGNAL(clicked(bool)),this,SLOT(SWJDialog::SaveSWJ()));
        //vlyout->addWidget(pb);
        //setLayout(vlyout);
        /*lyout = new QVBoxLayout;
        lyout->addWidget(TuneTW);
        setLayout(lyout);*/
        dlg->setLayout(vlyout);
        dlg->show();
    }
}
#endif

#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::Stage1_5()
{
#ifdef USBENABLE
    ShowConnectDialog();
#endif
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (Commands::Connect() != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Не удалось установить связь");
        QApplication::restoreOverrideCursor();
        return;
    }
    S2ConfigForTune.clear();
    S2Config.clear();
    SaveSettings();
    QApplication::restoreOverrideCursor();
    Stage2();
}

void MainWindow::Stage2()
{
    int res = ModuleBSI::SetupBSI();
    if (res == Error::ER_CANAL)
    {
        if (EMessageBox::question(this, "Ошибка", \
                                  "Блок Bsi не может быть прочитан, ошибка " + QString::number(res) + ", повторить?") == 1) // Yes
        {
            cn->Disconnect();
#ifdef COMPORTENABLE
            ShowConnectDialog();
#endif
            emit Retry();
        }
        return;
    }
#if PROGSIZE >= PROGSIZE_LARGE
    else if (res == Error::ER_RESEMPTY)
    {
        if (OpenBhbDialog() != Error::ER_NOERROR)
        {
            EMessageBox::error(this, "Ошибка", "Ошибка при работе с Hidden block");
            return;
        }
    }
#endif
    Stage3();
}

void MainWindow::UpdateHthWidget()
{
    ModuleBSI::Bsi bsi = ModuleBSI::GetBsi();
    for (int i = 0; i < MAXERRORFLAGNUM; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("hth"+QString::number(i));
        if (lbl == nullptr)
            return;
        quint32 tmpui = (0x00000001 << i) & bsi.Hth;
        if (tmpui)
            lbl->setStyleSheet("QLabel {background-color: rgba(255,10,10,255); color: rgba(255,255,255,255);}");
        else
            lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");
    }
}
#endif

void MainWindow::SetMainDefConf()
{
    if (MainConfDialog != nullptr)
        MainConfDialog->SetDefConf();
}

void MainWindow::SetBDefConf()
{
    if (ConfB != nullptr)
        ConfB->SetDefConf();
}

void MainWindow::SetMDefConf()
{
    if (ConfM != nullptr)
        ConfM->SetDefConf();
}

void MainWindow::Fill()
{
    if (MainConfDialog != nullptr)
        MainConfDialog->Fill();
    if (ConfB != nullptr)
        ConfB->Fill();
    if (ConfM != nullptr)
        ConfM->Fill();
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::UpdateMainTE(QByteArray ba)
{
    if (!TEEnabled)
        return;
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != nullptr)
        MainTE->append(ba.toHex());
}

void MainWindow::SetTEEnabled(bool enabled)
{
    TEEnabled = enabled;
}
#endif

void MainWindow::PasswordCheck(QString psw)
{
    if (psw == "se/7520a")
        ok = true;
    else
        ok = false;
    emit PasswordChecked();
}

#if PROGSIZE >= PROGSIZE_LARGE
int MainWindow::OpenBhbDialog()
{
    if (!Commands::isConnected())
    {
        QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модулем" : "прибором");
        EMessageBox::information(this, "Подтверждение", "Для работы данной функции необходимо сначала установить связь с "+tmps);
        return Error::ER_GENERALERROR;
    }
    if (CheckPassword() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;

    HiddenDialog *dlg = new HiddenDialog();
    dlg->Fill(); // заполняем диалог из недавно присвоенных значений
    dlg->exec();
    if (!dlg->ResultOk)
        return Error::ER_GENERALERROR;
    Disconnect();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (Commands::Connect() != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Не удалось установить связь");
        QApplication::restoreOverrideCursor();
        return Error::ER_GENERALERROR;
    }
    QApplication::restoreOverrideCursor();
    int res;
    if ((res = ModuleBSI::SetupBSI()) == Error::ER_CANAL)
    {
        EMessageBox::error(this, "Ошибка", "Блок Bsi не может быть прочитан, ошибка " + QString::number(res));
        Commands::Disconnect();
        return Error::ER_GENERALERROR;
    }
    emit BsiRefresh();
    return Error::ER_NOERROR;
}
#endif

#if PROGSIZE >= PROGSIZE_LARGE || PROGSIZE == PROGSIZE_EMUL
void MainWindow::StartEmul()
{
    bool ok;
    quint16 MType = sender()->objectName().toUShort(&ok, 16);
    if (!ok)
    {
        ERMSG("Wrong object name in StartEmul()");
        return;
    }
    ModuleBSI::Bsi bsi;
    bsi.MTypeB = (MType & 0xFF00) >> 8;
    bsi.MTypeM = MType & 0x00FF;
#if PROGSIZE != PROGSIZE_EMUL
    if (cn->Connected)
        DisconnectAndClear();
#endif
    bsi.SerialNum = 0x12345678;
    bsi.Hth = 0x00;
    ModuleBSI::SetupEmulatedBsi(bsi);
    StdFunc::SetEmulated(true);
    Stage3();
}
#endif

void MainWindow::StartSettingsDialog()
{
    SettingsDialog *dlg = new SettingsDialog;
    dlg->exec();
    SaveSettings();
}

void MainWindow::ShowErrorDialog()
{
    ErrorDialog *dlg = new ErrorDialog;
    dlg->exec();
}
#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::SetProgressBar1Size(int size)
{
    SetProgressBarSize("1", size);
}

void MainWindow::SetProgressBar1(int cursize)
{
    SetProgressBar("1", cursize);
}

void MainWindow::SetProgressBar2Size(int size)
{
    SetProgressBarSize("2", size);
}

void MainWindow::SetProgressBar2(int cursize)
{
    SetProgressBar("2", cursize);
}

void MainWindow::ShowConnectDialog()
{
    QDialog *dlg = new QDialog(this);
    QStringList sl = cn->DevicesFound();
    QStringListModel *tmpmodel = new QStringListModel;
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    if (sl.size() == 0)
    {
        lyout->addWidget(WDFunc::NewLBL(this, "Ошибка, устройства не найдены"));
        Error::ShowErMsg(CN_NOPORTSERROR);
    }
    tmpmodel->setStringList(sl);
    QComboBox *portscb = new QComboBox;
    connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetPortSlot(QString)));
    portscb->setModel(tmpmodel);
    lyout->addWidget(portscb);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)),cn,SLOT(SetCancelled()));
    connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
}

void MainWindow::SetProgressBarSize(QString prbnum, int size)
{
    QString prbname = "prb"+prbnum+"prb";
    QString lblname = "prb"+prbnum+"lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == nullptr)
    {
        DBGMSG;
        return;
    }
    WDFunc::SetLBLText(this, lblname,StdFunc::PrbMessage() + QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(size);
}

void MainWindow::SetProgressBar(QString prbnum, int cursize)
{
    QString prbname = "prb"+prbnum+"prb";
    QString lblname = "prb"+prbnum+"lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb != nullptr)
    {
        prb->setValue(cursize);
        WDFunc::SetLBLText(this, lblname, StdFunc::PrbMessage() + QString::number(cursize) + " из " + QString::number(prb->maximum()));
    }
}
#endif

void MainWindow::GetAbout()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *l2yout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    l2yout->addWidget(WDFunc::NewLBL(this, tmps));
    l2yout->addWidget(WDFunc::NewLBL(this, "ООО \"АВМ-Энерго\""));
    l2yout->addWidget(WDFunc::NewLBL(this, "2015-2019 гг."));
    l2yout->addWidget(WDFunc::NewLBL(this, "info@avmenergo.ru"));
    l2yout->addStretch(10);
    hlyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/evel.png")), 1, Qt::AlignVCenter);
    hlyout->addLayout(l2yout,100);
    lyout->addLayout(hlyout,1);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->exec();
}
#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::Disconnect()
{
    if (!StdFunc::IsInEmulateMode())
        cn->Disconnect();
}

void MainWindow::GetDeviceFromTable(QModelIndex idx)
{
    Q_UNUSED(idx);
    ETableView *tv = this->findChild<ETableView *>("devicetv");
    if (tv == nullptr)
    {
        DBGMSG;
        return;
    }
    QString tmps = tv->model()->index(tv->currentIndex().row(), 0, QModelIndex()).data(Qt::DisplayRole).toString();
    DevInfo.vendor_id = tmps.toUShort(nullptr, 16);
//    quint16 vid = tmps.toInt(nullptr, 16);
    tmps = tv->model()->index(tv->currentIndex().row(), 1, QModelIndex()).data(Qt::DisplayRole).toString();
    DevInfo.product_id = tmps.toUShort(nullptr, 16);
//    quint16 pid = tmps.toInt(nullptr, 16);
    tmps = tv->model()->index(tv->currentIndex().row(), 3, QModelIndex()).data(Qt::DisplayRole).toString();
    tmps.toWCharArray(DevInfo.serial);
}
#endif
void MainWindow::DisconnectAndClear()
{
#if PROGSIZE != PROGSIZE_EMUL
    Disconnect();
    TuneB = TuneM = nullptr;
#endif
#ifndef MODULE_A1
    OscD = nullptr;
#endif
    CheckB = CheckM = nullptr;
    emit FinishAll();
    emit ClearBsi();
    ClearTW();
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == nullptr)
        return;
    MainTW->hide();
    StdFunc::SetEmulated(false);
}
#ifndef MODULE_A1
void MainWindow::LoadOsc()
{
    QString filename = Files::ChooseFileForOpen(this, "Oscillogram files (*.osc)");
    LoadOscFromFile(filename);
}

void MainWindow::LoadSWJ()
{
    LoadSwjFromFile(Files::ChooseFileForOpen(this, "Switch journal files (*.swj)"));
}
#endif
#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::MouseMove()
{
    QPoint curPos = mapFromGlobal(QCursor::pos());
    QWidget *sww = this->findChild<QWidget *>("slidew");
    if (sww == nullptr)
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
#endif

void MainWindow::ShowErrorMessageBox(QString message)
{
    EMessageBox::error(this, "Ошибка", message);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (!SWHide)
    {
        QWidget *sww = this->findChild<QWidget *>("slidew");
        if (sww == nullptr)
            return;
        sww->setGeometry(QRect(width()-sww->width(), 0, sww->width(), height()));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        StdFunc::Cancel();
    QMainWindow::keyPressEvent(e);
}

void MainWindow::SetPortSlot(QString port)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->TranslateDeviceAndSave(port);
#endif
}

void MainWindow::StartA1Dialog(const QString &filename)
{
    A1Dialog *adlg = new A1Dialog(filename);
    delete adlg;
}

void MainWindow::SetDefConf()
{
    SetMainDefConf();
    SetBDefConf();
    SetMDefConf();
    Fill();
    EMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");
}

void MainWindow::ProtocolFromFile()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getOpenFileName(this, "Открыть файл", StdFunc::GetHomeDir(), \
                                            "PKDN verification files (*.vrf)", Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    dlg->close();
    StartA1Dialog(filename);
}
void MainWindow::ShowOsc()
{
    dlg->dlg->PlotShow();
    dlg->dlg->show();
}

