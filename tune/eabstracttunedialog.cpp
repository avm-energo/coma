#include <QTime>
#include <QTimer>
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
#include "../gen/stdfunc.h"
#include "../gen/maindef.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif
#include "../gen/files.h"
#include "../gen/timefunc.h"
#include "../gen/error.h"
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
#if PROGSIZE != PROGSIZE_EMUL
    connect(MeasurementTimer,SIGNAL(timeout()),this,SLOT(MeasTimerTimeout()));
#endif
}

EAbstractTuneDialog::~EAbstractTuneDialog()
{

}

QWidget *EAbstractTuneDialog::TuneUI()
{
    lbls.clear();
    pf.clear();
#if PROGSIZE != PROGSIZE_EMUL
    SetLbls();
    SetPf();
#endif
    int i;
    // CP1 - НАСТРОЙКА ПРИБОРА/МОДУЛЯ
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Начать настройку");
    pb->setObjectName("starttune");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
#endif
    if (StdFunc::IsInEmulateMode())
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
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    tmps = "Записать настроечные коэффициенты в ";
    tmps += ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    pb = new QPushButton(tmps);
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefsSlot()));
#endif
    if (StdFunc::IsInEmulateMode())
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

void EAbstractTuneDialog::SetBac(void *block, int blocknum, int blocksize)
{
    AbsBac.BacBlock = block;
    AbsBac.BacBlockSize = blocksize;
    AbsBac.BacBlockNum = blocknum;
}

#if PROGSIZE != PROGSIZE_EMUL
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
    if (Files::SaveToFile(StdFunc::GetSystemHomeDir()+"temptune.tn"+tunenum, ba, AbsBac.BacBlockSize) == Error::ER_NOERROR)
        TuneFileSaved = true;
    else
        TuneFileSaved = false;
    ReadTuneCoefs();
    MeasurementTimer->start();
    Cancelled = Skipped = false;
    MsgClear(); // очистка экрана с сообщениями
    for (bStep=0; bStep<lbls.size(); ++bStep)
    {
        WaitNSeconds(2);
        MsgSetVisible(bStep);
        int res = (this->*pf[lbls.at(bStep)])();
        if ((res == Error::ER_GENERALERROR) || (Cancelled))
        {
            ErMsgSetVisible(bStep);
            WDFunc::SetEnabled(this, "starttune", true);
            WARNMSG(lbls.at(bStep));
            MeasurementTimer->stop();
            return;
        }
        else if (res == Error::ER_RESEMPTY)
            SkMsgSetVisible(bStep);
        else
            OkMsgSetVisible(bStep);

    }
    MsgSetVisible(bStep); // выдаём надпись "Настройка завершена!"
    MeasurementTimer->stop();
    WDFunc::SetEnabled(this, "starttune", true);
    EMessageBox::information(this, "Готово", "Настройка завершена!");
}

