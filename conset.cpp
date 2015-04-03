#include <QtTest/QTest>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStringListModel>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFont>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QThread>
#include "mytabwidget.h"
#include <QDialog>

#include "conset.h"
#include "publicclass.h"

ConSet::ConSet(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("КАВТУК");
    setMinimumSize(QSize(800, 550));
    QWidget *wdgt = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;

    QHBoxLayout *uplyout = new QHBoxLayout;
    QLabel *uplbl1 = new QLabel("Модуль: АВТУК-");
    QLineEdit *uple1 = new QLineEdit("");
    uple1->setObjectName("mtypele");
    uple1->setEnabled(false);
    uple1->setMaximumWidth(55);
    uple1->setTextMargins(0,0,0,0);
    QLabel *uplbl2 = new QLabel("Аппаратная версия:");
    QLineEdit *uple2 = new QLineEdit("");
    uple2->setObjectName("hwverle");
    uple2->setEnabled(false);
    uple2->setMaximumWidth(70);
    uple2->setTextMargins(0,0,0,0);
    QLabel *uplbl3 = new QLabel("Версия ПО:");
    QLineEdit *uple3 = new QLineEdit("");
    uple3->setObjectName("fwverle");
    uple3->setEnabled(false);
    uple3->setMaximumWidth(70);
    uple3->setTextMargins(0,0,0,0);
    QLabel *uplbl4 = new QLabel("КС конфигурации:");
    QLineEdit *uple4 = new QLineEdit("");
    uple4->setObjectName("cfcrcle");
    uple4->setEnabled(false);
    uple4->setMaximumWidth(60);
    uple4->setTextMargins(0,0,0,0);
    QLabel *uplbl5 = new QLabel("Сброс:");
    QLineEdit *uple5 = new QLineEdit("");
    uple5->setObjectName("rstle");
    uple5->setEnabled(false);
    uple5->setMaximumWidth(60);
    uple5->setTextMargins(0,0,0,0);
    QLabel *uplbl6 = new QLabel("Неисправности (01=OK):");
    QLineEdit *uple6 = new QLineEdit("");
    uple6->setObjectName("hthle");
    uple6->setEnabled(false);
    uple6->setMaximumWidth(60);
    uple6->setTextMargins(0,0,0,0);

    uplyout->addWidget(uplbl1);
    uplyout->addWidget(uple1);
    uplyout->addWidget(uplbl2);
    uplyout->addWidget(uple2);
    uplyout->addWidget(uplbl3);
    uplyout->addWidget(uple3);
    uplyout->addWidget(uplbl4);
    uplyout->addWidget(uple4);
    uplyout->addWidget(uplbl5);
    uplyout->addWidget(uple5);
    uplyout->addWidget(uplbl6);
    uplyout->addWidget(uple6);
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
    pc.SThread = new SerialThread();
    connect(pc.SThread,SIGNAL(error(int)),this,SLOT(ShowErrMsg(int)));
}

ConSet::~ConSet()
{

}

void ConSet::Connect()
{
    int i;
    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QComboBox *portscb = new QComboBox;
    portscb->setObjectName("connectportscb");
    connect(portscb,SIGNAL(currentIndexChanged(QString)),pc.SThread,SLOT(SetPort(QString)));
    QStringListModel *tmpmodel = new QStringListModel;
    QStringList tmpsl;
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
    for (i = 0; i < info.size(); i++)
        tmpsl << info.at(i).portName();
    pc.SThread->SetPort(tmpsl.at(0));
    tmpmodel->setStringList(tmpsl);
    portscb->setModel(tmpmodel);
    lyout->addWidget(portscb);
    QComboBox *baudscb = new QComboBox;
    baudscb->setObjectName("connectbaudscb");
    connect(baudscb,SIGNAL(currentIndexChanged(QString)),pc.SThread,SLOT(SetBaud(QString)));
    tmpmodel = new QStringListModel;
    tmpsl.clear();
    QList<qint32> bauds = QSerialPortInfo::standardBaudRates();
    for (i = 0; i < bauds.size(); i++)
        tmpsl << QString::number(bauds.at(i));
    pc.SThread->SetBaud(tmpsl.at(0));
    tmpmodel->setStringList(tmpsl);
    baudscb->setModel(tmpmodel);
    lyout->addWidget(baudscb);

    QPushButton *nextL = new QPushButton("Далее");
    connect(nextL,SIGNAL(clicked()),this,SLOT(Next()));
    lyout->addWidget(nextL);
    dlg->setLayout(lyout);
    connect(this,SIGNAL(portopened()),dlg,SLOT(close()));
    dlg->exec();
}

void ConSet::Next()
{
    QThread *thread = new QThread;
    pc.SThread->moveToThread(thread);
    connect(thread, SIGNAL(started()), pc.SThread, SLOT(run()));
    connect(pc.SThread,SIGNAL(timeout()),this,SLOT(Timeout())); // таймаут по отсутствию принятых данных
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != 0)
        MainTE->show();
    thread->start();
    emit portopened();
    GetBsi();
}

void ConSet::ShowErrMsg(int ermsg)
{
    QMessageBox::critical(this,"error!","Ошибка открытия порта");
}

void ConSet::GetBsi()
{
    QByteArray tmpba = ">GBsi";
//    QByteArray tmpba = ">GBda";
    connect(pc.SThread,SIGNAL(receivecompleted()),this,SLOT(CheckBsi()));
    connect(pc.SThread,SIGNAL(datawritten(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    pc.SThread->InitiateWriteDataToPort(tmpba);
//    CheckBsi(); // temporary
}

void ConSet::CheckBsi()
{
    // раскидаем принятый inbuf по полочкам
    unsigned char *Bsipos;
    disconnect(pc.SThread,SIGNAL(receivecompleted()),this,SLOT(CheckBsi()));
    Bsi Bsi_block;
    inbuf = pc.SThread->data();
    Bsipos = reinterpret_cast<unsigned char *>(&Bsi_block);
    for (int i = 1; i < inbuf.size(); i++) // пропускаем "<", поэтому не от 0
    {
        if (i > (sizeof(Bsi)-1))
            break;
        *Bsipos = inbuf.at(i);
        Bsipos++;
    }

    pc.MType = Bsi_block.MType;
    pc.MType1 = Bsi_block.MType1;

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
    le->setText(QString::number(Bsi_block.Cfcrc, 16));
    le = this->findChild<QLineEdit *>("rstle");
    if (le == 0)
        return;
    le->setText(QString::number(Bsi_block.Rst, 16));
    le = this->findChild<QLineEdit *>("hthle");
    if (le == 0)
        return;
    le->setText(QString::number(Bsi_block.Hth, 16));

/*    while (1)
    {
        QTest::qWait(100);
    } */
    AllIsOk();
}

void ConSet::AllIsOk()
{

    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
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
            MainTE->setPlainText(tmpString.right(tmpString.size()-10000));
    }
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

void ConSet::Timeout()
{
    QMessageBox::warning(this,"warning!","Произошёл таймаут ожидания данных");
//    Disconnect();
}

void ConSet::Disconnect()
{
    pc.serial.close();
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
//    MainTW->hide();
}

void ConSet::GetAbout()
{

}

void ConSet::Exit()
{
    this->close();
}
