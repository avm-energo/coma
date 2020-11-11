#include "abstracttunedialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../usb/commands.h"
#include "../widgets/waitwidget.h"
#include "../widgets/wd_func.h"
#include "limereport/lrreportengine.h"

#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

AbstractTuneDialog::AbstractTuneDialog(QWidget *parent) : UDialog(parent)
{
    TuneVariant = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    SetMeasurementEnabled(false);
    MeasurementTimer = new QTimer;
    MeasurementTimer->setInterval(MEASTIMERINT);
    IsNeededDefConf = false;
    connect(MeasurementTimer, SIGNAL(timeout()), this, SLOT(MeasTimerTimeout()));
    RepModel = new ReportModel;
    //    m_ConfigCounter = 0;
    m_blockCount = 0;
}

AbstractTuneDialog::~AbstractTuneDialog()
{
}

void AbstractTuneDialog::SetupUI()
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    hlyout->addWidget(TuneUI());
    hlyout->addWidget(MainUI());
    vlyout->addLayout(hlyout);
    vlyout->addWidget(BottomUI());
}

QWidget *AbstractTuneDialog::TuneUI()
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
        hlyout->addWidget(WDFunc::NewLBL(this, lbls.at(i), "", "tunemsg" + QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBL(this, "", "", "tunemsgres" + QString::number(i)));
        hlyout->addStretch(1);
        w2lyout->addLayout(hlyout);
    }
    w2lyout->addStretch(10);
    w2->setLayout(w2lyout);
    area->setWidget(w2);
    lyout->addWidget(area);
    area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
    lyout->addWidget(WDFunc::NewLBL(this, "Настройка завершена!", "", "tunemsg" + QString::number(i)));
    for (int i = 0; i < lbls.size() + 1; ++i)
    {
        WDFunc::SetVisible(this, "tunemsg" + QString::number(i), false);
        WDFunc::SetVisible(this, "tunemsgres" + QString::number(i), false);
    }
    lyout->addWidget(WDFunc::NewPB2(this, "", "Прервать настройку", this, &AbstractTuneDialog::CancelTune));
    //    lyout->addStretch(1);
    w->setLayout(lyout);
    return w;
}

QWidget *AbstractTuneDialog::BottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Установить настроечные коэффициенты по умолчанию");
    //    pb->setObjectName(QString::number(bacnum));
    connect(pb, SIGNAL(clicked()), this, SLOT(SetDefCoefs()));
    lyout->addWidget(pb);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString tmps = "Прочитать настроечные коэффициенты из ";
    tmps += ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    pb = new QPushButton(tmps);
    //    pb->setObjectName(QString::number(bacnum));

    connect(pb, SIGNAL(clicked()), this, SLOT(ReadTuneCoefs()));

    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    tmps = "Записать настроечные коэффициенты в ";
    tmps += ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    pb = new QPushButton(tmps);
    //    pb->setObjectName(QString::number(bacnum));

    connect(pb, SIGNAL(clicked()), this, SLOT(WriteTuneCoefsSlot()));

    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setIcon(QIcon("../load.png"));
    //    pb->setObjectName(QString::number(bacnum));
    connect(pb, SIGNAL(clicked()), this, SLOT(LoadFromFile()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon("../save.png"));
    //    pb->setObjectName(QString::number(bacnum));
    connect(pb, SIGNAL(clicked()), this, SLOT(SaveToFile()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return w;
}

int AbstractTuneDialog::addDataBlock(
    DataBlock::DataBlockTypes type, const QString &caption, void *block, void *defblock, int blocknum, int blocksize)
{
    DataBlock::BlockStruct bds;
    bds.block = block;
    bds.defblock = defblock;
    bds.blocknum = blocknum;
    bds.blocksize = blocksize;
    bds.caption = caption;
    bds.blocktype = type;
    DataBlock *dblock = new DataBlock(bds);
    m_blocks[m_blockCount] = dblock;
    return m_blockCount++;
}

/*void AbstractTuneDialog::ShowTable()
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
} */

void AbstractTuneDialog::WaitNSeconds(int Seconds, bool isAllowedToStop)
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

void AbstractTuneDialog::Wait15Seconds()
{
    WaitNSeconds(15, false);
}

Error::Msg AbstractTuneDialog::CheckPassword()
{
    KeyPressDialog dlg; // = new KeyPressDialog;
    return dlg.CheckPassword("121941");
}

/*bool AbstractTuneDialog::IsWithinLimits(double number, double base, double threshold)
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
}*/

void AbstractTuneDialog::MsgSetVisible(int msg, bool Visible)
{
    WDFunc::SetVisible(this, "tunemsg" + QString::number(msg), Visible);
}

void AbstractTuneDialog::OkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap("images/ok.png");
    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
}

void AbstractTuneDialog::ErMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap("images/cross.png");
    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
}

