#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QDir>
#include <QMenu>
#include <QMenuBar>
#include <QEventLoop>
#include <QScrollBar>
#include <QProgressBar>
#include <QSettings>
#include <QStringListModel>
#include <QStandardPaths>
#include <QPropertyAnimation>
#include <QtSerialPort/QSerialPortInfo>
#include "mainwindow.h"
#include "canal.h"
#include "commands.h"
#include "widgets/wd_func.h"
#include "widgets/mytabwidget.h"
#include "widgets/messagebox.h"
#include "dialogs/errordialog.h"
#include "dialogs/hiddendialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/keypressdialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(pc.HomeDir);
    if (!dir.exists())
        dir.mkpath(".");
    cn = new Canal;
    S2Config.clear();
    ConfB = ConfM = 0;
    SetupMenubar();
    PrepareTimers();
    LoadSettings();
    connect(cn,SIGNAL(oscerasesize(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    connect(cn,SIGNAL(osceraseremaining(quint32)),this,SLOT(SetProgressBar(quint32)));
    connect(cn,SIGNAL(incomingdatalength(quint32)),this,SLOT(SetProgressBarSize(quint32)));
    connect(cn,SIGNAL(bytesreceived(quint32)),this,SLOT(SetProgressBar(quint32)));
    connect(cn,SIGNAL(sendend()),this,SLOT(DisableProgressBar()));
    connect(cn,SIGNAL(readbytessignal(QByteArray &)),this,SLOT(UpdateMainTE(QByteArray &)));
    connect(cn,SIGNAL(writebytessignal(QByteArray &)),this,SLOT(UpdateMainTE(QByteArray &)));
    connect(cn,SIGNAL(Disconnected()),this,SLOT(ContinueDisconnect()));
    connect(this,SIGNAL(Retry()),this,SLOT(Stage1()));
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
    return w;
}

QWidget *MainWindow::Least()
{
    QWidget *w = new QWidget;
//    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *inlyout = new QHBoxLayout;
    MyTabWidget *MainTW = new MyTabWidget;
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
    QLabel *lbl = new QLabel;
    lbl->setObjectName("prblbl");
    inlyout->addWidget(lbl);
    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prbprb");
    prb->setOrientation(Qt::Horizontal);
    prb->setMinimumWidth(500);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    w->setLayout(lyout);
    return w;
}

void MainWindow::SetSlideWidget()
{
    QWidget *SlideWidget = new QWidget(this);
    SlideWidget->setObjectName("slidew");
    SlideWidget->setStyleSheet("QWidget {background-color: rgba(110,234,145,255);}");
    QVBoxLayout *slyout = new QVBoxLayout;
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

void MainWindow::SetupMenubar()
{
    QMenuBar *MainMenuBar = new QMenuBar;
    QString tmps = "QMenuBar {background-color: "+QString(MAINWINCLRA1)+";}"\
            "QMenuBar::item {background-color: "+QString(MAINWINCLRA1)+";}";
    MainMenuBar->setStyleSheet(tmps);
    QMenu *MainMenu = new QMenu;
    MainMenu->setTitle("Главное");
    QAction *act = new QAction(this);
    act->setText("Выход");
    connect(act,SIGNAL(triggered()),this,SLOT(close()));
    MainMenu->addAction(act);
    act = new QAction(this);
    act->setText("Соединение");
    act->setIcon(QIcon(":/pic/play.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
    MainMenu->addAction(act);
    act = new QAction(this);
    act->setText("Разрыв соединения");
    act->setIcon(QIcon(":/pic/stop.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Disconnect()));
    MainMenu->addAction(act);
    MainMenuBar->addMenu(MainMenu);

    QMenu *menu = new QMenu;
    menu->setTitle("Секретные операции");
    act = new QAction(this);
    act->setText("Работа с Hidden Block");
    connect(act,SIGNAL(triggered()),this,SLOT(OpenBhbDialog()));
    menu->addAction(act);
    MainMenuBar->addMenu(menu);

    menu = new QMenu;
    menu->setTitle("Настройки");
    act = new QAction(this);
    act->setText("Настройки");
    act->setIcon(QIcon(":/pic/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    menu->addAction(act);
    MainMenuBar->addMenu(menu);

    act = new QAction(this);
    act->setText("О программе");
    connect(act,SIGNAL(triggered()),this,SLOT(GetAbout()));
    MainMenuBar->addAction(act);
    setMenuBar(MainMenuBar);
}

void MainWindow::PrepareTimers()
{
    QTimer *MouseTimer = new QTimer;
    connect(MouseTimer,SIGNAL(timeout()),this,SLOT(MouseMove()));
    MouseTimer->start(50);
}

void MainWindow::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"+PROGNAME+"/";
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    pc.Port = sets->value("Port", "COM1").toString();
    pc.HomeDir = sets->value("Homedir", HomeDir).toString();
    pc.OrganizationString = sets->value("Organization", "Р&К").toString();
}

void MainWindow::SaveSettings()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    sets->setValue("Port", pc.Port);
    sets->setValue("Homedir", pc.HomeDir);
    sets->setValue("Organization", pc.OrganizationString);
}

void MainWindow::ClearTW()
{
    S2Config.clear();
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    while (MainTW->count())
    {
        QWidget *wdgt = MainTW->widget(0);
        MainTW->removeTab(0);
        wdgt->deleteLater();
    }
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != 0)
        MainTE->clear();
}

void MainWindow::ShowOrHideSlideSW()
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

int MainWindow::CheckPassword()
{
    Cancelled = ok = false;
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc"));
    connect(dlg,SIGNAL(Cancelled()),this,SLOT(CancelPswCheck()));
    connect(dlg,SIGNAL(Finished(QString &)),this,SLOT(PasswordCheck(QString &)));
    connect(this,SIGNAL(PasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->setLayout(vlyout);
    dlg->show();
    PasswordLoop.exec();
    if (Cancelled)
        return GENERALERROR;
    if (!ok)
    {
        MessageBox2::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

void MainWindow::Stage1()
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
        pc.ErMsg(USB_NOCOMER);
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
    connect(nextL,SIGNAL(clicked()),this,SLOT(Stage1_5()));
    connect(nextL, SIGNAL(clicked(bool)),dlg,SLOT(close()));
    lyout->addWidget(nextL);
    dlg->setLayout(lyout);
    dlg->exec();
}

void MainWindow::Stage1_5()
{
    if (!cn->Connected)
    {
        pc.PrbMessage = "Загрузка данных...";

        QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
        if (info.size() == 0)
        {
            MessageBox2::error(this, "Ошибка", "В системе нет последовательных портов");
            pc.ErMsg(USB_NOCOMER);
            emit Retry();
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
            MessageBox2::error(this, "Ошибка", "Порт не найден");
            pc.ErMsg(USB_COMER);
            emit Retry();
            return;
        }
        cn->baud = 115200;
        if (!cn->Connect())
        {
            MessageBox2::error(this, "Ошибка", "Связь по данному порту не может быть установлена");
            emit Retry();
            return;
        }
        SaveSettings();
    }
    Stage2();
}

void MainWindow::Stage2()
{
    if (CN_GetBsi(&pc.ModuleBsi, sizeof(publicclass::Bsi)) != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Блок Bsi не может быть прочитан, связь потеряна");
        cn->Disconnect();
        emit Retry();
        return;
    }
    pc.MType = ((pc.ModuleBsi.MTypeB & 0x000000FF) << 8) | (pc.ModuleBsi.MTypeM & 0x000000FF);
    pc.ModuleTypeString = "ПКДН-";
    pc.ModuleTypeString.append(QString::number(pc.MType, 16));
    if ((pc.ModuleBsi.SerialNumB == 0xFFFFFFFF) || ((pc.ModuleBsi.SerialNumM == 0xFFFFFFFF) && (pc.ModuleBsi.MTypeM != MTM_00)) || \
            (pc.ModuleBsi.SerialNum == 0xFFFFFFFF)) // серийный номер не задан, выдадим предупреждение
        OpenBhbDialog();
    Stage3();
}

void MainWindow::SetDefConf()
{
    ConfB->SetDefConf();
    if (ConfM != 0)
        ConfM->SetDefConf();
    Fill();
    MessageBox2::information(this, "Успешно", "Задана конфигурация по умолчанию");
}

void MainWindow::Fill()
{
    ConfB->Fill();
    if (ConfM != 0)
        ConfM->Fill();
}

void MainWindow::UpdateMainTE(QByteArray &ba)
{
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != 0)
        MainTE->append(ba.toHex());
}

void MainWindow::SetPort(QString str)
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

void MainWindow::PasswordCheck(QString &psw)
{
    if (psw == "SE/7520A")
        ok = true;
    else
        ok = false;
    emit PasswordChecked();
}

void MainWindow::CancelPswCheck()
{
    Cancelled = true;
}

void MainWindow::OpenBhbDialog()
{
    if (!cn->Connected)
    {
        QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модулем" : "прибором");
        MessageBox2::information(this, "Подтверждение", "Для работы данной функции необходимо сначала установить связь с "+tmps);
        return;
    }
    if (CheckPassword() == GENERALERROR)
        return;
    HiddenDialog *dlg = new HiddenDialog(HiddenDialog::PKDN);
    pc.BoardBBhb.HWVer = pc.ModuleBsi.HwverB;
    pc.BoardBBhb.ModSerialNum = pc.ModuleBsi.SerialNum;
    pc.BoardBBhb.SerialNum = pc.ModuleBsi.SerialNumB;
    pc.BoardBBhb.MType = pc.ModuleBsi.MTypeB;
    dlg->Fill(); // заполняем диалог из недавно присвоенных значений
    dlg->exec();
    if (CN_GetBsi(&pc.ModuleBsi, sizeof(publicclass::Bsi)) != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Блок Bsi не может быть прочитан, связь потеряна");
        cn->Disconnect();
        emit Retry();
    }
    emit BsiRefresh();
}

void MainWindow::StartEmul()
{
    bool ok;
    quint16 MType = sender()->objectName().toInt(&ok, 16);
    if (!ok)
    {
        ERMSG("Wrong object name in StartEmul()");
        return;
    }
    pc.ModuleBsi.MTypeB = (MType & 0xFF00) >> 8;
    pc.ModuleBsi.MTypeM = MType & 0x00FF;
    if (cn->Connected)
        Disconnect();
    pc.ModuleBsi.SerialNum = 0x12345678;
    pc.ModuleBsi.Hth = 0x00;
    pc.Emul = true;
    Stage3();
}

void MainWindow::StartSettingsDialog()
{
    SettingsDialog *dlg = new SettingsDialog;
    dlg->exec();
}

void MainWindow::ShowErrorDialog()
{
    ErrorDialog *dlg = new ErrorDialog;
    dlg->exec();
}

void MainWindow::SetProgressBarSize(quint32 size)
{
    PrbSize = size;
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if (prb == 0)
    {
        DBGMSG;
        return;
    }
    WDFunc::SetLBLText(this, "prblbl",pc.PrbMessage + QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(PrbSize);
    prb->setEnabled(true);
}

void MainWindow::SetProgressBar(quint32 cursize)
{
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if (prb != 0)
        prb->setValue(cursize);
    WDFunc::SetLBLText(this, "prblbl",pc.PrbMessage + QString::number(cursize) + " из " + QString::number(PrbSize));
    if (cursize >= PrbSize)
        DisableProgressBar();
}

void MainWindow::DisableProgressBar()
{
    QProgressBar *prb = this->findChild<QProgressBar *>("prbprb");
    if (prb == 0)
    {
        DBGMSG;
        return;
    }
    prb->setEnabled(false);
    WDFunc::SetLBLText(this, "prblbl","",false);
}

void MainWindow::GetAbout()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *l2yout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);
    QLabel *lbl = new QLabel(tmps);
    l2yout->addWidget(lbl);
    lbl = new QLabel("накорябано Ёвелем");
    l2yout->addWidget(lbl);
    lbl = new QLabel("в 2015-2017 гг.");
    l2yout->addWidget(lbl);
    l2yout->addStretch(10);
    lbl = new QLabel;
    QPixmap pmp;
    pmp.load(":/pic/evel.png");
    lbl->setPixmap(pmp);
    lbl->setMaximumSize(64,64);
    hlyout->addWidget(lbl,1);
    hlyout->setAlignment(lbl,Qt::AlignTop);
    hlyout->addLayout(l2yout,100);
    lyout->addLayout(hlyout,1);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->exec();
}

void MainWindow::Disconnect()
{
    if (!pc.Emul)
        cn->Disconnect();
    ContinueDisconnect();
}

void MainWindow::ContinueDisconnect()
{
    emit FinishAll();
    emit ClearBsi();
    ClearTW();
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    MainTW->hide();
    pc.Emul = false;
}

void MainWindow::MouseMove()
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
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (!SWHide)
    {
        QWidget *sww = this->findChild<QWidget *>("slidew");
        if (sww == 0)
            return;
        sww->setGeometry(QRect(width()-sww->width(), 0, sww->width(), height()));
    }
}

