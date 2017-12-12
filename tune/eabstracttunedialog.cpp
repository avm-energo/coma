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
#include <QScrollArea>
#include <QScrollBar>
#include "eabstracttunedialog.h"
#include "../gen/commands.h"
#include "../dialogs/keypressdialog.h"
#include "../widgets/waitwidget.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

EAbstractTuneDialog::EAbstractTuneDialog(QWidget *parent) :
    QDialog(parent)
{
    TuneVariant = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    MeasurementEnabled = false;
    MeasurementTimer = new QTimer;
    MeasurementTimer->setInterval(MEASTIMERINT);
    connect(MeasurementTimer,SIGNAL(timeout()),this,SLOT(MeasTimerTimeout()));
}

QWidget *EAbstractTuneDialog::TuneUI()
{
    lbls.clear();
    pf.clear();
    SetLbls();
    SetPf();
    int i;
    // CP1 - НАСТРОЙКА ПРИБОРА/МОДУЛЯ
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Начать настройку");
    pb->setObjectName("starttune");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
    if (pc.Emul)
        pb->setEnabled(false);
    else
        pb->setEnabled(true);
    lyout->addWidget(pb);
    QScrollArea *area = new QScrollArea;
    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);
    QWidget *w2 = new QWidget;
    QVBoxLayout *w2lyout = new QVBoxLayout;
    for (i = 0; i < lbls.size(); ++i)
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
        w2lyout->addLayout(hlyout);
    }
    w2lyout->addStretch(10);
    w2->setLayout(w2lyout);
    area->setWidget(w2);
    lyout->addWidget(area);
    area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
    QLabel *lbl=new QLabel("Настройка завершена!");
    lbl->setVisible(false);
    lbl->setObjectName("tunemsg"+QString::number(i));
    lyout->addWidget(lbl);
//    lyout->addStretch(1);
    w->setLayout(lyout);
    return w;
}