void AbstractTuneDialog::SkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap("images/hr.png");
    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
}

void AbstractTuneDialog::MsgClear()
{
    int i;
    for (i = 0; i < lbls.size(); ++i)
    {
        MsgSetVisible(i, false);
        OkMsgSetVisible(i, false);
    }
    MsgSetVisible(i, false);
}

Error::Msg AbstractTuneDialog::StartMeasurement()
{
    MeasurementTimer->start();
    SetMeasurementEnabled(true);
    while (MeasurementEnabled && !StdFunc::IsCancelled())
        TimeFunc::Wait();
    MeasurementTimer->stop();
    if (StdFunc::IsCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

// ####################### SLOTS #############################

void AbstractTuneDialog::StartTune()
{
    WDFunc::SetEnabled(this, "starttune", false);
    if (lbls.size() > pf.size())
    {
        ERMSG("lbls size > pf size");
        WDFunc::SetEnabled(this, "starttune", true);
        return;
    }
    // сохраняем на всякий случай настроечные коэффициенты
    if (SaveAllTuneCoefs() == Error::Msg::NoError)
        TuneFileSaved = true;
    else
        TuneFileSaved = false;
    //    ReadAllTuneCoefs();
    ReadTuneCoefs();
    //    MeasurementTimer->start();
    StdFunc::ClearCancel();
    Skipped = false;
    MsgClear(); // очистка экрана с сообщениями
    for (bStep = 0; bStep < lbls.size(); ++bStep)
    {
        //        WaitNSeconds(2);
        MsgSetVisible(bStep);
        Error::Msg res = (this->*pf[lbls.at(bStep)])();
        if ((res == Error::Msg::GeneralError) || (StdFunc::IsCancelled()))
        {
            ErMsgSetVisible(bStep);
            WDFunc::SetEnabled(this, "starttune", true);
            WARNMSG(lbls.at(bStep));
            //           MeasurementTimer->stop();
            return;
        }
        else if (res == Error::Msg::ResEmpty)
            SkMsgSetVisible(bStep);
        else
            OkMsgSetVisible(bStep);
    }
    MsgSetVisible(bStep); // выдаём надпись "Настройка завершена!"
    //    MeasurementTimer->stop();
    WDFunc::SetEnabled(this, "starttune", true);
    QMessageBox::information(this, "Готово", "Настройка завершена!");
}

// void AbstractTuneDialog::PasswordCheck(QString psw)
//{
//    ok = false;
//    if (psw.isEmpty())
//        StdFunc::Cancel();
//    else if (psw == "121941")
//        ok = true;
//    emit PasswordChecked();
//}

void AbstractTuneDialog::TuneReadCoefs(int index)
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

    for (int i = 0; i < m_TuneBlockMap.keys().size(); i++)
    {
        int bacnum;
        if (Board::GetInstance().typeM() == 135)
            bacnum = i + 1;
        else
            bacnum = i + 2;
        if (m_TuneBlockMap.keys().contains(bacnum))
        {
            ReadTuneCoefsByBac(bacnum);
            TimeFunc::Wait(100);
        }
    }
    //  }
    // }
}

void AbstractTuneDialog::ReadTuneCoefs()
{
    //    int bacnum = sender()->objectName().toInt();
    //    ReadTuneCoefsByBac(bacnum);
    ReadBlocks(DataBlock::DataBlockTypes::BacBlock);
}

// void AbstractTuneDialog::ReadTuneCoefsByBac(int bacnum)
//{
//    if (m_TuneBlockMap.keys().contains(bacnum))
//    {
//        Error::Msg res = Commands::GetBac(bacnum, m_TuneBlockMap[bacnum].block, m_TuneBlockMap[bacnum].blocksize);
//        if (res == Error::Msg::NoError)
//            FillBac(bacnum);
//    }
//}

Error::Msg AbstractTuneDialog::LoadTuneSequenceFile()
{
    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::CheckCalibrStep()
{
    QString usbserialnum = EProtocom::GetInstance().usbSerial();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    if (!storedcalibrations.contains(usbserialnum))
    {
        QMessageBox::warning(this, "Внимание",
            "Не выполнены предыдущие шаги регулировки, пожалуйста,\n"
            "начните заново с шага 1");
        return Error::Msg::ResEmpty;
    }
    int calibrstep = storedcalibrations.value(usbserialnum + "/step", "1").toInt();
    if (calibrstep < m_tuneStep)
    {
        QMessageBox::warning(this, "Внимание",
            "Перед выполнением шага " + QString::number(m_tuneStep) + " необходимо\nвыполнить шаг " + calibrstep + "!");

        return Error::Msg::ResEmpty;
    }
    return Error::Msg::NoError;
}

void AbstractTuneDialog::SaveTuneSequenceFile()
{
}

Error::Msg AbstractTuneDialog::SaveWorkConfig(int configblocknum)
{
    Q_UNUSED(configblocknum)
    return Error::Msg::NoError;
}

bool AbstractTuneDialog::WriteTuneCoefsSlot()
{
    int bacnum = sender()->objectName().toInt();
    if (CheckPassword() != Error::Msg::NoError)
        return false;
    // FillBackBac(bacnum);
    // return WriteTuneCoefs(bacnum);

    if (QMessageBox::question(this, "Вопрос",
            "Сохранить регулировочные коэффициенты?\n(Результаты предыдущей регулировки будут потеряны)")
        == false)
        return false;

    for (int i = 0; i < m_TuneBlockMap.keys().size(); i++)
    {
        if (Board::GetInstance().typeM() == 135)
            bacnum = i + 1;
        else
            bacnum = i + 2;

        if (m_TuneBlockMap.keys().contains(bacnum))
        {
            //            m_VModel->updateFromModel();
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

bool AbstractTuneDialog::WriteTuneCoefs(int bacnum)
{
    // if (QMessageBox::question(this, "Вопрос", "Сохранить регулировочные коэффициенты?\n(Результаты предыдущей
    // регулировки будут потеряны)") == false)
    //    return false;
    // QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    if (m_TuneBlockMap.keys().contains(bacnum))
    {
        if (Commands::WriteBac(bacnum, m_TuneBlockMap[bacnum].block, m_TuneBlockMap[bacnum].blocksize)
            == Error::Msg::NoError)
        {
            // QMessageBox::information(this, "Внимание", "Коэффициенты переданы в " + tmps + " успешно!");
            return true;
        }
    }
    // QMessageBox::critical(this, "Ошибка", "Ошибка записи коэффициентов в " + tmps + "!");
    return false;
}

Error::Msg AbstractTuneDialog::SaveAllTuneCoefs()
{
    foreach (DataBlock *dblock, m_blocks)
    {
        if (dblock->block().blocktype == DataBlock::DataBlockTypes::BacBlock)
        {
            DataBlock::BlockStruct block = dblock->block();
            QByteArray ba;
            ba.resize(block.blocksize);
            memcpy(&(ba.data()[0]), block.block, block.blocksize);
            if (Files::SaveToFile(StdFunc::GetSystemHomeDir() + "temptune.tn" + block.blocknum, ba, block.blocksize)
                != Error::Msg::NoError)
                return Error::Msg::GeneralError;
        }
    }
    return Error::Msg::NoError;
}

void AbstractTuneDialog::PrereadConf()
{
    IsNeededDefConf = (ModuleBSI::PrereadConf(this, S2Config) == Error::Msg::ResEmpty) ? true : false;
}

void AbstractTuneDialog::SaveToFileEx(int bacnum)
{
    Error::Msg res = Error::Msg::NoError;
    QString tunenum = QString::number(bacnum, 16);
    if (!m_TuneBlockMap.keys().contains(bacnum))
    {
        QMessageBox::critical(this, "Ошибка", "Блок Bac с индексом " + tunenum + " не найден!");
        return;
    }
    QByteArray ba;
    ba.resize(m_TuneBlockMap[bacnum].blocksize);
    memcpy(&(ba.data()[0]), m_TuneBlockMap[bacnum].block, m_TuneBlockMap[bacnum].blocksize);
    res = Files::SaveToFile(Files::ChooseFileForSave(this, "Tune files (*.tn" + tunenum + ")", "tn" + tunenum), ba,
        m_TuneBlockMap[bacnum].blocksize);
    switch (res)
    {
    case Error::Msg::NoError:
        QMessageBox::information(this, "Внимание", "Файл коэффициентов записан успешно!");
        break;
    case Error::Msg::FILE_WRITE:
        QMessageBox::critical(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case Error::Msg::FILE_NAMEEMP:
        QMessageBox::critical(this, "Ошибка", "Пустое имя файла!");
        break;
    case Error::Msg::FILE_OPEN:
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void AbstractTuneDialog::ReadBlocks(DataBlock::DataBlockTypes type)
{
    foreach (DataBlock *dblock, m_blocks)
    {
        if (dblock->block().blocktype == type)
            dblock->readBlockFromModule();
    }
}

void AbstractTuneDialog::SaveToFile()
{
    int bacnum = sender()->objectName().toInt();
    //    FillBackBac(bacnum);
    //    m_VModel->updateFromModel();
    SaveToFileEx(bacnum);
}

void AbstractTuneDialog::SetMeasurementEnabled(bool enabled)
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

void AbstractTuneDialog::LoadFromFile()
{
    int bacnum = sender()->objectName().toInt();
    QByteArray ba;
    ba.resize(MAXTUNESIZE);
    QString tunenum = QString::number(bacnum, 16);
    if (!m_TuneBlockMap.keys().contains(bacnum))
    {
        QMessageBox::critical(this, "Ошибка", "Блок Bac с индексом " + tunenum + " не найден!");
        return;
    }
    Error::Msg res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Tune files (*.tn" + tunenum + ")"), ba);
    if (res != Error::Msg::NoError)
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка при загрузке файла");
        return;
    }
    memcpy(m_TuneBlockMap[bacnum].block, &(ba.data()[0]), ba.size());
    FillBac(bacnum);
    QMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
}

void AbstractTuneDialog::Good()
{
    SetMeasurementEnabled(false);
}

void AbstractTuneDialog::NoGood()
{
    SetMeasurementEnabled(false);
    StdFunc::Cancel();
}

void AbstractTuneDialog::CancelTune()
{
    StdFunc::Cancel();
    emit Finished();
}

void AbstractTuneDialog::MeasTimerTimeout()
{
    ReadBlocks(DataBlock::DataBlockTypes::BdBlock);
}

// ##################### PROTECTED ####################

void AbstractTuneDialog::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}

void AbstractTuneDialog::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        StdFunc::Cancel();
    QDialog::keyPressEvent(e);
}