int EAbstractTuneDialog::CheckPassword()
{
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(PasswordCheck(QString)));
    connect(this,SIGNAL(PasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->show();
    PasswordLoop.exec();
    if (!ok)
    {
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}

bool EAbstractTuneDialog::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
    {
        EMessageBox::error(this, "Ошибка", "Ошибочное значение: должно быть "+QString::number(base, 'f', 5) + \
                           "±"+QString::number(threshold, 'f', 5)+", а получили: "+QString::number(number, 'f', 5));
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

void EAbstractTuneDialog::WaitNSeconds(int Seconds, bool isAllowedToStop)
{
    WaitWidget *w = new WaitWidget;
    WaitWidget::ww_struct ww;
    ww.isincrement = false;
    ww.isallowedtostop = isAllowedToStop;
    ww.format = WaitWidget::WW_TIME;
    ww.initialseconds = Seconds;
    w->Init(ww);
    QEventLoop el;
    connect(w, SIGNAL(CountZero()), &el,SLOT(quit()));
    w->Start();
    el.exec();
}

int EAbstractTuneDialog::StartMeasurement()
{
    MeasurementEnabled = true;
    while (MeasurementEnabled && !Cancelled)
        TimeFunc::Wait();
    if (Cancelled)
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
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

void EAbstractTuneDialog::PasswordCheck(QString psw)
{
    ok = false;
    if (psw.isEmpty())
        Cancelled = true;
    else if (psw == "121941")
        ok = true;
    emit PasswordChecked();
}

void EAbstractTuneDialog::ReadTuneCoefs()
{
    if (Commands::GetBac(AbsBac.BacBlockNum, AbsBac.BacBlock, AbsBac.BacBlockSize) == Error::ER_NOERROR)
        FillBac();
}

bool EAbstractTuneDialog::WriteTuneCoefsSlot()
{
    //if (CheckPassword() != Error::ER_NOERROR)   На время отладки!!!
    //    return false;
    return WriteTuneCoefs();
}

bool EAbstractTuneDialog::WriteTuneCoefs()
{
    if (EMessageBox::question(this, "Вопрос", "Сохранить регулировочные коэффициенты?\n(Результаты предыдущей регулировки будут потеряны)") == false)
        return Error::ER_GENERALERROR;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    FillBackBac();
    if (Commands::WriteBac(AbsBac.BacBlockNum, AbsBac.BacBlock, AbsBac.BacBlockSize) == Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Коэффициенты переданы в " + tmps + " успешно!");
        return true;
    }
    EMessageBox::error(this, "Ошибка", "Ошибка записи коэффициентов в " + tmps + "!");
    return false;
}

void EAbstractTuneDialog::PrereadConf()
{
    if ((ModuleBSI::Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации, заполнить поля по умолчанию
      IsNeededDefConf = true; // emit LoadDefConf();
    else // иначе заполнить значениями из модуля
    {
        if ((Commands::GetFile(1, &S2Config)) != Error::ER_NOERROR)
        {
            QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля " : "прибора ");
            EMessageBox::error(this, "ошибка", "Ошибка чтения конфигурации из " + tmps);
        }
    }
}
#endif

void EAbstractTuneDialog::SaveToFileEx()
{
    int res = Error::ER_NOERROR;
    QString tunenum = QString::number(AbsBac.BacBlockNum, 16);
    QByteArray ba;
    ba.resize(AbsBac.BacBlockSize);
    memcpy(&(ba.data()[0]), AbsBac.BacBlock, AbsBac.BacBlockSize);
    res = Files::SaveToFile(Files::ChooseFileForSave(this, "Tune files (*.tn"+tunenum+")", "tn"+tunenum), ba, AbsBac.BacBlockSize);
    switch (res)
    {
    case Files::ER_NOERROR:
        EMessageBox::information(this, "Внимание", "Файл коэффициентов записан успешно!");
        break;
    case Files::ER_FILEWRITE:
        EMessageBox::error(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case Files::ER_FILENAMEEMP:
        EMessageBox::error(this, "Ошибка", "Пустое имя файла!");
        break;
    case Files::ER_FILEOPEN:
        EMessageBox::error(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
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
    int res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Tune files (*.tn"+tunenum+")"), ba);
    if (res != Files::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка при загрузке файла");
        return;
    }
    memcpy(AbsBac.BacBlock,&(ba.data()[0]),ba.size());
    FillBac();
    EMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
}

#if PROGSIZE != PROGSIZE_EMUL
void EAbstractTuneDialog::Good()
{
    MeasurementEnabled = false;
}

void EAbstractTuneDialog::NoGood()
{
    Cancelled = true;
    MeasurementEnabled = false;
}

void EAbstractTuneDialog::CancelTune()
{
    Cancelled = true;
}

/*void EAbstractTuneDialog::UpdateNSecondsWidget()
{
//    emit SecondsRemaining(--SecondsToEnd15SecondsInterval);
    --SecondsToEnd15SecondsInterval;
} */

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
#endif
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
        Cancelled = true;
    QDialog::keyPressEvent(e);
}
