#include <QtTest/QTest>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QScrollBar>
#include <QStringListModel>
#include <QPropertyAnimation>
#include <QMessageBox>
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
#include <QSettings>
#include "coma.h"
#include "widgets/mytabwidget.h"
#include "widgets/errorprotocolwidget.h"

Coma::Coma(QWidget *parent)
    : QMainWindow(parent)
{
    ERTimer = new QTimer;
    ERTimer->setInterval(5000);
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
    setMinimumSize(QSize(800, 600));
    DialogsAreReadyAlready = false;
    cn = new canal;
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
    QAction *SettingsAction = new QAction(this);
    SettingsAction->setToolTip("Настройки");
    SettingsAction->setIcon(QIcon(":/settings.png"));
    connect(SettingsAction,SIGNAL(triggered()),this,SLOT(SetMipDlg()));
    tb->addAction(ConnectAction);
    tb->addAction(DisconnectAction);
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
    QMenu *MainMenu = new QMenu;
    MainMenu->setTitle("Главное");
    QAction *MainExitAction = new QAction(this);
    MainExitAction->setText("Выход");
    connect(MainExitAction,SIGNAL(triggered()),this,SLOT(Exit()));
    QAction *MainConnectAction = new QAction(this);
    MainConnectAction->setText("Соединение");
    connect(MainConnectAction,SIGNAL(triggered()),this,SLOT(Connect()));
    QAction *MainDisconnectAction = new QAction(this);
    MainDisconnectAction->setText("Разрыв соединения");
    connect(MainDisconnectAction,SIGNAL(triggered()),this,SLOT(Disconnect()));
    MainMenu->addAction(MainConnectAction);
    MainMenu->addAction(MainDisconnectAction);
    MainMenu->addAction(MainExitAction);
    MainMenuBar->addMenu(MainMenu);

    QMenu *menu = new QMenu;
    menu->setTitle("Дополнительно");
    WriteSNAction = new QAction(this);
    WriteSNAction->setText("Запись серийного номера");
    WriteSNAction->setEnabled(false);
    connect(WriteSNAction,SIGNAL(triggered()),this,SLOT(WriteSN()));
    menu->addAction(WriteSNAction);
    QAction *act = new QAction(this);
    act->setText("Установка параметров связи с МИП");
    act->setStatusTip("Настройка связи с прибором контроля электр. параметров МИП для регулировки модулей Э");
    connect(act,SIGNAL(triggered()),this,SLOT(SetMipDlg()));
    menu->addAction(act);
    MainMenuBar->addMenu(menu);

    QAction *HelpAction = new QAction(this);
    HelpAction->setText("О программе");
    connect(HelpAction,SIGNAL(triggered()),this,SLOT(GetAbout()));
    MainMenuBar->addAction(HelpAction);
    setMenuBar(MainMenuBar);

    QHBoxLayout *inlyout = new QHBoxLayout;
    MyTabWidget *MainTW = new MyTabWidget;
    MainTW->setObjectName("maintw");
//    QString MainTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
//    MainTW->tabBar()->setStyleSheet(MainTWss);
    MainTW->setTabPosition(QTabWidget::West);
    inlyout->addWidget(MainTW, 60);
    MainTW->hide();

    lyout->addLayout(inlyout, 90);
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);

    connect(cn,SIGNAL(gotsomedata(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(cn,SIGNAL(somedatawritten(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));

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
    emit stopall();
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
//    le->setTextMargins(0,0,0,0);
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
// !!!   /*
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
// !!!    */
//    Bsi_block.MType = pc.MType = MT_E; // !!!
//    Bsi_block.MType1 = pc.MType1 = MTE_0T2N; // !!!
//    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte"); // !!!
//    if (MainTE != 0) // !!!
//        MainTE->show(); // !!!
//    AllIsOk(); // !!!
}

void Coma::Next()
{
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
/*    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != 0)
        MainTE->show(); */
    WriteSNAction->setEnabled(true);
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
    cn->Send(CN_GBsi, &Bsi_block, sizeof(publicclass::Bsi));
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        CheckBsi();
}

void Coma::CheckBsi()
{
    pc.MType = Bsi_block.MType;
    pc.MType1 = Bsi_block.MType1;
    pc.CpuIdHigh = Bsi_block.CpuIdHigh;
    pc.CpuIdMid = Bsi_block.CpuIdMid;
    pc.CpuIdLow = Bsi_block.CpuIdLow;

    qint32 tmpi = 0x0000000F;
    bool WrongSN = false;

    // серийный модуль проверяется просто - если есть хоть одна не цифра, то серийник некорректный
    for (int i = 0; i < 8; i++)
    {
        qint32 tmpi2 = (Bsi_block.SerNum & tmpi) >> (i*4);
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
        Bsi_block.SerNum = QString::number(tmpi, 10).toInt(0,16);
    }
    pc.SerNum = Bsi_block.SerNum;
    qint32 tmpint;
    pc.ModuleTypeString.clear();
    switch (Bsi_block.MType)
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
            cn->Send(CN_Wsn, &(Bsi_block.SerNum), 4);
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
    le->setText((clear)?"":pc.VerToStr(Bsi_block.HWver));
    le = this->findChild<QLineEdit *>("fwverle");
    if (le == 0)
        return;
    le->setText((clear)?"":pc.VerToStr(Bsi_block.FWver));
    le = this->findChild<QLineEdit *>("cfcrcle");
    if (le == 0)
        return;
    le->setText((clear)?"":"0x"+QString::number(static_cast<uint>(Bsi_block.Cfcrc), 16));
    le = this->findChild<QLineEdit *>("rstle");
    if (le == 0)
        return;
    le->setText((clear)?"":"0x"+QString::number(Bsi_block.Rst, 16));
    // расшифровка Hth
    for (int i = 0; i < 32; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>("hth"+QString::number(i));
        if (lbl == 0)
            return;
        quint32 tmpui = (0x00000001 << i) & Bsi_block.Hth;
        if ((tmpui) && (!clear))
            lbl->setStyleSheet("QLabel {background-color: rgba(255,10,10,255); color: rgba(255,255,255,255);}");
        else
            lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");
    }
    if (!clear)
    {
        if (Bsi_block.Hth & HTH_CONFIG)
        {
            emit updateconfproper(true);
            ShowErrMsg(ER_NOCONF);
        }
        else
            emit updateconfproper(false);
        if (Bsi_block.Hth & HTH_REGPARS)
        {
            emit updatetuneproper(true);
            ShowErrMsg(ER_NOTUNECOEF);
        }
        else
            emit updatetuneproper(false);
    }
    le = this->findChild<QLineEdit *>("rstcountle");
    if (le == 0)
        return;
    le->setText((clear)?"":QString::number(Bsi_block.RstCount, 16));
    le = this->findChild<QLineEdit *>("cpuidle");
    if (le == 0)
        return;
    le->setText((clear)?"":(QString::number(Bsi_block.CpuIdHigh, 16)+QString::number(Bsi_block.CpuIdMid, 16)+QString::number(Bsi_block.CpuIdLow, 16)));
    le = this->findChild<QLineEdit *>("snle");
    if (le == 0)
        return;
    le->setText((clear)?"":QString::number(Bsi_block.SerNum, 16));
}

void Coma::AllIsOk()
{
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    DownDialog = new downloaddialog;
    FwUpDialog = new fwupdialog;
    OscDialog = new oscdialog;
    switch (Bsi_block.MType)
    {
    case MT_A:
    {
        AConfDialog = new a_confdialog;
        ATuneDialog = new a_tunedialog;
        ACheckDialog = new a_checkdialog;
        connect(this,SIGNAL(updateconfproper(bool)),AConfDialog,SLOT(UpdateProper(bool)));
        connect(this,SIGNAL(updatetuneproper(bool)),ATuneDialog,SLOT(UpdateProper(bool)));
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
    if (Bsi_block.Hth & HTH_CONFIG) // нет конфигурации
        emit updateconfproper(true);
    if (Bsi_block.Hth & HTH_REGPARS) // нет коэффициентов
        emit updatetuneproper(true);
    MainTW->repaint();
    MainTW->show();
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
    UpdateMainTE(ba);
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
        if (tmpString.size() > 10000)
            MainTE->setPlainText(tmpString.right(10000));
    }
    MainTE->verticalScrollBar()->setValue(MainTE->verticalScrollBar()->maximum());
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

void Coma::Disconnect()
{
    cn->Disconnect();
    FillBsi("",true);
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
    MainTW->hide();
}

void Coma::GetAbout()
{

}

void Coma::Exit()
{
    this->close();
}

void Coma::WriteSN()
{
    bool ok = false;
    qint32 tmpi = QInputDialog::getInt(this, "Серийный номер", "Серийный номер модуля:", 0, 1, 99999999,\
                                1, &ok);
    if (!ok)
        return;
    Bsi_block.SerNum = QString::number(tmpi, 10).toInt(0,16);
    cn->Send(CN_Wsn, &(Bsi_block.SerNum), 4);
    while (cn->Busy)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (cn->result == CN_OK)
        GetBsi();
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
    s_tqspinbox *spb = new s_tqspinbox;
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
    s_tqspinbox *spb = this->findChild<s_tqspinbox *>("asduspb");
    if (spb == 0)
        return;
    pc.MIPASDU = spb->value();
    emit mipparset();
}

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
    if ((abs(curPos.x() - width()) < 20) && (curPos.y() > 0) && (curPos.y() < height()))
    {
        if (SWHide)
            ShowOrHideSlideSW();
    }
    else if ((abs(curPos.x() - width()) > sww->width()) && (curPos.y() > 0) && (curPos.y() < height()))
    {
        if (!SWHide)
            ShowOrHideSlideSW();
    }
    if ((abs(curPos.y() - height()) < 20) && (curPos.x() > 0) && (curPos.x() < width()))
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
    if (!ERTimerIsOn)
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
