#include <QTime>
#include <QtMath>
#include <QTabWidget>
#include <QEventLoop>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QInputDialog>
#include <QCoreApplication>
#include <QFileDialog>
#include "abstracttunedialog.h"
#include "../publicclass.h"
#include "../canal.h"
#include "../dialogs/keypressdialog.h"
#include "../widgets/waitwidget.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

AbstractTuneDialog::AbstractTuneDialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    MeasurementEnabled = false;
    MeasurementTimer = new QTimer;
    MeasurementTimer->setInterval(MEASTIMERINT);
    connect(MeasurementTimer,SIGNAL(timeout()),this,SLOT(MeasTimerTimeout()));
}

QWidget *AbstractTuneDialog::TuneUI()
{
    // CP1 - НАСТРОЙКА ПРИБОРА
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Начать настройку");
    pb->setObjectName("starttune");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    for (int i = 0; i < lbls.size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        QLabel *lbl=new QLabel(lbls.at(i));
        lbl->setVisible(false);
        lbl->setObjectName("tunemsg"+QString::number(i));
        hlyout->addWidget(lbl);
        lbl=new QLabel("");
        lbl->setVisible(false);
        lbl->setObjectName("tunemsgres"+QString::number(i));
        hlyout->addWidget(lbl);
        hlyout->addStretch(1);
        lyout->addLayout(hlyout);
    }
    lyout->addStretch(1);
    w->setLayout(lyout);
    return w;
}

