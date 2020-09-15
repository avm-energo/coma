#include "eabstracttunedialog.h"
#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../usb/commands.h"
#include "../widgets/waitwidget.h"
#include "../widgets/wd_func.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTabWidget>
#include <QTableView>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

EAbstractTuneDialog::EAbstractTuneDialog(QWidget *parent) : QDialog(parent)
{
    TuneVariant = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    SetMeasurementEnabled(false);
    MeasurementTimer = new QTimer;
    MeasurementTimer->setInterval(MEASTIMERINT);
    IsNeededDefConf = false;
    connect(MeasurementTimer, SIGNAL(timeout()), this, SLOT(MeasTimerTimeout()));
    RepModel = new ReportModel;
}

EAbstractTuneDialog::~EAbstractTuneDialog() { }

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

    connect(pb, SIGNAL(clicked()), this, SLOT(StartTune()));

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
        QLabel *lbl = new QLabel(lbls.at(i));
        lbl->setVisible(false);
        lbl->setObjectName("tunemsg" + QString::number(i));
        hlyout->addWidget(lbl);
        lbl = new QLabel("");
        lbl->setVisible(false);
        lbl->setObjectName("tunemsgres" + QString::number(i));
        hlyout->addWidget(lbl);
        hlyout->addStretch(1);
        w2lyout->addLayout(hlyout);
    }
    w2lyout->addStretch(10);
    w2->setLayout(w2lyout);
    area->setWidget(w2);
    lyout->addWidget(area);
    area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
    QLabel *lbl = new QLabel("Настройка завершена!");
    lbl->setVisible(false);
    lbl->setObjectName("tunemsg" + QString::number(i));
    lyout->addWidget(lbl);
    //    lyout->addStretch(1);
    w->setLayout(lyout);
    return w;
}

QWidget *EAbstractTuneDialog::BottomUI(int bacnum)
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Установить настроечные коэффициенты по умолчанию");
    pb->setObjectName(QString::number(bacnum));
    connect(pb, SIGNAL(clicked()), this, SLOT(SetDefCoefs()));
    lyout->addWidget(pb);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString tmps = "Прочитать настроечные коэффициенты из ";
    tmps += ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    pb = new QPushButton(tmps);
    pb->setObjectName(QString::number(bacnum));

    connect(pb, SIGNAL(clicked()), this, SLOT(ReadTuneCoefs()));

    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    tmps = "Записать настроечные коэффициенты в ";
    tmps += ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    pb = new QPushButton(tmps);
    pb->setObjectName(QString::number(bacnum));

    connect(pb, SIGNAL(clicked()), this, SLOT(WriteTuneCoefsSlot()));

    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setIcon(QIcon("../load.png"));
    pb->setObjectName(QString::number(bacnum));
    connect(pb, SIGNAL(clicked()), this, SLOT(LoadFromFile()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon("../save.png"));
    pb->setObjectName(QString::number(bacnum));
    connect(pb, SIGNAL(clicked()), this, SLOT(SaveToFile()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return w;
}

void EAbstractTuneDialog::SetBac(void *block, int blocknum, int blocksize)
{
    BacStruct Bac;
    Bac.BacBlock = block;
    Bac.BacBlockSize = blocksize;
    AbsBac[blocknum] = Bac;
}

void EAbstractTuneDialog::ShowTable()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTableView *tw = new QTableView;
    tw->setModel(RepModel);
    tw->resizeColumnsToContents();
    tw->resizeRowsToContents();
    lyout->addWidget(tw);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb, SIGNAL(clicked(bool)), dlg, SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
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
    connect(w, SIGNAL(CountZero()), &el, SLOT(quit()));
    w->Start();
    el.exec();
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
    if (SaveAllTuneCoefs() == NOERROR)
        TuneFileSaved = true;
    else
        TuneFileSaved = false;
    ReadAllTuneCoefs();
    //    MeasurementTimer->start();
    StdFunc::ClearCancel();
    Skipped = false;
    MsgClear(); // очистка экрана с сообщениями
    for (bStep = 0; bStep < lbls.size(); ++bStep)
    {
        //        WaitNSeconds(2);
        MsgSetVisible(bStep);
        int res = (this->*pf[lbls.at(bStep)])();
        if ((res == GENERALERROR) || (StdFunc::IsCancelled()))
        {
            ErMsgSetVisible(bStep);
            WDFunc::SetEnabled(this, "starttune", true);
            WARNMSG(lbls.at(bStep));
            //           MeasurementTimer->stop();
            return;
        }
        else if (res == RESEMPTY)
            SkMsgSetVisible(bStep);
        else
            OkMsgSetVisible(bStep);
    }
    MsgSetVisible(bStep); // выдаём надпись "Настройка завершена!"
    //    MeasurementTimer->stop();
    WDFunc::SetEnabled(this, "starttune", true);
    QMessageBox::information(this, "Готово", "Настройка завершена!");
}

int EAbstractTuneDialog::CheckPassword()
{
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg, SIGNAL(Finished(QString)), this, SLOT(PasswordCheck(QString)));
    connect(this, SIGNAL(PasswordChecked()), &PasswordLoop, SLOT(quit()));
    dlg->show();
    PasswordLoop.exec();
    if (!ok)
    {
        QMessageBox::critical(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

bool EAbstractTuneDialog::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number - base);
    if (tmpf < fabs(threshold))
        return true;
    else
    {
        QMessageBox::critical(this, "Ошибка",
            "Ошибочное значение: должно быть " + QString::number(base, 'f', 5) + "±"
                + QString::number(threshold, 'f', 5) + ", а получили: " + QString::number(number, 'f', 5));
        return false;
    }
}

void EAbstractTuneDialog::MsgSetVisible(int msg, bool Visible)
{
    WDFunc::SetVisible(this, "tunemsg" + QString::number(msg), Visible);
}

void EAbstractTuneDialog::OkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap("images/ok.png");
    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
}

