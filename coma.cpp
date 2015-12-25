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
#include "coma.h"
#include "widgets/mytabwidget.h"
#include "widgets/errorprotocolwidget.h"
#include "widgets/waitwidget.h"

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
    InitiateHth();
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
        QLabel *lbl = new QLabel(Hth[i]);
        lbl->setObjectName("hth"+QString::number(i));
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
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);

    if (pc.result)
        MAININFO("Не найден файл с сообщениями об ошибках!");

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

void Coma::closeEvent(QCloseEvent *e)
{
    if (cn)
    {
        QThread *CanalThread = cn->thread();
        if (CanalThread->isRunning())
        {
            CanalThread->quit();
            CanalThread->wait(1000);
        }
    }
    e->accept();
}

Coma::~Coma()
{
    QSettings *sets = new QSettings ("EvelSoft","Coma");
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

void Coma::InitiateHth()
{
    Hth[0] = "!OK";
    Hth[1] = "NC";
    Hth[2] = "TUPP";
    Hth[3] = "REL";
    Hth[4] = "1PPS";
    Hth[5] = "ADC";
    Hth[6] = "REG";
    Hth[7] = "CONF";
    Hth[8] = "LS";
    Hth[9] = "FNC";
    for (int i = 10; i < 32; i++)
        Hth[i] = "";
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

    pbh1 = connect(cn,SIGNAL(incomingdatalength(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    pbh2 = connect(cn,SIGNAL(bytesreceived(quint32)),this,SLOT(SetProgressBar(quint32)));
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
    qint32 tmpi = 0x0000000F;
    bool WrongSN = false;

    // серийный модуль проверяется просто - если есть хоть одна не цифра, то серийник некорректный
    for (int i = 0; i < 8; i++)
    {
        qint32 tmpi2 = (pc.ModuleBsi.SerNum & tmpi) >> (i*4);
        if (tmpi2 > 0x09)
            WrongSN = true;
        tmpi <<= 4;
    }

    if (WrongSN) // серийный номер не задан, надо задать
    {
        bool ok = false;
        qint32 tmpi = QInputDialog::getInt(this, "Серийный номер", "Серийный номер модуля:", 0, 1, 99999999,\
                                    1, &ok);
        if (!ok)
        {
            Disconnect();
            return;
        }
        pc.ModuleBsi.SerNum = QString::number(tmpi, 10).toInt(0,16);
    }
    qint32 tmpint;
    pc.ModuleTypeString.clear();
    switch (pc.ModuleBsi.MType)
    {
    case MT_A:
    {
        pc.ModuleTypeString.append("А");
        pc.ModuleTypeString.append(QString::number(pc.ANumD()));
        pc.ModuleTypeString.append(QString::number(pc.ANumCh1()));
        pc.ModuleTypeString.append(pc.AMTypes.at(pc.ATyp1()));
        tmpint = pc.ANumCh2();
        if (tmpint != 0)
        {
            pc.ModuleTypeString.append(QString::number(tmpint));
            pc.ModuleTypeString.append(pc.AMTypes.at(pc.ATyp2()));
        }
        pc.ModuleTypeString.append(QString("%1").arg(pc.AMdf(), 3, 10, QChar('0')));
        break;
    }
    case MT_C: // разборка подтипа модуля Ц
    {
        break;
    }
    case MT_D: // разборка подтипа модуля Д
    {
        break;
    }
    case MT_E: // разборка подтипа модуля Э
    {
        pc.ModuleTypeString.append("Э");
        pc.ModuleTypeString.append(pc.ETyp1());
        break;
    }
    default:
        break;
    }

    FillBsi(pc.ModuleTypeString);

    if (!DialogsAreReadyAlready)
    {
        DialogsAreReadyAlready = true;
        if (WrongSN)
        {
            cn->Send(CN_Wsn, &pc.ModuleBsi.SerNum, 4);
            while (cn->Busy)
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            if (cn->result == CN_OK)
                AllIsOk();
        }
        else
            AllIsOk();
    }
}

void Coma::FillBsi(QString MType, bool clear)
{
    QLineEdit *le = this->findChild<QLineEdit *>("mtypele");
    if (le == 0)
        return;
    le->setText(MType);
    le = this->findChild<QLineEdit *>("hwverle");
    if (le == 0)
        return;
    le->setText((clear)?"":pc.VerToStr(pc.ModuleBsi.HWver));
    le = this->findChild<QLineEdit *>("fwverle");
    if (le == 0)
        return;
    le->setText((clear)?"":pc.VerToStr(pc.ModuleBsi.FWver));
    le = this->findChild<QLineEdit *>("cfcrcle");
    if (le == 0)
        return;
    le->setText((clear)?"":"0x"+QString::number(static_cast<uint>(pc.ModuleBsi.Cfcrc), 16));
    le = this->findChild<QLineEdit *>("rstle");
    if (le == 0)
        return;
    le->setText((clear)?"":"0x"+QString::number(pc.ModuleBsi.Rst, 16));
    // расшифровка Hth
    for (int i = 0; i < 32; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("hth"+QString::number(i));
        if (lbl == 0)
            return;
        quint32 tmpui = (0x00000001 << i) & pc.ModuleBsi.Hth;
        if ((tmpui) && (!clear))
            lbl->setStyleSheet("QLabel {background-color: rgba(255,10,10,255); color: rgba(255,255,255,255);}");
        else
            lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");
    }
    le = this->findChild<QLineEdit *>("rstcountle");
    if (le == 0)
        return;
    le->setText((clear)?"":QString::number(pc.ModuleBsi.RstCount, 16));
    le = this->findChild<QLineEdit *>("cpuidle");
    if (le == 0)
        return;
    le->setText((clear)?"":(QString::number(pc.ModuleBsi.CpuIdHigh, 16)+QString::number(pc.ModuleBsi.CpuIdMid, 16)+QString::number(pc.ModuleBsi.CpuIdLow, 16)));
    le = this->findChild<QLineEdit *>("snle");
    if (le == 0)
        return;
    le->setText((clear)?"":QString::number(pc.ModuleBsi.SerNum, 16));
}

void Coma::AllIsOk()
{
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    DownDialog = new downloaddialog;
    FwUpDialog = new fwupdialog;
    OscDialog = new oscdialog;
    switch (pc.ModuleBsi.MType)
    {
    case MT_A:
    {
        AConfDialog = new a_confdialog;
        ATuneDialog = new a_tunedialog;
        ACheckDialog = new a_checkdialog;
        MainTW->addTab(AConfDialog, "Конфигурирование");
        MainTW->addTab(ATuneDialog, "Настройка");
        MainTW->addTab(ACheckDialog, "Проверка");
        MainTW->addTab(OscDialog, "Осциллограммы");
        MainTW->addTab(DownDialog, "События");
        MainTW->addTab(FwUpDialog, "Загрузка ВПО");
        connect(AConfDialog,SIGNAL(BsiIsNeedToBeAcquiredAndChecked()),this,SLOT(GetBsi()));
        break;
    }
    case MT_E:
    {
        EConfDialog = new e_confdialog;
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
        break;
    }
    default:
        break;
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
        cn->Disconnect();
    FillBsi("",true);
    emit stopall();
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

}

void Coma::Exit()
{
    this->close();
}

void Coma::EmulA()
{
    QTime tme;
    WaitWidget *w = new WaitWidget(0);
    tme.start();
    w->Start();
    while (tme.elapsed() < 5000)
        qApp->processEvents();
    w->Stop();
    if (pc.Emul) // если уже в режиме эмуляции, выход
        return;
    pc.ModuleBsi.MType = MT_A;
    pc.ModuleBsi.SerNum = 0x12345678;
    pc.ModuleBsi.Hth = 0x00;
    pc.Emul = true;
    AllIsOk();
}

void Coma::EmulE()
{
    if (pc.Emul) // если уже в режиме эмуляции, выход
        return;
    pc.ModuleBsi.MType = MT_E;
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
    QDialog *dlg = this->findChild<QDialog *>("emuledlg");
    QComboBox *cb = this->findChild<QComboBox *>("extxn");
    if ((dlg == 0) || (cb == 0))
    {
        MAINDBG;
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
        MAINDBG;
        return;
        break;
    }
    dlg->close();

    Bsi_block.SerNum = 0x12345678;
    Bsi_block.Hth = 0x00;
    pc.Emul = true;
    AllIsOk();
}

void Coma::WriteSN()
{
    bool ok = false;
    qint32 tmpi = QInputDialog::getInt(this,"Серийный номер","Серийный номер модуля:",0,1,99999999,1,&ok);
    if (!ok)
        return;
    pc.ModuleBsi.SerNum = QString::number(tmpi, 10).toInt(0,16);
    cn->Send(CN_Wsn, &(pc.ModuleBsi.SerNum), 4);
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
        MAINDBG;
        return;
    }
    s_tqSpinBox *spbmv = this->findChild<s_tqSpinBox *>("hwmv");
    s_tqSpinBox *spblv = this->findChild<s_tqSpinBox *>("hwlv");
    s_tqSpinBox *spbsv = this->findChild<s_tqSpinBox *>("hwsv");
    if ((spbmv == 0) || (spblv == 0) || (spbsv == 0))
    {
        MAINDBG;
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
        MAINER("Проблема при записи версии аппаратуры модуля");
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
        MAINDBG;
        return;
    }
    s_tqSpinBox *spb = this->findChild<s_tqSpinBox *>("errdelayspb");
    QCheckBox *cb = this->findChild<QCheckBox *>("showerrcb");
    if ((spb == 0) || (cb == 0))
    {
        MAINDBG;
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
        MAINER(pc.errmsgs.at(ermsg));
    else
        MAINER("Произошла неведомая фигня #"+QString::number(ermsg,10));
}

void Coma::UpdateErrorProtocol()
{
    ErrorProtocolWidget *ErWidget = this->findChild<ErrorProtocolWidget *>("errorwidget");
    if (ErWidget == 0)
    {
        MAINDBG;
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
    QProgressBar *prb = this->findChild<QProgressBar *>("oscprb");
    if (prb != 0)
        prb->setValue(cursize);
}

void Coma::SetProgressBarSize(quint32 size)
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(cn,SIGNAL(SendEnd()),dlg,SLOT(close()));
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("Загрузка и обработка данных...");
    lyout->addWidget(lbl,0,Qt::AlignTop);
    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("oscprb");
    prb->setOrientation(Qt::Horizontal);
    prb->setMinimumWidth(500);
    prb->setMinimum(0);
    prb->setMaximum(size);
    lyout->addWidget(prb);
    dlg->setLayout(lyout);
    if (!cn->Busy)
        dlg->close();
    else
        dlg->setVisible(true);
/*    if (!cn->Busy)
        dlg->close(); */
}
