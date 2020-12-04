#include "abstracttunedialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../gen/files.h"
//#include "../gen/modulebsi.h"
#include "../config/configkiv.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../interfaces/protocom.h"
#include "../module/module_kiv.h"
#include "../module/modules.h"
#include "../widgets/waitwidget.h"
#include "../widgets/wd_func.h"
#include "limereport/lrreportengine.h"

#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QtDebug>

AbstractTuneDialog::AbstractTuneDialog(int tuneStep, QWidget *parent) : UDialog(parent)
{
    TuneVariant = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    //    SetMeasurementEnabled(false);
    MeasurementTimer = new QTimer;
    MeasurementTimer->setInterval(MEASTIMERINT);
    IsNeededDefConf = false;
    connect(MeasurementTimer, SIGNAL(timeout()), this, SLOT(MeasTimerTimeout()));
    RepModel = new ReportModel;
    //    m_ConfigCounter = 0;
    m_blockCount = 0;
    m_tuneStep = tuneStep;
    SetupUI();
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
    setLayout(vlyout);
}

QWidget *AbstractTuneDialog::TuneUI()
{
    m_messages.clear();
    m_tuneFunctions.clear();

    setMessages();
    setTuneFunctions();

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
    for (i = 0; i < m_messages.size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(this, m_messages.at(i), "", "tunemsg" + QString::number(i)));
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
    for (int i = 0; i < m_messages.size() + 1; ++i)
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

    connect(pb, SIGNAL(clicked()), this, SLOT(readTuneCoefs()));

    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    tmps = "Записать настроечные коэффициенты в ";
    tmps += ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    pb = new QPushButton(tmps);
    //    pb->setObjectName(QString::number(bacnum));

    connect(pb, SIGNAL(clicked()), this, SLOT(writeTuneCoefs()));

    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setIcon(QIcon("../load.png"));
    //    pb->setObjectName(QString::number(bacnum));
    connect(pb, SIGNAL(clicked()), this, SLOT(LoadTuneBlocksFromFile()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon("../save.png"));
    //    pb->setObjectName(QString::number(bacnum));
    connect(pb, SIGNAL(clicked()), this, SLOT(SaveTuneBlocksToFiles()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return w;
}

void AbstractTuneDialog::SetBac(void *block, int blocknum, int blocksize)
{
    BlockStruct Bac;
    Bac.BacBlock = block;
    Bac.BacBlockSize = blocksize;
    AbsBac[blocknum] = Bac;
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

bool AbstractTuneDialog::CheckPassword()
{
    KeyPressDialog dlg; // = new KeyPressDialog;
    return (dlg.CheckPassword("121941")) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

bool AbstractTuneDialog::IsWithinLimits(double number, double base, double threshold)
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

// void AbstractTuneDialog::MsgSetVisible(int msg, bool Visible)
//{
//    WDFunc::SetVisible(this, "tunemsg" + QString::number(msg), Visible);
//}

void AbstractTuneDialog::MsgSetVisible(AbstractTuneDialog::MsgTypes type, int msg, bool Visible)
{
    QPixmap *pm;
    switch (type)
    {
    case OkMsg:
        pm = new QPixmap("images/ok.png");
        break;
    case ErMsg:
        pm = new QPixmap("images/cross.png");
        break;
    case SkMsg:
        pm = new QPixmap("images/hr.png");
        break;
    case NoMsg:
        WDFunc::SetVisible(this, "tunemsg" + QString::number(msg), Visible);
        return;
    }
    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
}

// void AbstractTuneDialog::OkMsgSetVisible(int msg, bool Visible)
//{
//    QPixmap *pm = new QPixmap();
//    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
//    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
//}

// void AbstractTuneDialog::ErMsgSetVisible(int msg, bool Visible)
//{
//    QPixmap *pm = new QPixmap("images/cross.png");
//    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
//    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
//}

// void AbstractTuneDialog::SkMsgSetVisible(int msg, bool Visible)
//{
//    QPixmap *pm = new QPixmap("images/hr.png");
//    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), Visible);
//    WDFunc::SetLBLImage(this, "tunemsgres" + QString::number(msg), pm);
//}

void AbstractTuneDialog::MsgClear()
{
    int i;
    for (i = 0; i < m_messages.size(); ++i)
    {
        MsgSetVisible(NoMsg, i, false);
        MsgSetVisible(OkMsg, i, false);
    }
    MsgSetVisible(NoMsg, i, false);
}

Error::Msg AbstractTuneDialog::StartMeasurement()
{
    MeasurementTimer->start();
    //    SetMeasurementEnabled(true);
    //    while (MeasurementEnabled && !StdFunc::isCancelled())
    while (!StdFunc::isCancelled())
        TimeFunc::Wait();
    MeasurementTimer->stop();
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

void AbstractTuneDialog::StartTune()
{
    WDFunc::SetEnabled(this, "starttune", false);
    if (m_messages.size() > m_tuneFunctions.size())
    {
        DBGMSG("lbls size > pf size");
        WDFunc::SetEnabled(this, "starttune", true);
        return;
    }
    // сохраняем на всякий случай настроечные коэффициенты
    //    if (SaveBlocksToFiles(DataBlock::DataBlockTypes::BacBlock) != Error::Msg::NoError)
    if (saveAllTuneCoefs() != Error::Msg::NoError)
    {
        if (QMessageBox::question(this, "Вопрос", "Сохранение настроечных коэффициентов не произведено, продолжать?")
            == QMessageBox::No)
            return;
    }
    //        TuneFileSaved = true;
    //    else
    //        TuneFileSaved = false;
    //    ReadAllTuneCoefs();
    readTuneCoefs();
    //    MeasurementTimer->start();
    StdFunc::clearCancel();
    //    Skipped = false;
    MsgClear(); // очистка экрана с сообщениями
    for (bStep = 0; bStep < m_messages.size(); ++bStep)
    {
        //        WaitNSeconds(2);
        MsgSetVisible(NoMsg, bStep);
        Error::Msg res = (this->*m_tuneFunctions[m_messages.at(bStep)])();
        if ((res == Error::Msg::GeneralError) || (StdFunc::isCancelled()))
        {
            MsgSetVisible(ErMsg, bStep);
            WDFunc::SetEnabled(this, "starttune", true);
            qWarning() << m_messages.at(bStep);
            //           MeasurementTimer->stop();
            loadAllTuneCoefs();
            return;
        }
        else if (res == Error::Msg::ResEmpty)
            MsgSetVisible(SkMsg, bStep);
        else
            MsgSetVisible(OkMsg, bStep);
    }
    MsgSetVisible(NoMsg, bStep); // выдаём надпись "Настройка завершена!"
    //    MeasurementTimer->stop();
    WDFunc::SetEnabled(this, "starttune", true);
    QMessageBox::information(this, "Готово", "Настройка завершена!");
    saveTuneSequenceFile();
}

Error::Msg AbstractTuneDialog::saveAllTuneCoefs()
{
    QString tunenum;
    for (QMap<int, BlockStruct>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        tunenum = QString::number(it.key(), 16); // key is the number of Bac block
        QByteArray ba;
        ba.resize(it.value().BacBlockSize);
        memcpy(&(ba.data()[0]), it.value().BacBlock, it.value().BacBlockSize);
        if (Files::SaveToFile(StdFunc::GetSystemHomeDir() + "temptune.tn" + tunenum, ba) != Error::Msg::NoError)
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::loadAllTuneCoefs()
{
    QString tunenum;
    for (QMap<int, BlockStruct>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        tunenum = QString::number(it.key(), 16); // key is the number of Bac block
        QByteArray ba;
        if (Files::LoadFromFile(StdFunc::GetSystemHomeDir() + "temptune.tn" + tunenum, ba) == Error::Msg::NoError)
            memcpy(it.value().BacBlock, &(ba.data()[0]), it.value().BacBlockSize);
    }
    return Error::Msg::NoError;
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

// void AbstractTuneDialog::TuneReadCoefs(int index)
//{
//    Q_UNUSED(index);
//    /*if(!MainWindow::TheEnd)
//    {
//        if(index == TuneIndex)
//        {
//            if(TimeIndex)
//            emit stopRead(TimeIndex);

//            if(CheckIndex)
//            emit stopRead(CheckIndex);*/

//    TimeFunc::Wait(100);

//    for (int i = 0; i < m_TuneBlockMap.keys().size(); i++)
//    {
//        int bacnum;
//        if (Board::GetInstance().typeM() == 135)
//            bacnum = i + 1;
//        else
//            bacnum = i + 2;
//        if (m_TuneBlockMap.keys().contains(bacnum))
//        {
//            ReadTuneCoefsByBac(bacnum);
//            TimeFunc::Wait(100);
//        }
//    }
//    //  }
//    // }
//}

void AbstractTuneDialog::readTuneCoefs()
{
    //    int bacnum = sender()->objectName().toInt();
    for (QMap<int, BlockStruct>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        readTuneCoefsByBac(it.key());
    }
    //    ReadBlocks(DataBlock::DataBlockTypes::BacBlock);
}

// на будущее, если вдруг будем регулировать модуль по частям
void AbstractTuneDialog::readTuneCoefsByBac(int bacnum)
{
    //    if (m_TuneBlockMap.keys().contains(bacnum))
    //    {
    //        Error::Msg res = Commands::GetBac(bacnum, m_TuneBlockMap[bacnum].block, m_TuneBlockMap[bacnum].blocksize);
    //        if (res == Error::Msg::NoError)
    //            FillBac(bacnum);
    //    }
    if (AbsBac.keys().contains(bacnum))
    {
        //        iface()->writeCommand(Queries::QUSB_ReqTuningCoef, bacnum);
        if (iface()->reqBlockSync(
                bacnum, DataTypes::DataBlockTypes::BacBlock, AbsBac[bacnum].BacBlock, AbsBac[bacnum].BacBlockSize)
            == Error::Msg::NoError)
            //        int res = Commands::GetBac(bacnum, AbsBac[bacnum].BacBlock, AbsBac[bacnum].BacBlockSize);
            //        if (res == Error::Msg::NoError)
            FillBac(bacnum);
    }
}

Error::Msg AbstractTuneDialog::writeTuneCoefsByBac(int bacnum)
{
    if (AbsBac.keys().contains(bacnum))
    {
        FillBackBac(bacnum);
        return iface()->writeBlockSync(
            bacnum, DataTypes::DataBlockTypes::BacBlock, AbsBac[bacnum].BacBlock, AbsBac[bacnum].BacBlockSize);
    }
    return Error::Msg::GeneralError;
}

bool AbstractTuneDialog::writeTuneCoefs()
{
    //    int bacnum = sender()->objectName().toInt();
    if (CheckPassword() != Error::Msg::NoError)
        return false;

    if (QMessageBox::question(this, "Вопрос",
            "Сохранить регулировочные коэффициенты?\n(Результаты предыдущей регулировки будут потеряны)")
        == false)
        return false;

    for (QMap<int, BlockStruct>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        FillBackBac(it.key());
        if (writeTuneCoefsByBac(it.key()) != Error::Msg::NoError)
            return false;
    }

    //    for (int i = 0; i < m_TuneBlockMap.keys().size(); i++)
    //    {
    //        if (Board::GetInstance().typeM() == 135)
    //            bacnum = i + 1;
    //        else
    //            bacnum = i + 2;

    //        if (m_TuneBlockMap.keys().contains(bacnum))
    //        {
    //            //            m_VModel->updateFromModel();
    //            //            FillBackBac(bacnum);

    //            if (WriteTuneCoefs(bacnum) != true)
    //                return false;

    //            TimeFunc::Wait(100);
    //        }
    //    }
    //    WriteBlocks(DataBlock::DataBlockTypes::BacBlock);
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    QMessageBox::information(this, "Внимание", "Коэффициенты переданы в " + tmps + " успешно!");
    return true;
}

// Error::Msg AbstractTuneDialog::LoadTuneSequenceFile()
//{
//    return Error::Msg::NoError;
//}

Error::Msg AbstractTuneDialog::checkCalibrStep()
{
    QString cpuserialnum = Board::GetInstance().UID();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    if (!storedcalibrations.contains(cpuserialnum))
    {
        QMessageBox::warning(this, "Внимание",
            "Не выполнены предыдущие шаги регулировки, пожалуйста,\n"
            "начните заново с шага 1");
        return Error::Msg::ResEmpty;
    }
    int calibrstep = storedcalibrations.value(cpuserialnum + "/step", "1").toInt();
    if (calibrstep < m_tuneStep)
    {
        QMessageBox::warning(this, "Внимание",
            "Перед выполнением шага " + QString::number(m_tuneStep) + " необходимо\nвыполнить шаг " + calibrstep + "!");

        return Error::Msg::ResEmpty;
    }
    return Error::Msg::NoError;
}

void AbstractTuneDialog::saveTuneSequenceFile()
{
    QString cpuserialnum = Board::GetInstance().UID();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    storedcalibrations.setValue(cpuserialnum + "/step", m_tuneStep);
}

Error::Msg AbstractTuneDialog::saveWorkConfig()
{
    //    return SaveBlocksToFiles(DataBlock::DataBlockTypes::BciBlock);
    //    iface()->reqFile(Files::FilesEnum::Config, true);
    //    memcpy(&m_BciSaveBlock, &CKIV->Bci_block, sizeof(ConfigKIV::Bci));
    //    else return Error::Msg::GeneralError;
    QByteArray ba;
    if (iface()->readFileSync(Files::Config, ba) != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    return Files::SaveToFile(StdFunc::GetSystemHomeDir() + Board::GetInstance().UID() + ".cf", ba);
    //    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::loadWorkConfig()
{
    //    return LoadBlocksFromFiles(DataBlock::DataBlockTypes::BciBlock);
    //    if (Files::LoadFromFile(StdFunc::GetSystemHomeDir() + "temptune.cf") == Error::Msg::NoError)
    //        memcpy(it.value().BacBlock, &(ba.data()[0]), it.value().BacBlockSize);
    QByteArray ba;
    if (Files::LoadFromFile(StdFunc::GetSystemHomeDir() + Board::GetInstance().UID() + ".cf", ba)
        != Error::Msg::NoError)
        return iface()->writeFileSync(Files::Config, ba);
    return Error::Msg::GeneralError;
}

// bool AbstractTuneDialog::WriteTuneCoefs(int blocknum)
//{
//    // if (QMessageBox::question(this, "Вопрос", "Сохранить регулировочные коэффициенты?\n(Результаты предыдущей
//    // регулировки будут потеряны)") == false)
//    //    return false;
//    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
//    if (blocknum < m_blocks.size())
//    {
//        if (m_blocks.at(blocknum)->block().blocktype == DataBlock::DataBlockTypes::BacBlock)
//        {
//            m_blocks.at(blocknum)->writeBlockToModule();
//            QMessageBox::information(this, "Внимание", "Коэффициенты переданы в " + tmps + " успешно!");
//            return true;
//        }
//        WARNMSG("Тип блока неверен, не настроечный блок");
//        return false;
//    }
//    //    if (m_TuneBlockMap.keys().contains(blocknum))
//    //    {
//    //        if (Commands::WriteBac(blocknum, m_TuneBlockMap[blocknum].block, m_TuneBlockMap[blocknum].blocksize)
//    //            == Error::Msg::NoError)
//    //        {
//    //            // QMessageBox::information(this, "Внимание", "Коэффициенты переданы в " + tmps + " успешно!");
//    //            return true;
//    //        }
//    //    }
//    QMessageBox::critical(this, "Ошибка", "Ошибка записи коэффициентов в " + tmps + "!");
//    return false;
//}

// void AbstractTuneDialog::SaveTuneBlocksToFiles()
//{
//        SaveBlocksToFiles(DataBlock::DataBlockTypes::BacBlock);
//}

// Error::Msg AbstractTuneDialog::SaveBlocksToFiles(DataBlock::DataBlockTypes type, bool userChoose)
//{
//    DataBlock::FilePropertiesStruct fst;
//    DataBlock::getFileProperties(type, fst);
//    foreach (DataBlock *dblock, m_blocks)
//    {
//        if (dblock->block().blocktype == type)
//        {
//            DataBlock::BlockStruct block = dblock->block();
//            QByteArray ba;
//            ba.resize(block.blocksize);
//            memcpy(&(ba.data()[0]), block.block, block.blocksize);
//            Error::Msg res;
//            if (userChoose)
//                res = Files::SaveToFile(Files::ChooseFileForSave(this, fst.mask, fst.extension), ba);
//            else
//                res = Files::SaveToFile(StdFunc::GetSystemHomeDir() + fst.filename + block.blocknum, ba);
//            if (res != Error::Msg::NoError)
//            {
//                QMessageBox::critical(this, "Ошибка", "Ошибка при сохранении файла");
//                return Error::Msg::GeneralError;
//            }
//        }
//    }
//    return Error::Msg::NoError;
//}

// Error::Msg AbstractTuneDialog::LoadBlocksFromFiles(DataBlock::DataBlockTypes type, bool userChoose)
//{
//    DataBlock::FilePropertiesStruct fst;
//    DataBlock::getFileProperties(type, fst);
//    //    int bacnum = sender()->objectName().toInt();
//    QByteArray ba;
//    //    ba.resize(MAXTUNESIZE);
//    //    QString tunenum = QString::number(bacnum, 16);
//    //    if (!m_TuneBlockMap.keys().contains(bacnum))
//    //    {
//    //        QMessageBox::critical(this, "Ошибка", "Блок Bac с индексом " + tunenum + " не найден!");
//    //        return;
//    //    }
//    foreach (DataBlock *dblock, m_blocks)
//    {
//        if (dblock->block().blocktype == type)
//        {
//            DataBlock::BlockStruct block = dblock->block();
//            QByteArray ba;
//            Error::Msg res;
//            if (userChoose)
//                res = Files::LoadFromFile(Files::ChooseFileForOpen(this, fst.mask), ba);
//            else
//                res = Files::LoadFromFile(StdFunc::GetSystemHomeDir() + fst.filename + block.blocknum, ba);
//            if (res != Error::Msg::NoError)
//            {
//                QMessageBox::critical(this, "Ошибка", "Ошибка при загрузке файла");
//                return Error::Msg::GeneralError;
//            }
//            memcpy(block.block, &(ba.data()[0]), block.blocksize);
//        }
//    }
//    QMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
//    return Error::Msg::NoError;
//}

// void AbstractTuneDialog::PrereadConf()
//{
//    IsNeededDefConf = (ModuleBSI::PrereadConf(this, S2Config) == Error::Msg::ResEmpty) ? true : false;
//}

// void AbstractTuneDialog::SaveToFileEx(int bacnum)
//{
//    Error::Msg res = Error::Msg::NoError;
//    QString tunenum = QString::number(bacnum, 16);
//    if (!m_TuneBlockMap.keys().contains(bacnum))
//    {
//        QMessageBox::critical(this, "Ошибка", "Блок Bac с индексом " + tunenum + " не найден!");
//        return;
//    }
//    QByteArray ba;
//    ba.resize(m_TuneBlockMap[bacnum].blocksize);
//    memcpy(&(ba.data()[0]), m_TuneBlockMap[bacnum].block, m_TuneBlockMap[bacnum].blocksize);
//    res = Files::SaveToFile(Files::ChooseFileForSave(this, "Tune files (*.tn" + tunenum + ")", "tn" + tunenum), ba,
//        m_TuneBlockMap[bacnum].blocksize);
//    switch (res)
//    {
//    case Error::Msg::NoError:
//        QMessageBox::information(this, "Внимание", "Файл коэффициентов записан успешно!");
//        break;
//    case Error::Msg::FILE_WRITE:
//        QMessageBox::critical(this, "Ошибка", "Ошибка при записи файла!");
//        break;
//    case Error::Msg::FILE_NAMEEMP:
//        QMessageBox::critical(this, "Ошибка", "Пустое имя файла!");
//        break;
//    case Error::Msg::FILE_OPEN:
//        QMessageBox::critical(this, "Ошибка", "Ошибка открытия файла!");
//        break;
//    default:
//        break;
//    }
//}

// void AbstractTuneDialog::ReadBlocks(DataBlock::DataBlockTypes type)
//{
//    foreach (DataBlock *dblock, m_blocks)
//    {
//        if (dblock->block().blocktype == type)
//            dblock->readBlockFromModule();
//    }
//}

// void AbstractTuneDialog::WriteBlocks(DataBlock::DataBlockTypes type)
//{
//    foreach (DataBlock *dblock, m_blocks)
//    {
//        if (dblock->block().blocktype == type)
//            dblock->writeBlockToModule();
//    }
//}

// void AbstractTuneDialog::SaveToFile()
//{
//    int bacnum = sender()->objectName().toInt();
//    //    FillBackBac(bacnum);
//    //    m_VModel->updateFromModel();
//    SaveToFileEx(bacnum);
//}

// void AbstractTuneDialog::SetMeasurementEnabled(bool enabled)
//{
//    if (enabled)
//    {
//        WDFunc::SetEnabled(this, "Good", true);
//        WDFunc::SetEnabled(this, "NoGood", true);
//        MeasurementEnabled = true;
//    }
//    else
//    {
//        WDFunc::SetEnabled(this, "Good", false);
//        WDFunc::SetEnabled(this, "NoGood", false);
//        MeasurementEnabled = false;
//    }
//}

// void AbstractTuneDialog::LoadTuneBlocksFromFile()
//{
//    LoadBlocksFromFiles(DataBlock::DataBlockTypes::BacBlock);
//}

// void AbstractTuneDialog::Good()
//{
//    SetMeasurementEnabled(false);
//}

// void AbstractTuneDialog::NoGood()
//{
//    SetMeasurementEnabled(false);
//    StdFunc::cancel();
//}

void AbstractTuneDialog::CancelTune()
{
    StdFunc::cancel();
    emit Finished();
}

// void AbstractTuneDialog::MeasTimerTimeout()
//{
//    ReadBlocks(DataBlock::DataBlockTypes::BdBlock);
//}

// ##################### PROTECTED ####################

void AbstractTuneDialog::closeEvent(QCloseEvent *e)
{
    //    emit stopall();
    e->accept();
}

void AbstractTuneDialog::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        StdFunc::cancel();
    UDialog::keyPressEvent(e);
}