void EAbstractTuneDialog::ErMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap("images/cross.png");
    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
}

void EAbstractTuneDialog::SkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap("images/hr.png");
    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
}

void EAbstractTuneDialog::MsgClear()
{
    int i;
    for (i = 0; i < lbls.size(); ++i)
    {
        MsgSetVisible(i, false);
        OkMsgSetVisible(i, false);
    }
    MsgSetVisible(i, false);
}

int EAbstractTuneDialog::StartMeasurement()
{
    MeasurementTimer->start();
    SetMeasurementEnabled(true);
    while (MeasurementEnabled && !StdFunc::IsCancelled())
        TimeFunc::Wait();
    MeasurementTimer->stop();
    if (StdFunc::IsCancelled())
        return GENERALERROR;
    return NOERROR;
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
        StdFunc::Cancel();
    else if (psw == "121941")
        ok = true;
    emit PasswordChecked();
}

void EAbstractTuneDialog::TuneReadCoefs(int index)
{
    Q_UNUSED(index);
    /*if(!MainWindow::TheEnd)
    {
        if(index == TuneIndex)
        {
            if(TimeIndex)
            emit stopRead(TimeIndex);

            if(CheckIndex)
            emit stopRead(CheckIndex);*/

    TimeFunc::Wait(100);

    for (int i = 0; i < AbsBac.keys().size(); i++)
    {
        int bacnum;
        if (Board::GetInstance()->typeM() == 135)
            bacnum = i + 1;
        else
            bacnum = i + 2;
        if (AbsBac.keys().contains(bacnum))
        {
            ReadTuneCoefsByBac(bacnum);
            TimeFunc::Wait(100);
        }
    }
    //  }
    // }
}

void EAbstractTuneDialog::ReadTuneCoefs()
{
    int bacnum = sender()->objectName().toInt();
    ReadTuneCoefsByBac(bacnum);
}

void EAbstractTuneDialog::ReadTuneCoefsByBac(int bacnum)
{
    if (AbsBac.keys().contains(bacnum))
    {
        int res = Commands::GetBac(bacnum, AbsBac[bacnum].BacBlock, AbsBac[bacnum].BacBlockSize);
        if (res == NOERROR)
            FillBac(bacnum);
    }
}

bool EAbstractTuneDialog::WriteTuneCoefsSlot()
{
    int bacnum = sender()->objectName().toInt();
    if (CheckPassword() != NOERROR)
        return false;
    // FillBackBac(bacnum);
    // return WriteTuneCoefs(bacnum);

    if (QMessageBox::question(this, "Вопрос",
            "Сохранить регулировочные коэффициенты?\n(Результаты предыдущей регулировки будут потеряны)")
        == false)
        return false;

    for (int i = 0; i < AbsBac.keys().size(); i++)
    {
        if (Board::GetInstance()->typeM() == 135)
            bacnum = i + 1;
        else
            bacnum = i + 2;

        if (AbsBac.keys().contains(bacnum))
        {
            m_VModel->updateFromModel();
            //            FillBackBac(bacnum);

            if (WriteTuneCoefs(bacnum) != true)
                return false;

            TimeFunc::Wait(100);
        }
    }

    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    QMessageBox::information(this, "Внимание", "Коэффициенты переданы в " + tmps + " успешно!");

    return true;
}

