#include <QtTest/QTest>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QScrollBar>
#include <QStringListModel>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFont>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QThread>
#include <QInputDialog>
#include "mytabwidget.h"
#include <QDialog>
#include "conset.h"
#include "config.h"

ConSet::ConSet(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("КАВТУК");
    setMinimumSize(QSize(800, 600));
    DialogsAreReadyAlready = false;
    thread = new QThread;
    cn = new canal;
    QWidget *wdgt = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    setMinimumHeight(650);

    QHBoxLayout *uplyout = new QHBoxLayout;
    QLabel *uplbl1 = new QLabel("Модуль АВТУК-");
    QLineEdit *uple1 = new QLineEdit("");
    uple1->setObjectName("mtypele");
    uple1->setEnabled(false);
    uple1->setTextMargins(0,0,0,0);
    QLabel *uplbl2 = new QLabel("Аппаратная версия:");
    QLineEdit *uple2 = new QLineEdit("");
    uple2->setObjectName("hwverle");
    uple2->setEnabled(false);
    uple2->setTextMargins(0,0,0,0);
    QLabel *uplbl3 = new QLabel("Версия ПО:");
    QLineEdit *uple3 = new QLineEdit("");
    uple3->setObjectName("fwverle");
    uple3->setEnabled(false);
    uple3->setTextMargins(0,0,0,0);
    QLabel *uplbl4 = new QLabel("КС конфигурации:");
    QLineEdit *uple4 = new QLineEdit("");
    uple4->setObjectName("cfcrcle");
    uple4->setEnabled(false);
    uple4->setTextMargins(0,0,0,0);
    QLabel *uplbl5 = new QLabel("Сброс:");
    QLineEdit *uple5 = new QLineEdit("");
    uple5->setObjectName("rstle");
    uple5->setEnabled(false);
    uple5->setTextMargins(0,0,0,0);
    QLabel *uplbl5_2 = new QLabel("Количество сбросов:");
    QLineEdit *uple5_2 = new QLineEdit("");
    uple5_2->setObjectName("rstcountle");
    uple5_2->setEnabled(false);
    uple5_2->setTextMargins(0,0,0,0);
    QLabel *uplbl6 = new QLabel("Неиспр (01=OK):");
    QLineEdit *uple6 = new QLineEdit("");
    uple6->setObjectName("hthle");
    uple6->setEnabled(false);
    uple6->setTextMargins(0,0,0,0);
    QLabel *uplbl7 = new QLabel("Серийный номер:");
    QLineEdit *uple7 = new QLineEdit("");
    uple7->setObjectName("snle");
    uple7->setEnabled(false);
    uple7->setTextMargins(0,0,0,0);
    QLabel *uplbl8 = new QLabel("ИД процессора:");
    QLineEdit *uple8 = new QLineEdit("");
    uple8->setObjectName("cpuidle");
    uple8->setEnabled(false);
    uple8->setTextMargins(0,0,0,0);

    uplyout->addWidget(uplbl1);
    uplyout->addWidget(uple1);
    uplyout->addWidget(uplbl4);
    uplyout->addWidget(uple4);
    uplyout->addWidget(uplbl5);
    uplyout->addWidget(uple5);
    uplyout->addWidget(uplbl5_2);
    uplyout->addWidget(uple5_2);
    uplyout->addWidget(uplbl6);
    uplyout->addWidget(uple6);
    lyout->addLayout(uplyout);

    uplyout = new QHBoxLayout;
    uplyout->addWidget(uplbl2);
    uplyout->addWidget(uple2, 80);
    uplyout->addWidget(uplbl3);
    uplyout->addWidget(uple3, 80);
    uplyout->addWidget(uplbl7);
    uplyout->addWidget(uple7, 80);
    uplyout->addWidget(uplbl8);
    uplyout->addWidget(uple8, 80);
    lyout->addLayout(uplyout);

    lyout->addStretch(1);

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

    QAction *HelpAction = new QAction(this);
    HelpAction->setText("О программе");
    connect(HelpAction,SIGNAL(triggered()),this,SLOT(GetAbout()));
    MainMenuBar->addAction(HelpAction);
    setMenuBar(MainMenuBar);

    QHBoxLayout *inlyout = new QHBoxLayout;
    MyTabWidget *MainTW = new MyTabWidget;
    MainTW->setObjectName("maintw");
    MainTW->setTabPosition(QTabWidget::West);
    MainTW->hide();
    inlyout->addWidget(MainTW, 60);

    QTextEdit *MainTE = new QTextEdit;
    MainTE->setObjectName("mainte");
    MainTE->hide();
    inlyout->addWidget(MainTE, 40);
    lyout->addLayout(inlyout, 90);
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
}

