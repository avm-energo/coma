#include <QtTest/QTest>
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
#include <QThread>
#include <QInputDialog>
#include <QRegExp>
#include <QSettings>
#include <QDialog>
#include <QSizePolicy>
#include <QCheckBox>
#include "coma.h"
#include "widgets/mytabwidget.h"
#include "widgets/errorprotocolwidget.h"
#include "widgets/waitwidget.h"
#include "widgets/messagebox.h"
#include "widgets/s_tqspinbox.h"

Coma::Coma(QWidget *parent)
    : QMainWindow(parent)
{
    ERTimer = new QTimer;
    ERTimer->setInterval(pc.ErrWindowDelay*1000);
    connect(ERTimer,SIGNAL(timeout()),this,SLOT(HideErrorProtocol()));
    ERTimerIsOn = false;
    QTimer *MouseTimer = new QTimer;
    connect(MouseTimer,SIGNAL(timeout()),this,SLOT(MouseMove()));
    MouseTimer->start(50);
    QTimer *ErrorProtocolUpdateTimer = new QTimer;
    ErrorProtocolUpdateTimer->setInterval(1000);
    connect(ErrorProtocolUpdateTimer,SIGNAL(timeout()),this,SLOT(UpdateErrorProtocol()));
    ErrorProtocolUpdateTimer->start();
    ReconTry = 0;
    setWindowTitle(PROGNAME);
    QString tmps = "QMainWindow {background-color: "+QString(MAINWINCLR)+";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(800, 600));
    DialogsAreReadyAlready = false;
    QWidget *wdgt = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    setMinimumHeight(650);

    QHBoxLayout *hlyout = new QHBoxLayout;
    QToolBar *tb = new QToolBar;
    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(20,20));
    QAction *ConnectAction = new QAction(this);
    ConnectAction->setToolTip("Соединение");
    ConnectAction->setIcon(QIcon(":/play.png"));
    connect(ConnectAction,SIGNAL(triggered()),this,SLOT(Next()));
    QAction *DisconnectAction = new QAction(this);
    DisconnectAction->setToolTip("Разрыв соединения");
    DisconnectAction->setIcon(QIcon(":/stop.png"));
    connect(DisconnectAction,SIGNAL(triggered()),this,SLOT(Disconnect()));
    QAction *EmulAAction = new QAction(this);
    EmulAAction->setToolTip("Запуск в режиме А");
    EmulAAction->setIcon(QIcon(":/a.png"));
    connect(EmulAAction,SIGNAL(triggered()),this,SLOT(EmulA()));
    QAction *EmulEAction = new QAction(this);
    EmulEAction->setToolTip("Запуск в режиме Э");
    EmulEAction->setIcon(QIcon(":/e.png"));
    connect(EmulEAction,SIGNAL(triggered()),this,SLOT(EmulE()));
    QAction *SettingsAction = new QAction(this);
    SettingsAction->setToolTip("Настройки");
    SettingsAction->setIcon(QIcon(":/settings.png"));
    connect(SettingsAction,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    tb->addAction(ConnectAction);
    tb->addAction(DisconnectAction);
    tb->addSeparator();
    tb->addAction(EmulAAction);
    tb->addAction(EmulEAction);
    tb->addSeparator();
    tb->addAction(SettingsAction);
    hlyout->addWidget(tb);

    for (int i = 31; i >= 0; i--)
    {
        QLabel *lbl = new QLabel(Hth().at(i));
        lbl->setObjectName("hth"+QString::number(i));
        lbl->setToolTip(HthToolTip().at(i));
        lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);" \
                           "background: 0px; margin: 0px; spacing: 0; padding: 0px;}");
        hlyout->addWidget(lbl);
    }
    lyout->addLayout(hlyout);

    QMenuBar *MainMenuBar = new QMenuBar;
    tmps = "QMenuBar {background-color: "+QString(MAINWINCLR)+";}"\
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
    act->setIcon(QIcon(":/play.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Connect()));
    MainMenu->addAction(act);
    act = new QAction(this);
    act->setText("Разрыв соединения");
    act->setIcon(QIcon(":/stop.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Disconnect()));
    MainMenu->addAction(act);
    MainMenuBar->addMenu(MainMenu);

    QMenu *menu = new QMenu;
    menu->setTitle("Запуск...");
    EmulAAction = new QAction(this);
    EmulAAction->setText("...в режиме А");
    EmulAAction->setIcon(QIcon(":/a.png"));
    connect(EmulAAction,SIGNAL(triggered()),this,SLOT(EmulA()));
    menu->addAction(EmulAAction);
    EmulEAction = new QAction(this);
    EmulEAction->setIcon(QIcon(":/e.png"));
    EmulEAction->setText("...в режиме Э");
    connect(EmulEAction,SIGNAL(triggered()),this,SLOT(EmulE()));
    menu->addAction(EmulEAction);
    MainMenuBar->addMenu(menu);

    menu = new QMenu;
    menu->setTitle("Разработка");
    WriteSNAction = new QAction(this);
    WriteSNAction->setText("Запись серийного номера");
    WriteSNAction->setEnabled(false);
    connect(WriteSNAction,SIGNAL(triggered()),this,SLOT(WriteSN()));
    menu->addAction(WriteSNAction);
    WriteHWAction = new QAction(this);
    WriteHWAction->setText("Запись аппаратной версии модуля");
    WriteHWAction->setEnabled(false);
    connect(WriteHWAction,SIGNAL(triggered()),this,SLOT(WriteHW()));
    menu->addAction(WriteHWAction);
    MainMenuBar->addMenu(menu);

    menu = new QMenu;
    menu->setTitle("Настройки");
    act = new QAction(this);
    act->setText("Настройки");
    act->setIcon(QIcon(":/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    menu->addAction(act);
    act = new QAction(this);
    act->setText("Установка параметров связи с МИП");
    act->setStatusTip("Настройка связи с прибором контроля электр. параметров МИП для регулировки модулей Э");
    connect(act,SIGNAL(triggered()),this,SLOT(SetMipDlg()));
    menu->addAction(act);
    MainMenuBar->addMenu(menu);

    act = new QAction(this);
    act->setText("О программе");
    connect(act,SIGNAL(triggered()),this,SLOT(GetAbout()));
    MainMenuBar->addAction(act);
    setMenuBar(MainMenuBar);

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

    AddLabelAndLineedit(slyout, "Модуль АВТУК-", "mtypele");
    AddLabelAndLineedit(slyout, "Аппаратная версия:", "hwverle");
    AddLabelAndLineedit(slyout, "Версия ПО:", "fwverle");
    AddLabelAndLineedit(slyout, "КС конфигурации:", "cfcrcle");
    AddLabelAndLineedit(slyout, "Сброс:", "rstle");
    AddLabelAndLineedit(slyout, "Количество сбросов:", "rstcountle");
    AddLabelAndLineedit(slyout, "Серийный номер:", "snle");
    AddLabelAndLineedit(slyout, "ИД процессора:", "cpuidle");
    QTextEdit *MainTE = new QTextEdit;
    MainTE->setObjectName("mainte");
    slyout->addWidget(MainTE, 40);

    SlideWidget->setLayout(slyout);
    SlideWidget->setMinimumWidth(250);
    SlideWidget->hide();
    SWGeometry = SlideWidget->geometry();
    SWHide = true;

    ErrorProtocolWidget *ErrorWidget = new ErrorProtocolWidget(this);
    ErrorWidget->setObjectName("errorwidget");
    QString ErrWss = "QWidget {background-color: "+QString(ERPROTCLR)+";}";
    ErrorWidget->setStyleSheet(ErrWss);
    ErrorWidget->setAutoFillBackground(true);
    ErrorWidget->setMinimumHeight(150);
    ErrorWidget->hide();
    ERGeometry = ErrorWidget->geometry();
    ERHide = true;
}

Coma::~Coma()
{
    QSettings *sets = new QSettings ("EvelSoft","COMA");
    sets->setValue("mip/asdu",pc.MIPASDU);
    sets->setValue("mip/ip",pc.MIPIP);
    sets->setValue("Port", pc.Port);
    sets->setValue("erpath",pc.ermsgpath);
}

void Coma::AddLabelAndLineedit(QVBoxLayout *lyout, QString caption, QString lename)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel(caption);
    hlyout->addWidget(lbl);
    QLineEdit *le = new QLineEdit("");
    le->setObjectName(lename);
    le->setEnabled(false);
    hlyout->addWidget(le);
    lyout->addLayout(hlyout);
}

void Coma::Connect()
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
        ShowErrMsg(2);
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
    connect(nextL,SIGNAL(clicked()),this,SLOT(Next()));
    connect(nextL, SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(nextL);
    dlg->setLayout(lyout);
    connect(cn,SIGNAL(portopened()),dlg,SLOT(close()));
    dlg->exec();
}

void Coma::Next()
{
    cn = new canal;
    QThread *CanalThread = new QThread(this);
    cn->moveToThread(CanalThread);
    connect(CanalThread, &QThread::finished, cn, &canal::deleteLater);
    connect(CanalThread, &QThread::finished, CanalThread, &QThread::deleteLater);

    pc.PrbMessage = "Загрузка данных...";
    connect(cn,SIGNAL(OscEraseSize(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    connect(cn,SIGNAL(OscEraseRemaining(quint32)),this,SLOT(SetProgressBar(quint32)));
    connect(cn,SIGNAL(incomingdatalength(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    connect(cn,SIGNAL(bytesreceived(quint32)),this,SLOT(SetProgressBar(quint32)));
//    connect(cn,SIGNAL(SendEnd()),this,SLOT(DisableProgressBar()));
    connect(cn,SIGNAL(gotsomedata(QByteArray *)),this,SLOT(ReadUpdateMainTE(QByteArray *)));
    connect(cn,SIGNAL(somedatawritten(QByteArray *)),this,SLOT(WriteUpdateMainTE(QByteArray *)));
    CanalThread->start();

    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
    if (info.size() == 0)
    {
        ShowErrMsg(2);
        Connect();
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
        ShowErrMsg(2);
        Connect();
        return;
    }
    cn->Connect();
    if (cn->result != CN_OK)
    {
        ShowErrMsg(cn->result);
        Connect();
        return;
    }
    WriteSNAction->setEnabled(true);
    WriteHWAction->setEnabled(true);
    GetBsi();
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

void Coma::SetBaud(QString str)
{
    cn->baud = str.toInt();
}

void Coma::GetBsi()
{
    cn->Send(CN_GBsi, &pc.ModuleBsi, sizeof(publicclass::Bsi));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        CheckBsi();
}

void Coma::CheckBsi()
{
    bool WrongSN = false;

    // серийный модуль проверяется просто - если все цифры F, то серийник некорректный
    if (pc.ModuleBsi.SerialNum == 0xFFFFFFFF) // серийный номер не задан, надо задать
    {
        bool ok = false;
        qint32 tmpi = QInputDialog::getInt(this, "Серийный номер", "Серийный номер базовой платы:", 0, 1, 999999999,\
                                    1, &ok);
        if (!ok)
        {
            Disconnect();
            return;
        }
        pc.ModuleBsi.SerialNum = QString::number(tmpi, 16).toInt(0,10);
        WrongSN = true;
    }
    if (pc.ModuleBsi.SerialNumM == 0xFFFFFFFF) // серийный номер не задан, надо задать
    {
        bool ok = false;
        qint32 tmpi = QInputDialog::getInt(this, "Серийный номер", "Серийный номер мезонина:", 0, 1, 999999999,\
                                    1, &ok);
        if (!ok)
        {
            Disconnect();
            return;
        }
        pc.ModuleBsi.SerialNumM = QString::number(tmpi, 16).toInt(0,10);
        WrongSN = true;
    }
    pc.ModuleTypeString = "АВ-ТУК-";
    pc.ModuleTypeString.append(QString::number(pc.ModuleBsi.MTypeB, 16));
    pc.ModuleTypeString.append(QString::number(pc.ModuleBsi.MTypeM, 16));
    FillBsi(pc.ModuleTypeString);

    if (!DialogsAreReadyAlready)
    {
        DialogsAreReadyAlready = true;
        if (WrongSN)
        {
            cn->Send(CN_Wsn, &pc.ModuleBsi.SerialNum, 4);
            while (cn->Busy)
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            if (cn->result == CN_OK)
                AllIsOk();
        }
        else
            AllIsOk();
    }
}

void Coma::FillBsi(QString MType)
{
    SetText("mtypele", MType);
    SetText("hwverle", pc.VerToStr(pc.ModuleBsi.Hwver));
    SetText("hwvermle", pc.VerToStr(pc.ModuleBsi.HwverM));
    SetText("fwverle", pc.VerToStr(pc.ModuleBsi.Fwver));
    SetText("cfcrcle", "0x"+QString::number(static_cast<uint>(pc.ModuleBsi.Cfcrc), 16));
    SetText("rstle", "0x"+QString::number(pc.ModuleBsi.Rst, 16));

    // расшифровка Hth
    for (int i = 0; i < 32; i++)
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
    SetText("rstcountle", QString::number(pc.ModuleBsi.RstCount, 16));
    SetText("cpuidle", QString::number(pc.ModuleBsi.UIDHigh, 16)+QString::number(pc.ModuleBsi.UIDMid, 16)+QString::number(pc.ModuleBsi.UIDLow, 16));
    SetText("snle", QString::number(pc.ModuleBsi.SerialNum, 16));
    SetText("snmle", QString::number(pc.ModuleBsi.SerialNumM, 16));
}

void Coma::ClearBsi()
{
    SetText("mtypele", "");
    SetText("hwverle", "");
    SetText("hwvermle", "");
    SetText("fwverle", "");
    SetText("cfcrcle", "");
    SetText("rstle", "");

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
    SetText("rstcountle", "");
    SetText("cpuidle", "");
    SetText("snle", "");
    SetText("snmle", "");
}

void Coma::AllIsOk()
{
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    DownDialog = new downloaddialog;
    FwUpDialog = new fwupdialog;
    OscDialog = new oscdialog;
    if ((pc.ModuleBsi.MTypeB > 0x1F) && (pc.ModuleBsi.MTypeB < 0x30))
    {
        ConfDialog21 = new confdialog_21;
        ATuneDialog = new a_tunedialog;
        ACheckDialog = new a_checkdialog;
        MainTW->addTab(ConfDialog21, "Конфигурирование");
        MainTW->addTab(ATuneDialog, "Настройка");
        MainTW->addTab(ACheckDialog, "Проверка");
        MainTW->addTab(OscDialog, "Осциллограммы");
        MainTW->addTab(DownDialog, "События");
        MainTW->addTab(FwUpDialog, "Загрузка ВПО");
        connect(ConfDialog21,SIGNAL(BsiIsNeedToBeAcquiredAndChecked()),this,SLOT(GetBsi()));
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
        connect(EConfDialog,SIGNAL(BsiIsNeedToBeAcquiredAndChecked()),this,SLOT(GetBsi()));
        connect(ETuneDialog,SIGNAL(dataready(QByteArray)),this,SLOT(UpdateMainTE104(QByteArray)));
    }
    if (pc.ModuleBsi.Hth & HTH_CONFIG) // нет конфигурации
        ShowErrMsg(ER_NOCONF);
    if (pc.ModuleBsi.Hth & HTH_REGPARS) // нет коэффициентов
        ShowErrMsg(ER_NOTUNECOEF);
    MainTW->repaint();
    MainTW->show();
}

void Coma::Disconnect()
{
    if (!pc.Emul)
    {
        cn->Disconnect();
        QThread *CanalThread = cn->thread();
        if (CanalThread->isRunning())
        {
            CanalThread->quit();
            CanalThread->wait(1000);
        }
    }
    ClearBsi();
    DialogsAreReadyAlready = false;
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
    WriteSNAction->setEnabled(false);
    WriteHWAction->setEnabled(false);
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
    lbl = new QLabel("в 2015-2016 гг.");
    l2yout->addWidget(lbl);
    l2yout->addStretch(10);
    lbl = new QLabel;
    QPixmap pmp;
    pmp.load(":/evel.png");
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
    pc.ModuleBsi.SerialNum = 0x12345678;
    pc.ModuleBsi.Hth = 0x00;
    pc.Emul = true;
    AllIsOk();
}

void Coma::EmulE()
{
    if (pc.Emul) // если уже в режиме эмуляции, выход
        return;
    pc.ModuleBsi.MTypeB = MTB_80;
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("emuledlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("Выберите тип модуля:");
    hlyout->addWidget(lbl);
    QStringListModel *mdl = new QStringListModel;
    QStringList sl = QStringList() << "Э2Т0Н" << "Э1Т1Н" << "Э0Т2Н";
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
/*    QDialog *dlg = this->findChild<QDialog *>("emuledlg");
    QComboBox *cb = this->findChild<QComboBox *>("extxn");
    if ((dlg == 0) || (cb == 0))
    {
        DBGMSG;
        return;
    }
    switch (cb->currentIndex())
    {
    case 0:
        pc.ModuleBsi.MType1 = MTE_2T0N;
        break;
    case 1:
        pc.ModuleBsi.MType1 = MTE_1T1N;
        break;
    case 2:
        pc.ModuleBsi.MType1 = MTE_0T2N;
        break;
    default:
        DBGMSG;
        return;
        break;
    }
    dlg->close();

    Bsi_block.SerNum = 0x12345678;
    Bsi_block.Hth = 0x00;
    pc.Emul = true;
    AllIsOk(); */
}

void Coma::WriteSN()
{
    bool ok = false;
    qint32 tmpi = QInputDialog::getInt(this,"Серийный номер","Серийный номер модуля:",0,1,99999999,1,&ok);
    if (!ok)
        return;
    pc.ModuleBsi.SerialNum = QString::number(tmpi, 10).toInt(0,16);
    cn->Send(CN_Wsn, &(pc.ModuleBsi.SerialNum), 4);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        GetBsi();
}

void Coma::WriteHW()
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("hwdlg");
    dlg->setWindowTitle("Ввод версии аппаратуры");
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *lyout = new QHBoxLayout;
    s_tqSpinBox *spb = new s_tqSpinBox;
    spb->setObjectName("hwmv");
    spb->setDecimals(0);
    spb->setMinimum(0);
    spb->setMaximum(15);
    spb->setValue(1);
    QLabel *lbl = new QLabel("Версия аппаратуры модуля:");
    lyout->addWidget(lbl);
    lyout->addWidget(spb);
    lbl = new QLabel(".");
    lyout->addWidget(lbl);
    spb = new s_tqSpinBox;
    spb->setObjectName("hwlv");
    spb->setDecimals(0);
    spb->setMinimum(0);
    spb->setMaximum(15);
    spb->setValue(0);
    lyout->addWidget(spb);
    lbl = new QLabel("-");
    lyout->addWidget(lbl);
    spb = new s_tqSpinBox;
    spb->setObjectName("hwsv");
    spb->setDecimals(0);
    spb->setMinimum(0);
    spb->setMaximum(65535);
    spb->setValue(0);
    lyout->addWidget(spb);
    vlyout->addLayout(lyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(SendHW()));
    vlyout->addWidget(pb);
    dlg->setLayout(vlyout);
    dlg->exec();
}

void Coma::SendHW()
{
    QDialog *dlg = this->findChild<QDialog *>("hwdlg");
    if (dlg == 0)
    {
        DBGMSG;
        return;
    }
    s_tqSpinBox *spbmv = this->findChild<s_tqSpinBox *>("hwmv");
    s_tqSpinBox *spblv = this->findChild<s_tqSpinBox *>("hwlv");
    s_tqSpinBox *spbsv = this->findChild<s_tqSpinBox *>("hwsv");
    if ((spbmv == 0) || (spblv == 0) || (spbsv == 0))
    {
        DBGMSG;
        return;
    }
    quint32 tmpi = spbmv->value();
    tmpi <<= 24;
    tmpi |= (static_cast<quint32>(spblv->value()) << 16);
    tmpi |= static_cast<quint32>(spbsv->value());
    cn->Send(CN_WHv, &tmpi, 4);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    dlg->close();
    if (cn->result == CN_OK)
        GetBsi();
    else
    {
        ERMSG("Проблема при записи версии аппаратуры модуля");
        return;
    }
    dlg->close();
}

void Coma::SetMipDlg()
{
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("setmipdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QStringList tmpsl = pc.MIPIP.split(".");
    if (tmpsl.size()<4)
    {
        for (int i = tmpsl.size(); i < 4; i++)
            tmpsl.append("");
    }
    QLabel *lbl = new QLabel("IP-адрес МИП:");
    hlyout->addWidget(lbl);
    QLineEdit *le = new QLineEdit;
    QRegExp re;
    re.setPattern("^[0-2]{0,1}[0-9]{1,2}$");
    QValidator *val = new QRegExpValidator(re, this);
    le->setValidator(val);
    le->setObjectName("mip1");
    le->setText(tmpsl.at(0));
    hlyout->addWidget(le);
    lbl = new QLabel(".");
    hlyout->addWidget(lbl);
    le = new QLineEdit;
    le->setValidator(val);
    le->setObjectName("mip2");
    le->setText(tmpsl.at(1));
    hlyout->addWidget(le);
    lbl = new QLabel(".");
    hlyout->addWidget(lbl);
    le = new QLineEdit;
    le->setValidator(val);
    le->setObjectName("mip3");
    le->setText(tmpsl.at(2));
    hlyout->addWidget(le);
    lbl = new QLabel(".");
    hlyout->addWidget(lbl);
    le = new QLineEdit;
    le->setValidator(val);
    le->setObjectName("mip4");
    le->setText(tmpsl.at(3));
    hlyout->addWidget(le);
    lyout->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    lbl = new QLabel("ASDU:");
    hlyout->addWidget(lbl);
    s_tqSpinBox *spb = new s_tqSpinBox;
    spb->setDecimals(0);
    spb->setMinimum(1);
    spb->setMaximum(65534);
    spb->setValue(pc.MIPASDU);
    spb->setObjectName("asduspb");
    hlyout->addWidget(spb);
    hlyout->addStretch(90);
    lyout->addLayout(hlyout);

    QPushButton *pb = new QPushButton("Готово");
    pb->setObjectName("dlgpb");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetMipConPar()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    connect(this,SIGNAL(mipparset()),dlg,SLOT(close()));
    dlg->exec();
}

void Coma::SetMipConPar()
{
    pc.MIPIP.clear();
    for (int i = 1; i < 5; i++)
    {
        QLineEdit *le = this->findChild<QLineEdit *>("mip"+QString::number(i));
        if (le == 0)
            return;
        pc.MIPIP += le->text()+".";
    }
    pc.MIPIP.chop(1); // последнюю точку убираем
    s_tqSpinBox *spb = this->findChild<s_tqSpinBox *>("asduspb");
    if (spb == 0)
        return;
    pc.MIPASDU = spb->value();
    emit mipparset();
}

void Coma::StartSettingsDialog()
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("settingsdlg");
    dlg->setWindowTitle("Настройки");
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("Показывать окно ошибок при их появлении");
    hlyout->addWidget(lbl);
    QCheckBox *cb = new QCheckBox;
    QSettings *sets = new QSettings("EvelSoft","COMA");
    cb->setChecked(sets->value("ShowErrWindow", "1").toBool());
    cb->setObjectName("showerrcb");
    hlyout->addWidget(cb);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Задержка появления окна ошибок, с");
    hlyout->addWidget(lbl);
    s_tqSpinBox *spb = new s_tqSpinBox;
    spb->setObjectName("errdelayspb");
    spb->setDecimals(0);
    spb->setMinimum(1);
    spb->setMaximum(10);
    spb->setValue(sets->value("ErrWindowDelay", "5").toDouble());
    hlyout->addWidget(spb);
    vlyout->addLayout(hlyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    dlg->setLayout(vlyout);
    dlg->exec();
}

void Coma::AcceptSettings()
{
    QDialog *dlg = this->findChild<QDialog *>("settingsdlg");
    if (dlg == 0)
    {
        DBGMSG;
        return;
    }
    s_tqSpinBox *spb = this->findChild<s_tqSpinBox *>("errdelayspb");
    QCheckBox *cb = this->findChild<QCheckBox *>("showerrcb");
    if ((spb == 0) || (cb == 0))
    {
        DBGMSG;
        return;
    }
    QSettings *sets = new QSettings("EvelSoft","COMA");
    sets->setValue("ErrWindowDelay",spb->value());
    sets->setValue("ShowErrWindow", cb->isChecked());
    pc.ErrWindowDelay = spb->value();
    pc.ShowErrWindow = cb->isChecked();
    if (ERTimer->isActive())
        ERTimer->stop();
    ERTimer->setInterval(pc.ErrWindowDelay*1000);
    dlg->close();
}

void Coma::UpdateMainTE104(QByteArray ba)
{
    QString tmpString;
    if (ba.at(0) == 0x3A) // ':' - символ установления связи по Ethernet
    {
        ba.remove(0,1);
        tmpString = "Eth connected!";
    }
    else if (ba.at(0) == 0x3B) // ';' - символ закрытия связи по Ethernet
    {
        ba.remove(0,1);
        tmpString = "Eth disconnected!";
    }
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE == 0)
        return;
    MainTE->append(tmpString);
    UpdateMainTE(&ba);
}

void Coma::ReadUpdateMainTE(QByteArray *ba)
{
    cn->ReadDataMtx.lock();
    UpdateMainTE(ba);
    cn->ReadDataMtx.unlock();
}

void Coma::WriteUpdateMainTE(QByteArray *ba)
{
    cn->WriteDataMtx.lock();
    UpdateMainTE(ba);
    cn->WriteDataMtx.unlock();
}

void Coma::UpdateMainTE(QByteArray *ba)
{
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    QString tmpString;
    if (MainTE != 0)
    {
        for (int i = 0; i < ba->size(); i++)
            tmpString.append(ByteToHex(ba->at(i)));
        MainTE->append(tmpString);
        tmpString = MainTE->toPlainText();
        if (tmpString.size() > 10000)
            MainTE->setPlainText(tmpString.right(10000));
        MainTE->verticalScrollBar()->setValue(MainTE->verticalScrollBar()->maximum());
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
    if (!ERHide)
    {
        QWidget *erw = this->findChild<QWidget *>("errorwidget");
        if (erw == 0)
            return;
        erw->setGeometry(QRect(0, height()-erw->height(), width(), erw->height()));
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
    if ((abs(curPos.y() - height()) < 10) && (curPos.x() > 0) && (curPos.x() < width()))
    {
        if (ERHide)
            ShowOrHideSlideER();
    }
    else if ((abs(curPos.y() - height()) > 120) && (curPos.x() > 0) && (curPos.x() < width()))
    {
        if ((!ERHide) && (!ERTimerIsOn))
            ShowOrHideSlideER();
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
    QRect startRect(width(), 0, 0, height());
    QRect endRect(width() - w->width(), 0, w->width(), height());
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

void Coma::ShowOrHideSlideER()
{
    QWidget *w = this->findChild<QWidget *>("errorwidget");
    if (w == 0)
        return;
    if (w->isHidden())
        w->show();
    if (ERHide)
        w->setGeometry(ERGeometry);
    QPropertyAnimation *ani = new QPropertyAnimation(w, "geometry");
    ani->setDuration(500);
    QRect startRect(0, height(), width(), 0);
    QRect endRect(0, height()-w->height(), width(), w->height());
    if (ERHide)
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
    ERHide = !ERHide;
}

void Coma::ShowErrMsg(int ermsg)
{
    if (ermsg < pc.errmsgs.size())
        ERMSG(pc.errmsgs.at(ermsg));
    else
        ERMSG("Произошла неведомая фигня #"+QString::number(ermsg,10));
}

void Coma::UpdateErrorProtocol()
{
    ErrorProtocolWidget *ErWidget = this->findChild<ErrorProtocolWidget *>("errorwidget");
    if (ErWidget == 0)
    {
        DBGMSG;
        return;
    }
    if (pc.ermsgpool.isEmpty())
        return;
    if (!ERTimerIsOn && pc.ShowErrWindow)
    {
        ERTimerIsOn = true;
        ERHide = true;
        ShowOrHideSlideER();
    }
    while (!pc.ermsgpool.isEmpty())
    {
        ErWidget->AddRowToProt(pc.ermsgpool.first());
        pc.ermsgpool.removeFirst();
    }
    ERTimer->start();
}

void Coma::HideErrorProtocol()
{
    ERTimer->stop();
    ERTimerIsOn = false;
    ShowOrHideSlideER();
}

void Coma::SetProgressBar(quint32 cursize)
{
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if (prb != 0)
        prb->setValue(cursize);
    QLabel *lbl = this->findChild<QLabel *>("prblbl");
    if (lbl != 0)
        lbl->setText(pc.PrbMessage + QString::number(cursize) + " из " + QString::number(PrbSize));
    if (cursize >= PrbSize)
        DisableProgressBar();
}

void Coma::SetProgressBarSize(quint32 size)
{
    PrbSize = size;
    QLabel *lbl = this->findChild<QLabel *>("prblbl");
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if ((prb == 0) || (lbl == 0))
    {
        DBGMSG;
        return;
    }
    lbl->setText(pc.PrbMessage + QString::number(size));
    prb->setMinimum(0);
    prb->setMaximum(PrbSize);
    lbl->setEnabled(true);
    prb->setEnabled(true);
}

void Coma::DisableProgressBar()
{
    QLabel *lbl = this->findChild<QLabel *>("prblbl");
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if ((prb == 0) || (lbl == 0))
    {
        DBGMSG;
        return;
    }
    prb->setEnabled(false);
    lbl->setEnabled(false);
}

void Coma::SetText(QString name, QString txt)
{
    QLineEdit *le = this->findChild<QLineEdit *>(name);
    if (le == 0)
    {
        ERMSG("Не найден виджет "+name);
        return;
    }
    le->setText(txt);
}