bool EAbstractTuneDialog::WriteTuneCoefs(int bacnum)
{
    // if (QMessageBox::question(this, "Вопрос", "Сохранить регулировочные коэффициенты?\n(Результаты предыдущей
    // регулировки будут потеряны)") == false)
    //    return false;
    // QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    if (AbsBac.keys().contains(bacnum))
    {
        if (Commands::WriteBac(bacnum, AbsBac[bacnum].BacBlock, AbsBac[bacnum].BacBlockSize) == NOERROR)
        {
            // QMessageBox::information(this, "Внимание", "Коэффициенты переданы в " + tmps + " успешно!");
            return true;
        }
    }
    // QMessageBox::critical(this, "Ошибка", "Ошибка записи коэффициентов в " + tmps + "!");
    return false;
}

int EAbstractTuneDialog::SaveAllTuneCoefs()
{
    QString tunenum;
    for (QMap<int, BacStruct>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        tunenum = QString::number(it.key(), 16); // key is the number of Bac block
        QByteArray ba;
        ba.resize(it.value().BacBlockSize);
        memcpy(&(ba.data()[0]), it.value().BacBlock, it.value().BacBlockSize);
        if (Files::SaveToFile(StdFunc::GetSystemHomeDir() + "temptune.tn" + tunenum, ba, it.value().BacBlockSize)
            != NOERROR)
            return GENERALERROR;
    }
    return NOERROR;
}

void EAbstractTuneDialog::PrereadConf()
{
    IsNeededDefConf = (ModuleBSI::PrereadConf(this, &S2Config) == RESEMPTY) ? true : false;
}

void EAbstractTuneDialog::SaveToFileEx(int bacnum)
{
    int res = NOERROR;
    QString tunenum = QString::number(bacnum, 16);
    if (!AbsBac.keys().contains(bacnum))
    {
        QMessageBox::critical(this, "Ошибка", "Блок Bac с индексом " + tunenum + " не найден!");
        return;
    }
    QByteArray ba;
    ba.resize(AbsBac[bacnum].BacBlockSize);
    memcpy(&(ba.data()[0]), AbsBac[bacnum].BacBlock, AbsBac[bacnum].BacBlockSize);
    res = Files::SaveToFile(Files::ChooseFileForSave(this, "Tune files (*.tn" + tunenum + ")", "tn" + tunenum), ba,
        AbsBac[bacnum].BacBlockSize);
    switch (res)
    {
    case Files::ER_NOERROR:
        QMessageBox::information(this, "Внимание", "Файл коэффициентов записан успешно!");
        break;
    case Files::ER_FILEWRITE:
        QMessageBox::critical(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case Files::ER_FILENAMEEMP:
        QMessageBox::critical(this, "Ошибка", "Пустое имя файла!");
        break;
    case Files::ER_FILEOPEN:
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void EAbstractTuneDialog::SaveToFile()
{
    int bacnum = sender()->objectName().toInt();
    //    FillBackBac(bacnum);
    m_VModel->updateFromModel();
    SaveToFileEx(bacnum);
}

void EAbstractTuneDialog::SetMeasurementEnabled(bool enabled)
{
    if (enabled)
    {
        WDFunc::SetEnabled(this, "Good", true);
        WDFunc::SetEnabled(this, "NoGood", true);
        MeasurementEnabled = true;
    }
    else
    {
        WDFunc::SetEnabled(this, "Good", false);
        WDFunc::SetEnabled(this, "NoGood", false);
        MeasurementEnabled = false;
    }
}

void EAbstractTuneDialog::LoadFromFile()
{
    int bacnum = sender()->objectName().toInt();
    QByteArray ba;
    ba.resize(MAXTUNESIZE);
    QString tunenum = QString::number(bacnum, 16);
    if (!AbsBac.keys().contains(bacnum))
    {
        QMessageBox::critical(this, "Ошибка", "Блок Bac с индексом " + tunenum + " не найден!");
        return;
    }
    int res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Tune files (*.tn" + tunenum + ")"), ba);
    if (res != Files::ER_NOERROR)
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка при загрузке файла");
        return;
    }
    memcpy(AbsBac[bacnum].BacBlock, &(ba.data()[0]), ba.size());
    FillBac(bacnum);
    QMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
}

void EAbstractTuneDialog::Good() { SetMeasurementEnabled(false); }

void EAbstractTuneDialog::NoGood()
{
    SetMeasurementEnabled(false);
    StdFunc::Cancel();
}

void EAbstractTuneDialog::CancelTune()
{
    StdFunc::Cancel();
    emit Finished();
}

void EAbstractTuneDialog::ReadAllTuneCoefs() { }

void EAbstractTuneDialog::MeasTimerTimeout()
{
    if (MeasurementEnabled)
        GetBdAndFill();
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
        StdFunc::Cancel();
    QDialog::keyPressEvent(e);
}