ConSet::~ConSet()
{
    thread->quit();
    thread->wait();
    delete thread;
}

void ConSet::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}

void ConSet::Connect()
{
// !!! /*
    int i;
    pc.SThread = new SerialThread();
    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QComboBox *portscb = new QComboBox;
    portscb->setObjectName("connectportscb");
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
    QComboBox *baudscb = new QComboBox;
    baudscb->setObjectName("connectbaudscb");
    connect(baudscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetBaud(QString)));
    tmpmodel = new QStringListModel;
    tmpsl.clear();
    QList<qint32> bauds = QSerialPortInfo::standardBaudRates();
    for (i = 0; i < bauds.size(); i++)
        tmpsl << QString::number(bauds.at(i));
    SetBaud(tmpsl.at(0));
    tmpmodel->setStringList(tmpsl);
    baudscb->setModel(tmpmodel);
    lyout->addWidget(baudscb);

    QPushButton *nextL = new QPushButton("Далее");
    connect(nextL,SIGNAL(clicked()),this,SLOT(Next()));
    lyout->addWidget(nextL);
    dlg->setLayout(lyout);
    connect(this,SIGNAL(portopened()),dlg,SLOT(close()));
    dlg->exec(); //*/
//    Bsi_block.MType = pc.MType = MT_A; // !!!
//    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte"); // !!!
//    if (MainTE != 0) // !!!
//        MainTE->show(); // !!!
//    AllIsOk(); // !!!
}

void ConSet::Next()
{
    pc.SThread->moveToThread(thread);
    connect(thread, SIGNAL(started()), pc.SThread, SLOT(run()));
    connect(this,SIGNAL(stopall()),pc.SThread,SLOT(stop()));
    connect(pc.SThread,SIGNAL(datawritten(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(pc.SThread,SIGNAL(error(int)),this,SLOT(ShowErrMsg(int)));
    connect(pc.SThread,SIGNAL(finished()),this,SLOT(KillSThread()));
    connect(pc.SThread,SIGNAL(finished()),thread,SLOT(quit()));
    connect(thread,SIGNAL(finished()),pc.SThread,SLOT(deleteLater()));
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != 0)
        MainTE->show();
    thread->start();
    emit portopened();
    GetBsi();
}

void ConSet::SetPort(QString str)
{
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
    for (int i = 0; i < info.size(); i++)
    {
        if (info.at(i).portName() == str)
        {
            pc.SThread->portinfo = info.at(i);
            return;
        }
    }
}

void ConSet::SetBaud(QString str)
{
    pc.SThread->baud = str.toInt();
}

void ConSet::ShowErrMsg(int ermsg)
{
    QMessageBox::critical(this,"error!",errmsgs.at(ermsg));
}

void ConSet::GetBsi()
{
    connect(cn,SIGNAL(DataReady()),this,SLOT(CheckBsi()));
    cn->Send(CN_GBsi, &Bsi_block, sizeof(publicclass::Bsi));
}

void ConSet::CheckBsi()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(CheckBsi()));
    if (cn->result)
    {
        ShowErrMsg(cn->result);
        return;
    }
    pc.MType = Bsi_block.MType;
    pc.MType1 = Bsi_block.MType1;
    pc.CpuId = Bsi_block.CpuId;

    qint32 tmpi = 0x0000000F;
    bool WrongSN = false;

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
    QLineEdit *le = this->findChild<QLineEdit *>("mtypele");
    if (le == 0)
        return;
    QString MType;
    qint32 tmpint;
    switch (Bsi_block.MType)
    {
    case MT_A:
    {
        MType.append("А");
        MType.append(QString::number(pc.ANumD()));
        MType.append(QString::number(pc.ANumCh1()));
        MType.append(pc.AMTypes.at(pc.ATyp1()));
        tmpint = pc.ANumCh2();
        if (tmpint != 0)
        {
            MType.append(QString::number(tmpint));
            MType.append(pc.AMTypes.at(pc.ATyp2()));
        }
        MType.append(QString("%1").arg(pc.AMdf(), 3, 10, QChar('0')));
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
        MType.append("Э");
        MType.append(pc.ETyp1());
        break;
    }
    default:
        break;
    }
    le->setText(MType);
    le = this->findChild<QLineEdit *>("hwverle");
    if (le == 0)
        return;
    le->setText(pc.VerToStr(Bsi_block.HWver));
    le = this->findChild<QLineEdit *>("fwverle");
    if (le == 0)
        return;
    le->setText(pc.VerToStr(Bsi_block.FWver));
    le = this->findChild<QLineEdit *>("cfcrcle");
    if (le == 0)
        return;
    le->setText(QString::number(static_cast<uint>(Bsi_block.Cfcrc), 16));
    le = this->findChild<QLineEdit *>("rstle");
    if (le == 0)
        return;
    le->setText(QString::number(Bsi_block.Rst, 16));
    le = this->findChild<QLineEdit *>("hthle");
    if (le == 0)
        return;
    le->setText(QString::number(Bsi_block.Hth, 16));
    le = this->findChild<QLineEdit *>("rstcountle");
    if (le == 0)
        return;
    le->setText(QString::number(Bsi_block.RstCount, 16));
    le = this->findChild<QLineEdit *>("cpuidle");
    if (le == 0)
        return;
    le->setText(QString::number(Bsi_block.CpuId, 16));
    le = this->findChild<QLineEdit *>("snle");
    if (le == 0)
        return;
    le->setText(QString::number(Bsi_block.SerNum, 16));

    if (!DialogsAreReadyAlready)
    {
        DialogsAreReadyAlready = true;
        if (WrongSN)
        {
            connect(cn,SIGNAL(DataReady()),this,SLOT(AllIsOk()));
            cn->Send(CN_Wsn, &(Bsi_block.SerNum), 4);
        }
        else
            AllIsOk();
    }
}

