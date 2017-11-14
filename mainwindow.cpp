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
#include "mainwindow.h"
#include "commands.h"
#include "widgets/wd_func.h"
#include "widgets/mytabwidget.h"
#include "widgets/messagebox.h"
#include "dialogs/errordialog.h"
#include "dialogs/hiddendialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/keypressdialog.h"
#include "widgets/etablemodel.h"
#include "widgets/s_tqtableview.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(pc.HomeDir);
    if (!dir.exists())
        dir.mkpath(".");
    S2Config.clear();
    ConfB = ConfM = 0;
#ifndef MODULE_A1
    OscD = 0;
#endif
#if PROGSIZE >= PROGSIZE_LARGE
    PrepareTimers();
#endif
    LoadSettings();
#if PROGSIZE != PROGSIZE_EMUL
#ifdef USBENABLE
    cn = new EUsbHid;
    connect(cn,SIGNAL(Retry()),this,SLOT(ShowUSBConnectDialog()));
#else
#ifdef COMPORTENABLE
    cn = new EUsbCom;
    connect(cn,SIGNAL(Retry()),this,SLOT(ShowCOMConnectDialog()));
#endif
#endif
    connect(cn,SIGNAL(SetDataSize(quint32)),this,SLOT(SetProgressBar1Size(quint32)));
    connect(cn,SIGNAL(SetDataCount(quint32)),this,SLOT(SetProgressBar1(quint32)));
    connect(cn,SIGNAL(readbytessignal(QByteArray &)),this,SLOT(UpdateMainTE(QByteArray &)));
    connect(cn,SIGNAL(writebytessignal(QByteArray &)),this,SLOT(UpdateMainTE(QByteArray &)));
    connect(cn, SIGNAL(ShowError(QString)), this, SLOT(ShowErrorMessageBox(QString)));
    connect(this,SIGNAL(Retry()),this,SLOT(Stage1_5()));
#endif
#ifndef MODULE_A1
    OscFunc = new EOscillogram;
#endif
}

MainWindow::~MainWindow()
{
    Disconnect();
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
#if PROGSIZE != PROGSIZE_EMUL
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
#endif
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
    pc.Port = sets->value("Port", "COM1").toString();
    pc.HomeDir = sets->value("Homedir", HomeDir).toString();
    pc.PovDev.DevName = sets->value("PovDevName", "UPTN").toString();
    pc.PovDev.DevSN = sets->value("PovDevSN", "00000001").toString();
    pc.PovDev.DevPrecision = sets->value("PovDevPrecision", "0.05").toString();
    pc.OrganizationString = sets->value("Organization", "Р&К").toString();
    pc.WriteUSBLog = sets->value("WriteLog", "0").toBool();
    pc.PovNumPoints = sets->value("PovNumPoints", "60").toInt();
    pc.TEEnabled = sets->value("TEEnabled", "0").toBool();
}

void MainWindow::SaveSettings()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    sets->setValue("Port", pc.Port);
    sets->setValue("Homedir", pc.HomeDir);
    sets->setValue("PovDevName", pc.PovDev.DevName);
    sets->setValue("PovDevSN", pc.PovDev.DevSN);
    sets->setValue("PovDevPrecision", pc.PovDev.DevPrecision);
    sets->setValue("Organization", pc.OrganizationString);
    sets->setValue("WriteLog", pc.WriteUSBLog);
    sets->setValue("PovNumPoints", QString::number(pc.PovNumPoints, 10));
    sets->setValue("TEEnabled", pc.TEEnabled);
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