QWidget *EAbstractTuneDialog::BottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Установить настроечные коэффициенты по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefCoefs()));
    lyout->addWidget(pb);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString tmps = "Прочитать настроечные коэффициенты из ";
    tmps += ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    pb = new QPushButton(tmps);
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    tmps = "Записать настроечные коэффициенты в ";
    tmps += ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    pb = new QPushButton(tmps);
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setIcon(QIcon("../load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon("../save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return w;
}

void EAbstractTuneDialog::ProcessTune()
{
    if (lbls.size() > pf.size())
    {
        ERMSG("lbls size > pf size");
        WDFunc::SetEnabled(this, "starttune", true);
        return;
    }
    // сохраняем на всякий случай настроечные коэффициенты
    QString tunenum = QString::number(AbsBac.BacBlockNum, 16);
    QByteArray ba;
    ba.resize(AbsBac.BacBlockSize);
    memcpy(&(ba.data()[0]), AbsBac.BacBlock, AbsBac.BacBlockSize);
    if (pc.SaveToFile(pc.SystemHomeDir+"temptune.tn"+tunenum, ba, AbsBac.BacBlockSize) == NOERROR)
        TuneFileSaved = true;
    else
        TuneFileSaved = false;
    ReadTuneCoefs();
    MeasurementTimer->start();
    pc.Cancelled = Skipped = false;
    MsgClear(); // очистка экрана с сообщениями
    for (bStep=0; bStep<lbls.size(); ++bStep)
    {
        MsgSetVisible(bStep);
        int res = (this->*pf[lbls.at(bStep)])();
        if ((res == GENERALERROR) || (pc.Cancelled))
        {
            ErMsgSetVisible(bStep);
            WDFunc::SetEnabled(this, "starttune", true);
            WARNMSG(lbls.at(bStep));
            MeasurementTimer->stop();
            return;
        }
        else if (res == ER_RESEMPTY)
            SkMsgSetVisible(bStep);
        else
            OkMsgSetVisible(bStep);
    }
    MsgSetVisible(bStep); // выдаём надпись "Настройка завершена!"
    MeasurementTimer->stop();
    WDFunc::SetEnabled(this, "starttune", true);
    MessageBox2::information(this, "Готово", "Настройка завершена!");
}

int EAbstractTuneDialog::CheckPassword()
{
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc"));
    connect(dlg,SIGNAL(Finished(QString &)),this,SLOT(PasswordCheck(QString &)));
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

void EAbstractTuneDialog::SetBac(void *block, int blocknum, int blocksize)
{
    AbsBac.BacBlock = block;
    AbsBac.BacBlockSize = blocksize;
    AbsBac.BacBlockNum = blocknum;
}

bool EAbstractTuneDialog::IsWithinLimits(double number, double base, double threshold)
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

void EAbstractTuneDialog::MsgSetVisible(int msg, bool Visible)
{
    WDFunc::SetVisible(this, "tunemsg"+QString::number(msg), Visible);
}

void EAbstractTuneDialog::OkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap("images/ok.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void EAbstractTuneDialog::ErMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap("images/cross.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void EAbstractTuneDialog::SkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap("images/hr.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void EAbstractTuneDialog::MsgClear()
{
    int i;
    for (i=0; i<lbls.size(); ++i)
    {
        MsgSetVisible(i, false);
        OkMsgSetVisible(i, false);
    }
    MsgSetVisible(i, false);
}

void EAbstractTuneDialog::WaitNSeconds(int Seconds)
{
    SecondsToEnd15SecondsInterval = Seconds;
    WaitWidget *w = new WaitWidget;
    QTimer *tmr = new QTimer;
    tmr->setInterval(1000);
    connect(tmr,SIGNAL(timeout()),this,SLOT(UpdateNSecondsWidget()));
//    connect(this,SIGNAL(SecondsRemaining(quint32)),w,SLOT(SetSeconds(quint32)));
    w->SetMessage("Пожалуйста, подождите...");
    w->SetSeconds(Seconds);
    tmr->start();
    w->Start();
    while ((SecondsToEnd15SecondsInterval > 0) && !pc.Cancelled)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < 20)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    tmr->stop();
    w->close();
}

void EAbstractTuneDialog::SaveToFileEx()
{
    int res = NOERROR;
    QString tunenum = QString::number(AbsBac.BacBlockNum, 16);
    QByteArray ba;
    ba.resize(AbsBac.BacBlockSize);
    memcpy(&(ba.data()[0]), AbsBac.BacBlock, AbsBac.BacBlockSize);
    res = pc.SaveFile(this, "Tune files (*.tn"+tunenum+")", "tn"+tunenum, ba, AbsBac.BacBlockSize);
    switch (res)
    {
    case NOERROR:
        MessageBox2::information(this, "Внимание", "Файл коэффициентов записан успешно!");
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

int EAbstractTuneDialog::StartMeasurement()
{
    MeasurementEnabled = true;
    while (MeasurementEnabled && !pc.Cancelled)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < SLEEPINT)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    if (pc.Cancelled)
        return GENERALERROR;
    return NOERROR;
}

void EAbstractTuneDialog::InputTuneVariant(int varnum)
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QStringList sl;
    for (int i=0; i<varnum; ++i)
        sl << QString::number(i+1);
    hlyout->addWidget(WDFunc::NewLBLT(this, "Выберите вариант использования"), 0);
    QComboBox *cb = WDFunc::NewCB(this, "tunevariantcb", sl);
    cb->setMinimumWidth(50);
    hlyout->addWidget(cb, 0);

    lyout->addLayout(hlyout);
    QPushButton *pb = new QPushButton("Подтвердить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetTuneVariant()));
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout = new QHBoxLayout;
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
}

// ####################### SLOTS #############################

void EAbstractTuneDialog::StartTune()
{
    WDFunc::SetEnabled(this, "starttune", false);
    ProcessTune();
}

void EAbstractTuneDialog::PasswordCheck(QString &psw)
{
    ok = true;
    if (psw.isEmpty())
        pc.Cancelled = true;
    else if (psw != "121941")
        ok = false;
    emit PasswordChecked();
}

void EAbstractTuneDialog::ReadTuneCoefs()
{
    if (Commands::GetBac(AbsBac.BacBlockNum, AbsBac.BacBlock, AbsBac.BacBlockSize) == NOERROR)
        FillBac();
}

bool EAbstractTuneDialog::WriteTuneCoefs()
{
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    FillBackBac();
    if (Commands::WriteBac(AbsBac.BacBlockNum, AbsBac.BacBlock, AbsBac.BacBlockSize) == NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Коэффициенты переданы в " + tmps + " успешно!");
        return true;
    }
    MessageBox2::error(this, "Ошибка", "Ошибка записи коэффициентов в " + tmps + "!");
    return false;
}

void EAbstractTuneDialog::SaveToFile()
{
    FillBackBac();
    SaveToFileEx();
}

void EAbstractTuneDialog::LoadFromFile()
{
    QByteArray ba;
    ba.resize(MAXTUNESIZE);
    QString tunenum = QString::number(AbsBac.BacBlockNum, 16);
    int res = pc.LoadFile(this, "Tune files (*.tn"+tunenum+")", ba);
    if (res != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Ошибка при загрузке файла");
        return;
    }
    memcpy(AbsBac.BacBlock,&(ba.data()[0]),ba.size());
    FillBac();
    MessageBox2::information(this, "Внимание", "Загрузка прошла успешно!");
}

void EAbstractTuneDialog::Good()
{
    MeasurementEnabled = false;
}

void EAbstractTuneDialog::NoGood()
{
    pc.Cancelled = true;
    MeasurementEnabled = false;
}

void EAbstractTuneDialog::CancelTune()
{
    pc.Cancelled = true;
}

void EAbstractTuneDialog::UpdateNSecondsWidget()
{
//    emit SecondsRemaining(--SecondsToEnd15SecondsInterval);
    --SecondsToEnd15SecondsInterval;
}

void EAbstractTuneDialog::MeasTimerTimeout()
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

void EAbstractTuneDialog::SetTuneVariant()
{
    if (!WDFunc::CBIndex(this, "tunevariantcb", TuneVariant))
        DBGMSG;
}

// ##################### PROTECTED ####################

void EAbstractTuneDialog::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}

void EAbstractTuneDialog::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        pc.Cancelled = true;
    QDialog::keyPressEvent(e);
}