void ConSet::AllIsOk()
{
    disconnect(cn,SIGNAL(DataReady()),this,SLOT(AllIsOk()));
    if (cn->result)
    {
        ShowErrMsg(cn->result);
        return;
    }
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    switch (Bsi_block.MType)
    {
    case MT_A:
    {
        AConfDialog = new a_confdialog;
        ATuneDialog = new a_tunedialog;
        ACheckDialog = new a_checkdialog;
        DownDialog = new downloaddialog;
        FwUpDialog = new fwupdialog;
        MainTW->addTab(AConfDialog, "Конфигурирование");
        MainTW->addTab(ATuneDialog, "Настройка");
        MainTW->addTab(ACheckDialog, "Проверка");
        MainTW->addTab(DownDialog, "Скачать");
        MainTW->addTab(FwUpDialog, "Загрузка ВПО");
        connect(AConfDialog,SIGNAL(BsiIsNeedToBeAcquiredAndChecked()),this,SLOT(GetBsi()));
        break;
    }
    case MT_E:
    {
        EConfDialog = new e_confdialog;
        ETuneDialog = new e_tunedialog;
        ECheckDialog = new e_checkdialog;
        DownDialog = new downloaddialog;
        FwUpDialog = new fwupdialog;
        MainTW->addTab(EConfDialog, "Конфигурирование");
        MainTW->addTab(ETuneDialog, "Настройка");
        MainTW->addTab(ECheckDialog, "Проверка");
        MainTW->addTab(DownDialog, "Скачать");
        MainTW->addTab(FwUpDialog, "Загрузка ВПО");
        connect(EConfDialog,SIGNAL(BsiIsNeedToBeAcquiredAndChecked()),this,SLOT(GetBsi()));
        break;
    }
    default:
        break;
    }
    MainTW->repaint();
    MainTW->show();
}

void ConSet::UpdateMainTE(QByteArray ba)
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

QString ConSet::ByteToHex(quint8 hb)
{
    QString tmpString;
    quint8 halfbyte = hb & 0xF0;
    halfbyte >>= 4;
    tmpString.append(QString::number(halfbyte, 16));
    halfbyte = hb & 0x0F;
    tmpString.append(QString::number(halfbyte, 16));
    return tmpString;
}

void ConSet::Disconnect()
{
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
    {
        MainTE->clear();
        MainTE->hide();
    }
    MainTW->hide();
}

void ConSet::KillSThread()
{
    delete pc.SThread;
}

void ConSet::GetAbout()
{

}

void ConSet::Exit()
{
    this->close();
}