#if PROGSIZE >= PROGSIZE_LARGE
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
#endif
#if PROGSIZE != PROGSIZE_EMUL
int MainWindow::CheckPassword()
{
    pc.Cancelled = ok = false;
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc"));
    connect(dlg,SIGNAL(Finished(QString &)),this,SLOT(PasswordCheck(QString &)));
    connect(this,SIGNAL(PasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->setLayout(vlyout);
    dlg->show();
    PasswordLoop.exec();
    if (pc.Cancelled)
        return GENERALERROR;
    if (!ok)
    {
        MessageBox2::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

void MainWindow::Stage1_5()
{
#ifdef USBENABLE
    ShowUSBConnectDialog();
#endif
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (Commands::Connect() != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Не удалось установить связь");
        QApplication::restoreOverrideCursor();
        return;
    }
    SaveSettings();
    QApplication::restoreOverrideCursor();
    Stage2();
}

void MainWindow::Stage2()
{
    int res;
    if ((res = Commands::GetBsi()) != NOERROR)
    {
        if (MessageBox2::question(this, "Ошибка", \
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
    pc.MType = ((pc.ModuleBsi.MTypeB & 0x000000FF) << 8) | (pc.ModuleBsi.MTypeM & 0x000000FF);
    pc.ModuleTypeString = "АВТУК-";
    pc.ModuleTypeString.append(QString::number(pc.MType, 16));
#if PROGSIZE >= PROGSIZE_LARGE
    if ((pc.ModuleBsi.SerialNumB == 0xFFFFFFFF) || ((pc.ModuleBsi.SerialNumM == 0xFFFFFFFF) && (pc.ModuleBsi.MTypeM != MTM_00)) || \
            (pc.ModuleBsi.SerialNum == 0xFFFFFFFF)) // серийный номер не задан, выдадим предупреждение
        OpenBhbDialog();
#endif
    Stage3();
}

void MainWindow::SetDefConf()
{
    MainConfDialog->SetDefConf();
    ConfB->SetDefConf();
    if (ConfM != 0)
        ConfM->SetDefConf();
    Fill();
    MessageBox2::information(this, "Успешно", "Задана конфигурация по умолчанию");
}

void MainWindow::Fill()
{
    MainConfDialog->Fill();
    ConfB->Fill();
    if (ConfM != 0)
        ConfM->Fill();
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::UpdateMainTE(QByteArray &ba)
{
    if (!pc.TEEnabled)
        return;
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != 0)
        MainTE->append(ba.toHex());
}

void MainWindow::SetTEEnabled(bool enabled)
{
    pc.TEEnabled = enabled;
}
#endif

void MainWindow::PasswordCheck(QString &psw)
{
    if (psw == "SE/7520A")
        ok = true;
    else
        ok = false;
    emit PasswordChecked();
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::OpenBhbDialog()
{
    if (!Commands::isConnected())
    {
        QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модулем" : "прибором");
        MessageBox2::information(this, "Подтверждение", "Для работы данной функции необходимо сначала установить связь с "+tmps);
        return;
    }
    if (CheckPassword() == GENERALERROR)
        return;

    HiddenDialog *dlg = new HiddenDialog();
    dlg->Fill(); // заполняем диалог из недавно присвоенных значений
    dlg->exec();
    Disconnect();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (Commands::Connect() != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Не удалось установить связь");
        QApplication::restoreOverrideCursor();
        return;
    }
    QApplication::restoreOverrideCursor();
    int res;
    if ((res = Commands::GetBsi()) != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Блок Bsi не может быть прочитан, ошибка " + QString::number(res));
        Commands::Disconnect();
    }
    emit BsiRefresh();
}
#endif
#endif

#if PROGSIZE >= PROGSIZE_LARGE || PROGSIZE == PROGSIZE_EMUL
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
#if PROGSIZE != PROGSIZE_EMUL
    if (cn->Connected)
        DisconnectAndClear();
#endif
    pc.ModuleBsi.SerialNum = 0x12345678;
    pc.ModuleBsi.Hth = 0x00;
    pc.Emul = true;
    Stage3();
}
#endif

void MainWindow::StartSettingsDialog()
{
    SettingsDialog *dlg = new SettingsDialog;
//    dlg->Fill();
    dlg->exec();
    SaveSettings();
}

void MainWindow::ShowErrorDialog()
{
    ErrorDialog *dlg = new ErrorDialog;
    dlg->exec();
}
#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::SetProgressBar1Size(quint32 size)
{
    SetProgressBarSize("1", size);
}

void MainWindow::SetProgressBar1(quint32 cursize)
{
    SetProgressBar("1", cursize);
}

void MainWindow::SetProgressBar2Size(quint32 size)
{
    SetProgressBarSize("2", size);
}

void MainWindow::SetProgressBar2(quint32 cursize)
{
    SetProgressBar("2", cursize);
}

void MainWindow::SetProgressBarSize(QString prbnum, quint32 size)
{
    QString prbname = "prb"+prbnum+"prb";
    QString lblname = "prb"+prbnum+"lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == 0)
    {
        DBGMSG;
        return;
    }
    WDFunc::SetLBLText(this, lblname,pc.PrbMessage + QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(size);
}

void MainWindow::SetProgressBar(QString prbnum, quint32 cursize)
{
    QString prbname = "prb"+prbnum+"prb";
    QString lblname = "prb"+prbnum+"lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb != 0)
    {
        prb->setValue(cursize);
        WDFunc::SetLBLText(this, lblname, pc.PrbMessage + QString::number(cursize) + " из " + QString::number(prb->maximum()));
    }
}

void MainWindow::ShowCOMConnectDialog()
{
    int i;
    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QList<QSerialPortInfo> infolist = QSerialPortInfo::availablePorts();
    if (infolist.size() == 0)
    {
        QLabel *lbl = new QLabel("Ошибка, в системе нет последовательных портов");
        lyout->addWidget(lbl);
        pc.ErMsg(USB_NOCOMER);
    }
    else
    {
        QComboBox *portscb = new QComboBox;
        connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetPortSlot(QString)));
        QList<QSerialPortInfo> infolist = QSerialPortInfo::availablePorts();
        QStringListModel *tmpmodel = new QStringListModel;
        QStringList tmpsl;
        for (i = 0; i < infolist.size(); i++)
            tmpsl << infolist.at(i).portName();
        tmpmodel->setStringList(tmpsl);
        portscb->setModel(tmpmodel);
        lyout->addWidget(portscb);
    }
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
#endif
void MainWindow::GetAbout()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *l2yout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);
    QLabel *lbl = new QLabel(tmps);
    l2yout->addWidget(lbl);
    lbl = new QLabel("ООО \"АВМ-Энерго\"");
    l2yout->addWidget(lbl);
    lbl = new QLabel("2015-2017 гг.");
    l2yout->addWidget(lbl);
    l2yout->addStretch(10);
    lbl = new QLabel;
    QPixmap pmp;
    pmp.load("images/evel.png");
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
#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::Disconnect()
{
    if (!pc.Emul)
        cn->Disconnect();
}

void MainWindow::SetUSBDev()
{
    QString rbname = sender()->objectName();
    QStringList sl = rbname.split(".");
    if (sl.size() < 3)
    {
        DBGMSG;
        return;
    }
    QString tmps = sl.at(0);
    pc.DeviceInfo.vendor_id = tmps.toInt(nullptr, 16);
    tmps = sl.at(1);
    pc.DeviceInfo.product_id = tmps.toInt(nullptr, 16);
    tmps = sl.at(2);
    tmps.toWCharArray(pc.DeviceInfo.serial);
}
#endif
void MainWindow::DisconnectAndClear()
{
#if PROGSIZE != PROGSIZE_EMUL
    Disconnect();
#endif
#ifndef MODULE_A1
    OscD = 0;
#endif
    TuneB = TuneM = 0;
    CheckB = CheckM = 0;
    emit FinishAll();
    emit ClearBsi();
    ClearTW();
    MyTabWidget *MainTW = this->findChild<MyTabWidget *>("maintw");
    if (MainTW == 0)
        return;
    MainTW->hide();
    pc.Emul = false;
}
#ifndef MODULE_A1
void MainWindow::LoadOsc()
{
    pc.LoadFile(this, "Oscillogram files (*.osc)", OscFunc->BA);
    OscFunc->ProcessOsc();
}
#endif
void MainWindow::ShowUSBConnectDialog()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QDialog *dlg = new QDialog(this);
    QVBoxLayout *lyout = new QVBoxLayout;
/*    s_tqTableView *tv = new s_tqTableView;
    ETableModel *mdl = new ETableModel; */
    struct hid_device_info *devs, *cur_dev;

    pc.DeviceInfo.vendor_id = 0;
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    lyout->addWidget(WDFunc::NewLBLT(this, "Найдены следующие устройства"));
/*    mdl->addColumn("ИД производителя");
    mdl->addColumn("ИД устройства");
    mdl->addColumn("Путь");
    mdl->addColumn("Серийный номер");
    mdl->addColumn("Производитель");
    mdl->addColumn("Устройство");
    QVector<QVector<QVariant> > lsl;
    QVector<QVariant> sl[6]; */
    while (cur_dev)
    {
        QString venid = QString::number(cur_dev->vendor_id, 16);
        QString prodid = QString::number(cur_dev->product_id, 16);
        QString sn = QString::fromWCharArray(cur_dev->serial_number);
        QString tmps = "VEN_" + venid + " & DEV_" + prodid + " & SN_" + sn;
        QString rbname = venid + "." + prodid + "." + sn;
        QRadioButton *rb = WDFunc::NewRB(this, tmps, rbname);
        connect(rb,SIGNAL(clicked(bool)),this,SLOT(SetUSBDev()));
        lyout->addWidget(rb);
        cur_dev = cur_dev->next;
    }
/*    for (int i=0; i<6; ++i)
        lsl.append(sl[i]); */
    hid_free_enumeration(devs);
/*    mdl->fillModel(lsl);
    tv->setObjectName("devicetv");
    tv->setModel(mdl);
    connect(tv,SIGNAL(clicked(QModelIndex)),this,SLOT(GetDeviceFromTable(QModelIndex)));
    lyout->addWidget(tv); */
    QPushButton *pb = new QPushButton("Далее");
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    QApplication::restoreOverrideCursor();
    dlg->exec();
}

void MainWindow::GetDeviceFromTable(QModelIndex idx)
{
    Q_UNUSED(idx);
    s_tqTableView *tv = this->findChild<s_tqTableView *>("devicetv");
    if (tv == 0)
    {
        DBGMSG;
        return;
    }
    QString tmps = tv->model()->index(tv->currentIndex().row(), 0, QModelIndex()).data(Qt::DisplayRole).toString();
    pc.DeviceInfo.vendor_id = tmps.toInt(nullptr, 16);
//    quint16 vid = tmps.toInt(nullptr, 16);
    tmps = tv->model()->index(tv->currentIndex().row(), 1, QModelIndex()).data(Qt::DisplayRole).toString();
    pc.DeviceInfo.product_id = tmps.toInt(nullptr, 16);
//    quint16 pid = tmps.toInt(nullptr, 16);
    tmps = tv->model()->index(tv->currentIndex().row(), 3, QModelIndex()).data(Qt::DisplayRole).toString();
    tmps.toWCharArray(pc.DeviceInfo.serial);
}

#if PROGSIZE >= PROGSIZE_LARGE
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
#endif

void MainWindow::ShowErrorMessageBox(QString message)
{
    MessageBox2::error(this, "Ошибка", message);
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

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        pc.Cancelled = true;
    QMainWindow::keyPressEvent(e);
}

void MainWindow::SetPortSlot(QString port)
{
    pc.Port = port;
}