QWidget *AbstractTuneDialog::BottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Установить настроечные коэффициенты по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefCoefs()));
    lyout->addWidget(pb);
    pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setIcon(QIcon(":/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    lyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon(":/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    lyout->addWidget(pb);
    w->setLayout(lyout);
    return w;
}

void AbstractTuneDialog::ProcessTune()
{
    if (lbls.size() > pf.size())
    {
        ERMSG("lbls size > pf size");
        return;
    }
    ReadTuneCoefs();
    MeasurementTimer->start();
    Cancelled = Skipped = false;
    MsgClear(); // очистка экрана с сообщениями
    for (int i=0; i<lbls.size(); ++i)
    {
        MsgSetVisible(i);
        int res = (this->*pf[lbls.at(i)])();
        if ((res == GENERALERROR) || (Cancelled))
        {
            ErMsgSetVisible(i);
            WDFunc::SetEnabled(this, "starttune", true);
            WARNMSG(lbls.at(i));
            MeasurementTimer->stop();
            return;
        }
        else if (res == ER_RESEMPTY)
            SkMsgSetVisible(i);
        else
            OkMsgSetVisible(i);
    }
    MeasurementTimer->stop();
}

int AbstractTuneDialog::CheckPassword()
{
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc"));
    connect(dlg,SIGNAL(Cancelled()),this,SLOT(CancelTune()));
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(PasswordCheck(QString)));
    connect(this,SIGNAL(PasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->setLayout(vlyout);
    dlg->show();
    PasswordLoop.exec();
    if (!ok)
    {
        MessageBox2::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

void AbstractTuneDialog::SetBac(void *block, int blocknum, int blocksize)
{
    AbsBac.BacBlock = block;
    AbsBac.BacBlockSize = blocksize;
    AbsBac.BacBlockNum = blocknum;
}

bool AbstractTuneDialog::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
    {
        MessageBox2::error(this, "Ошибка", "Ошибочное значение: должно быть "+QString::number(base, 'f', 5) + \
                           ", а получили: "+QString::number(number, 'f', 5));
        return false;
    }
}

void AbstractTuneDialog::MsgSetVisible(int msg, bool Visible)
{
    WDFunc::SetVisible(this, "tunemsg"+QString::number(msg), Visible);
}

void AbstractTuneDialog::OkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/ok.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void AbstractTuneDialog::ErMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/cross.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void AbstractTuneDialog::SkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/hr.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void AbstractTuneDialog::MsgClear()
{
    for (int i=0; i<lbls.size(); ++i)
    {
        MsgSetVisible(i, false);
        OkMsgSetVisible(i, false);
    }
}

void AbstractTuneDialog::WaitNSeconds(int Seconds)
{
    SecondsToEnd15SecondsInterval = Seconds;
    WaitWidget *w = new WaitWidget;
    QTimer *tmr = new QTimer;
    tmr->setInterval(1000);
    connect(tmr,SIGNAL(timeout()),this,SLOT(UpdateNSecondsWidget()));
    connect(this,SIGNAL(SecondsRemaining(QString)),w,SLOT(SetMessage(QString)));
    tmr->start();
    w->Start();
    while (SecondsToEnd15SecondsInterval > 0)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < 20)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    tmr->stop();
    w->Stop();
}

void AbstractTuneDialog::SaveToFileEx()
{
    int res = NOERROR;
    QString tunenum = QString::number(AbsBac.BacBlockNum, 16);
    QByteArray ba;
    ba.resize(AbsBac.BacBlockSize);
    memcpy(&(ba.data()[0]), &AbsBac.BacBlock, AbsBac.BacBlockSize);
    res = pc.SaveFile(this, "Tune files (*.tn"+tunenum+")", "tn"+tunenum, ba, AbsBac.BacBlockSize);
    switch (res)
    {
    case NOERROR:
        MessageBox2::information(this, "Внимание", "Записано успешно!");
        break;
    case ER_FILEWRITE:
        MessageBox2::error(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case ER_FILENAMEEMP:
        MessageBox2::error(this, "Ошибка", "Пустое имя файла!");
        break;
    case ER_FILEOPEN:
        MessageBox2::error(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

int AbstractTuneDialog::StartMeasurement()
{
    MeasurementEnabled = true;
    while (MeasurementEnabled && !Cancelled)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < SLEEPINT)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    if (Cancelled)
        return GENERALERROR;
    return NOERROR;
}

// ####################### SLOTS #############################

void AbstractTuneDialog::StartTune()
{
    WDFunc::SetEnabled(this, "starttune", false);
    ProcessTune();
}

void AbstractTuneDialog::PasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit PasswordChecked();
}

void AbstractTuneDialog::ReadTuneCoefs()
{
    cn->Send(CN_GBac, AbsBac.BacBlockNum, AbsBac.BacBlock, AbsBac.BacBlockSize);
    if (cn->result == NOERROR)
        FillBac();
}

void AbstractTuneDialog::WriteTuneCoefs()
{
    FillBackBac();
    cn->Send(CN_WBac, AbsBac.BacBlockNum, AbsBac.BacBlock, AbsBac.BacBlockSize);
    if (cn->result == NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Записано успешно!");
        return;
    }
    MessageBox2::error(this, "Ошибка", "Ошибка записи коэффициентов в модуль!");
}

void AbstractTuneDialog::SaveToFile()
{
    FillBackBac();
    SaveToFileEx();
}

void AbstractTuneDialog::LoadFromFile()
{
    QByteArray ba;
    ba.resize(MAXTUNESIZE);
    int res = pc.LoadFile(this, "Tune files (*.tn)", ba);
    if (res != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Ошибка при загрузке файла");
        return;
    }
    memcpy(AbsBac.BacBlock,&(ba.data()[0]),ba.size());
    FillBac();
    MessageBox2::information(this, "Внимание", "Загрузка прошла успешно!");
}

void AbstractTuneDialog::Good()
{
    MeasurementEnabled = false;
}

void AbstractTuneDialog::NoGood()
{
    Cancelled = true;
    MeasurementEnabled = false;
}

void AbstractTuneDialog::CancelTune()
{
    Cancelled = true;
}

void AbstractTuneDialog::UpdateNSecondsWidget()
{
    QString tmps = "Подождите " + QString::number(--SecondsToEnd15SecondsInterval) + " с";
    emit SecondsRemaining(tmps);
}

void AbstractTuneDialog::MeasTimerTimeout()
{
    if (MeasurementEnabled)
    {
        WDFunc::SetEnabled(this, "GoodDN", true);
        WDFunc::SetEnabled(this, "NoGoodDN", true);
        GetBdAndFillMTT();
    }
    else
    {
        WDFunc::SetEnabled(this, "GoodDN", false);
        WDFunc::SetEnabled(this, "NoGoodDN", false);
    }
}

// ##################### PROTECTED ####################

void AbstractTuneDialog::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}
