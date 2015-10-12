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
#include <QFont>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QThread>
#include <QInputDialog>
#include <QRegExp>
#include <QSettings>
#include "mytabwidget.h"
#include <QDialog>
#include "coma.h"

Coma::Coma(QWidget *parent)
    : QMainWindow(parent)
{
    ReconTry = 0;
    InitiateHth();
    setWindowTitle("КОМА");
    setMinimumSize(QSize(800, 600));
    DialogsAreReadyAlready = false;
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

    uplyout = new QHBoxLayout;
    for (int i = 31; i >= 0; i--)
    {
        QLabel *lbl = new QLabel(Hth[i]);
        lbl->setObjectName("hth"+QString::number(i));
        lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255)}");
        uplyout->addWidget(lbl);
    }
    lyout->addLayout(uplyout);

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

    connect(cn,SIGNAL(error(int)),this,SLOT(ShowErrMsg(int)));
    connect(cn,SIGNAL(gotsomedata(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(cn,SIGNAL(somedatawritten(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));

    QSettings *sets = new QSettings ("EvelSoft","Coma");
    pc.MIPASDU = sets->value("mip/asdu","206").toInt();
    pc.MIPIP = sets->value("mip/ip","192.168.1.2").toString();
    if (pc.result)
        QMessageBox::warning(this,"ошибка!","Не найден файл с сообщениями об ошибках!");
}

Coma::~Coma()
{
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

void Coma::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
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

    QPushButton *nextL = new QPushButton("Далее");
    connect(nextL,SIGNAL(clicked()),this,SLOT(Next()));
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
    cn->Connect();
    if (cn->result != CN_OK)
    {
        ShowErrMsg(cn->result);
        return;
    }
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != 0)
        MainTE->show();
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
            cn->info = info.at(i);
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
        connect(ETuneDialog,SIGNAL(error(int)),this,SLOT(ShowErrMsg(int)));
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
    {
        MainTE->clear();
        MainTE->hide();
    }
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
    QSettings *sets = new QSettings ("EvelSoft","Coma");
    sets->setValue("mip/asdu",pc.MIPASDU);
    sets->setValue("mip/ip",pc.MIPIP);
    emit mipparset();
}

void Coma::ShowErrMsg(int ermsg)
{
    if (ermsg < pc.errmsgs.size())
        QMessageBox::critical(this,"error!",pc.errmsgs.at(ermsg));
    else
        QMessageBox::critical(this,"error!","Произошла неведомая фигня #"+QString::number(ermsg,10));
}
