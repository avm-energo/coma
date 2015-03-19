#include <QLabel>
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
    setMinimumSize(QSize(800, 550));
    QWidget *wdgt = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QLabel *lbl1 = new QLabel("КАВТУК");
    QFont font;
    font.setBold(true);
    font.setPointSize(24);
    lbl1->setFont(font);

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
    inlyout->addWidget(MainTW, 75);

    QTextEdit *MainTE = new QTextEdit;
    MainTE->setObjectName("mainte");
    MainTE->hide();
    inlyout->addWidget(MainTE, 25);
    lyout->addLayout(inlyout);
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
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
    connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetPort(QString)));
    QStringListModel *tmpmodel = new QStringListModel;
    QStringList tmpsl;
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
    for (i = 0; i < info.size(); i++)
        tmpsl << info.at(i).portName();
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
    tmpmodel->setStringList(tmpsl);
    baudscb->setModel(tmpmodel);
    lyout->addWidget(baudscb);

    QPushButton *nextL = new QPushButton("Далее");
    connect(nextL,SIGNAL(clicked()),this,SLOT(Next()));
    lyout->addWidget(nextL);
    dlg->setLayout(lyout);
    connect(this,SIGNAL(portopened()),dlg,SLOT(close()));
    connect(this,SIGNAL(portopened()),this,SLOT(GetBsi()));
    dlg->exec();
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
        MainTE->hide();
    MainTW->hide();
}

void ConSet::Next()
{
    pc.serial.setPort(port);
    pc.serial.setBaudRate(baud);
    pc.serial.setParity(QSerialPort::NoParity);
    pc.serial.setDataBits(QSerialPort::Data8);
    pc.serial.setFlowControl(QSerialPort::NoFlowControl);
    pc.serial.setStopBits(QSerialPort::OneStop);
    if (pc.serial.open(QIODevice::ReadWrite))
        GetBsi();
    else
        QMessageBox::critical(this,"error!","Ошибка открытия порта " + QString::number(pc.serial.error()));
}

void ConSet::DownloadArchives()
{

}

void ConSet::DownloadWaveforms()
{

}

void ConSet::GetAbout()
{

}

void ConSet::Exit()
{
    this->close();
}

void ConSet::SetPort(QString str)
{
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
    for (int i = 0; i < info.size(); i++)
    {
        if (info.at(i).portName() == str)
        {
            port = info.at(i);
            return;
        }
    }
}

void ConSet::SetBaud(QString str)
{
    baud = str.toInt();
}

void ConSet::GetBsi()
{
    QThread *thread = new QThread;
    SThread = new SerialThread(&pc.serial);
    SThread->moveToThread(thread);
    thread->start();
    QByteArray tmpba = ">GBsi";
    connect(SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(CheckBsi(QByteArray)));
    SThread->WriteData(tmpba);
}

void ConSet::CheckBsi(QByteArray ba)
{
    QByteArray cmpba;
    cmpba.resize(4);
    cmpba.append(0x00);
    cmpba.append(0x00);
    cmpba.append(0x00);
    cmpba.append(0x03);
    for (int i = 0; i < 4; i++)
    {
        if (cmpba.at(i) != ba.at(i))
            QMessageBox::critical(this,"error!","Модуль не распознан");
    }
    emit moduleisok();
}

void ConSet::AllIsOk()
{
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    ConfDialog = new confdialog;
    TuneDialog = new tunedialog;
    CheckDialog = new checkdialog;
    DownDialog = new downloaddialog;
    FwUpDialog = new fwupdialog;
    MainTW->addTab(ConfDialog, "Конфигурирование");
    MainTW->addTab(TuneDialog, "Настройка");
    MainTW->addTab(CheckDialog, "Проверка");
    MainTW->addTab(DownDialog, "Скачать");
    MainTW->addTab(FwUpDialog, "Загрузка ВПО");
    MainTW->repaint();
    MainTW->show();
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != 0)
        MainTE->show();
    emit portopened();
}
