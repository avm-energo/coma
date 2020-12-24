#include "abstracttunedialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "generaltunedialog.h"
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
#include <QProgressBar>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QtDebug>

AbstractTuneDialog::AbstractTuneDialog(int tuneStep, QWidget *parent) : QDialog(parent)
{
    TuneVariant = 0;
    //    setAttribute(Qt::WA_DeleteOnClose);
    //    SetMeasurementEnabled(false);
    MeasurementTimer = new QTimer;
    MeasurementTimer->setInterval(MEASTIMERINT);
    IsNeededDefConf = false;
    connect(MeasurementTimer, SIGNAL(timeout()), this, SLOT(MeasTimerTimeout()));
    RepModel = new ReportModel;
    //    m_ConfigCounter = 0;
    m_blockCount = 0;
    m_tuneStep = tuneStep;
    m_finished = false;
    GeneralTuneDialog *dlg = qobject_cast<GeneralTuneDialog *>(parent);
    if (dlg)
    {
        connect(this, &AbstractTuneDialog::Finished, dlg, &GeneralTuneDialog::setCalibrButtons);
    }
}

AbstractTuneDialog::~AbstractTuneDialog()
{
}

void AbstractTuneDialog::SetupUI()
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    hlyout->addWidget(TuneUI());
    if (!m_mainWidgetList.isEmpty())
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
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(
        this, "starttune", "", this, &AbstractTuneDialog::startTune, "images/tnstart.svg", "Начать настройку"));
    WDFunc::setMinimumSize(this, "starttune", 50, 50);
    //    QPushButton *pb = new QPushButton("Начать настройку");
    //    pb->setObjectName("starttune");

    //    connect(pb, SIGNAL(clicked()), this, SLOT(startTune()));

    if (StdFunc::IsInEmulateMode())
        WDFunc::SetEnabled(this, "starttune", false);
    //        pb->setEnabled(false);
    //    else
    //        pb->setEnabled(true);
    //    lyout->addWidget(pb);
    hlyout->addWidget(WDFunc::NewPB(
        this, "stoptune", "", this, &AbstractTuneDialog::CancelTune, "images/tnstop.svg", "Прервать настройку"));
    WDFunc::setMinimumSize(this, "stoptune", 50, 50);
    WDFunc::SetEnabled(this, "stoptune", false);
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    QScrollArea *area = new QScrollArea;
    //    area->setObjectName("scrollarea");
    //    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget *w2 = new QWidget;
    //    w2->setObjectName("messagewidget");
    QVBoxLayout *w2lyout = new QVBoxLayout;
    w2lyout->addWidget(WDFunc::NewLBL2(this, "Для запуска регулировки нажмите кнопку \"Начать настройку\""));
    for (i = 0; i < m_messages.size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL2(this, m_messages.at(i), "tunemsg" + QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBL2(this, "", "tunemsgres" + QString::number(i)));
        //        WDFunc::SetVisible(this, "tunemsg" + QString::number(i), false);
        //        WDFunc::SetVisible(this, "tunemsgres" + QString::number(i), false);
        hlyout->addStretch(1);
        w2lyout->addLayout(hlyout);
    }
    w2lyout->addSpacing(50);
    w2->setLayout(w2lyout);
    //    w2->setMinimumHeight(400); // !!! it has to be replaced
    area->setWidgetResizable(true);
    area->setWidget(w2);
    for (int i = 0; i < m_messages.size() + 1; ++i)
    {
        WDFunc::SetVisible(w2, "tunemsg" + QString::number(i), false);
        WDFunc::SetVisible(w2, "tunemsgres" + QString::number(i), false);
    }
    lyout->addWidget(area);
    //    area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
    lyout->addWidget(WDFunc::NewLBL2(this, "Настройка завершена!", "tunemsg" + QString::number(i)));
    WDFunc::SetVisible(this, "tunemsg" + QString::number(i), false);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(300);
    hlyout->addWidget(WDFunc::NewHexagonPB(
        this, "finishpb",
        [this]() {
            emit Finished();
            this->hide();
        },
        "images/tnyes.svg", "Готово"));
    hlyout->addStretch(300);
    lyout->addLayout(hlyout);
    //    lyout->addStretch(1);
    WDFunc::SetEnabled(this, "finishpb", true);
    w->setLayout(lyout);
    return w;
}

QWidget *AbstractTuneDialog::MainUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *tw = new QTabWidget;
    tw->setObjectName("tunetw");
    for (int i = 0; i < m_mainWidgetList.size(); ++i)
        //    for (auto str : m_mainWidgetList)
        //        tw->addTab(str.widget, str.caption);
        tw->addTab(m_mainWidgetList.at(i).widget, m_mainWidgetList.at(i).caption);
    lyout->addWidget(tw);
    w->setLayout(lyout);
    return w;
}

QWidget *AbstractTuneDialog::BottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL2(this, "Регулировка"), 0);
    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prb");
    prb->setOrientation(Qt::Horizontal);
    // prb->setMinimumWidth(50);
    prb->setMaximumHeight(height() / 50);
    connect(this, &AbstractTuneDialog::setProgressSize, prb, &QProgressBar::setMaximum);
    connect(this, &AbstractTuneDialog::setProgressCount, prb, &QProgressBar::setValue);
    hlyout->addWidget(prb, 100);
    lyout->addLayout(hlyout);
    /*    hlyout = new QHBoxLayout;
        //    QPushButton *pb = new QPushButton;
        //    pb->setToolTip("Установить настроечные коэффициенты по умолчанию");
        //    //    pb->setMinimumSize(70, 70);
        //    pb->setIcon(QIcon("images/reset.svg"));
        //    pb->setIconSize(QSize(70, 70));
        //    //        pb->setObjectName(QString::number(bacnum));
        //    connect(pb, SIGNAL(clicked()), this, SLOT(setDefCoefs()));
        //    hlyout->addWidget(pb);
        hlyout->addWidget(WDFunc::NewPB(this, "resetpb", "", this, &AbstractTuneDialog::setDefCoefs,
       "images/tnreset.svg", "Установить настроечные коэффициенты по умолчанию")); WDFunc::setMinimumSize(this,
       "resetpb", 50, 50); QString tmps = "Прочитать настроечные коэффициенты из "; tmps += ((DEVICETYPE ==
       DEVICETYPE_MODULE) ? "модуля" : "прибора"); hlyout->addWidget( WDFunc::NewPB(this, "readpb", "", this,
       &AbstractTuneDialog::readTuneCoefs, "images/tnread.svg", tmps));

        //    pb = new QPushButton(tmps);
        //    //    pb->setObjectName(QString::number(bacnum));

        //    connect(pb, SIGNAL(clicked()), this, SLOT(readTuneCoefs()));

        if (StdFunc::IsInEmulateMode())
            //        pb->setEnabled(false);
            WDFunc::SetEnabled(this, "readpb", false);
        //    hlyout->addWidget(pb);
        WDFunc::setMinimumSize(this, "readpb", 50, 50);
        tmps = "Записать настроечные коэффициенты в ";
        tmps += ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
        hlyout->addWidget(
            WDFunc::NewPB(this, "writepb", "", this, &AbstractTuneDialog::writeTuneCoefsSlot, "images/tnwrite.svg",
       tmps));
        //    pb = new QPushButton(tmps);
        //    //    pb->setObjectName(QString::number(bacnum));

        //    connect(pb, SIGNAL(clicked()), this, SLOT(writeTuneCoefs()));

        if (StdFunc::IsInEmulateMode())
            //        pb->setEnabled(false);
            WDFunc::SetEnabled(this, "writepb", false);
        WDFunc::setMinimumSize(this, "writepb", 50, 50);
        //    hlyout->addWidget(pb);
        //    lyout->addLayout(hlyout);
        //    hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewPB(this, "loadpb", "", this, &AbstractTuneDialog::loadTuneCoefsSlot,
            "images/tnload.svg", "Прочитать настроечные коэффициенты из файла"));
        //    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
        //    pb->setIcon(QIcon("../load.png"));
        //    //    pb->setObjectName(QString::number(bacnum));
        //    connect(pb, SIGNAL(clicked()), this, SLOT(LoadTuneBlocksFromFile()));
        //    hlyout->addWidget(pb);
        //    pb = new QPushButton("Записать настроечные коэффициенты в файл");
        WDFunc::setMinimumSize(this, "loadpb", 50, 50);
        hlyout->addWidget(WDFunc::NewPB(this, "savepb", "", this, &AbstractTuneDialog::saveTuneCoefsSlot,
            "images/tnsave.svg", "Записать настроечные коэффициенты в файл"));
        //    pb->setIcon(QIcon("../save.png"));
        //    //    pb->setObjectName(QString::number(bacnum));
        //    connect(pb, SIGNAL(clicked()), this, SLOT(SaveTuneBlocksToFiles()));
        //    hlyout->addWidget(pb);
        WDFunc::setMinimumSize(this, "savepb", 50, 50);
        hlyout->addStretch(300);
        lyout->addLayout(hlyout); */
    w->setLayout(lyout);
    return w;
}

// void AbstractTuneDialog::SetBac(void *block, int blocknum, int blocksize)
void AbstractTuneDialog::SetBac(DataBlock *block)
{
    AbsBac[block->block().blocknum] = block;
    //    BlockStruct Bac;
    //    Bac.BacBlock = block;
    //    Bac.BacBlockSize = blocksize;
    //    AbsBac[blocknum] = Bac;
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

void AbstractTuneDialog::startWait()
{
    WaitWidget *ww = new WaitWidget;
    ww->setObjectName("ww");
    WaitWidget::ww_struct wws = { true, true, WaitWidget::WW_SIMPLE, 0 }; // isallowedtostop = true
    ww->Init(wws);
    ww->SetMessage("Пожалуйста, подождите");
    ww->Start();
}

void AbstractTuneDialog::stopWait()
{
    WaitWidget *ww = this->findChild<WaitWidget *>("ww");
    if (ww != nullptr)
        ww->Stop();
}

Error::Msg AbstractTuneDialog::CheckPassword()
{
    KeyPressDialog dlg; // = new KeyPressDialog;
    return (dlg.CheckPassword("121941")) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

int AbstractTuneDialog::addWidgetToTabWidget(QWidget *w, const QString &caption)
{
    MainWidgetStruct str;
    str.widget = w;
    str.caption = caption;
    int widgetindex = m_mainWidgetList.size();
    m_mainWidgetList.append(str);
    return widgetindex;
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

// Error::Msg AbstractTuneDialog::StartMeasurement()
//{
//    MeasurementTimer->start();
//    //    SetMeasurementEnabled(true);
//    //    while (MeasurementEnabled && !StdFunc::isCancelled())
//    while (!StdFunc::isCancelled())
//        TimeFunc::Wait();
//    MeasurementTimer->stop();
//    if (StdFunc::isCancelled())
//        return Error::Msg::GeneralError;
//    return Error::Msg::NoError;
//}

void AbstractTuneDialog::startTune()
{
    if (checkCalibrStep() != Error::Msg::NoError)
        return;
    WDFunc::SetEnabled(this, "starttune", false);
    WDFunc::SetEnabled(this, "finishpb", false);
    WDFunc::SetEnabled(this, "stoptune", true);
    if (m_messages.size() != m_tuneFunctions.size())
    {
        DBGMSG("lbls size != pf size");
        WDFunc::SetEnabled(this, "starttune", true);
        WDFunc::SetEnabled(this, "finishpb", true);
        return;
    }
    // сохраняем на всякий случай настроечные коэффициенты
    //    if (SaveBlocksToFiles(DataBlock::DataBlockTypes::BacBlock) != Error::Msg::NoError)
    readTuneCoefs();
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
    //    MeasurementTimer->start();
    StdFunc::clearCancel();
    //    Skipped = false;
    MsgClear(); // очистка экрана с сообщениями
    for (bStep = 0; bStep < m_messages.size(); ++bStep)
    {
        //        WaitNSeconds(2);
        MsgSetVisible(NoMsg, bStep);
        Error::Msg res = (this->*m_tuneFunctions[m_messages.at(bStep)])();
        if ((res != Error::Msg::NoError) || (StdFunc::isCancelled()))
        {
            MsgSetVisible(ErMsg, bStep);
            WDFunc::SetEnabled(this, "starttune", true);
            WDFunc::SetEnabled(this, "stoptune", false);
            WDFunc::SetEnabled(this, "finishpb", true);
            qWarning() << m_messages.at(bStep);
            //           MeasurementTimer->stop();
            loadAllTuneCoefs();
            QMessageBox::critical(this, "Ошибка", Error::MsgStr[res]);
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
    WDFunc::SetEnabled(this, "stoptune", false);
    WDFunc::SetEnabled(this, "finishpb", true);
    QMessageBox::information(this, "Готово", "Настройка завершена!");
    saveTuneSequenceFile(m_tuneStep + 1); // +1 to let the next stage run
}

// void AbstractTuneDialog::closeThis()
//{
//    this->close();
//}

Error::Msg AbstractTuneDialog::saveAllTuneCoefs()
{
    QString tunenum;
    //    for (QMap<int, BlockStruct>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        tunenum = QString::number(it.key(), 16); // key is the number of Bac block
        QByteArray ba;
        //        ba.resize(it.value().BacBlockSize);
        //        memcpy(&(ba.data()[0]), it.value().BacBlock, it.value().BacBlockSize);
        ba.resize(it.value()->block().blocksize);
        memcpy(&(ba.data()[0]), it.value()->block().block, it.value()->block().blocksize);
        if (Files::SaveToFile(StdFunc::GetSystemHomeDir() + "temptune.tn" + tunenum, ba) != Error::Msg::NoError)
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::loadAllTuneCoefs()
{
    QString tunenum;
    //    for (QMap<int, BlockStruct>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        tunenum = QString::number(it.key(), 16); // key is the number of Bac block
        QByteArray ba;
        if (Files::LoadFromFile(StdFunc::GetSystemHomeDir() + "temptune.tn" + tunenum, ba) == Error::Msg::NoError)
            //            memcpy(it.value().BacBlock, &(ba.data()[0]), it.value().BacBlockSize);
            memcpy(it.value()->block().block, &(ba.data()[0]), it.value()->block().blocksize);
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
    //    for (QMap<int, BlockStruct>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        readTuneCoefsByBac(it.key());
    }
    //    ReadBlocks(DataBlock::DataBlockTypes::BacBlock);
}

void AbstractTuneDialog::writeTuneCoefsSlot()
{
    writeTuneCoefs();
}

void AbstractTuneDialog::loadTuneCoefsSlot()
{
    readTuneCoefs();
}

void AbstractTuneDialog::saveTuneCoefsSlot()
{
    saveAllTuneCoefs();
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
        //    {
        //        BaseInterface::iface()->writeCommand(Queries::QUSB_ReqTuningCoef, bacnum);
        //        if (BaseInterface::iface()->reqBlockSync(
        //                bacnum, DataTypes::DataBlockTypes::BacBlock, AbsBac[bacnum].BacBlock,
        //                AbsBac[bacnum].BacBlockSize)
        //            == Error::Msg::NoError)
        AbsBac[bacnum]->readAndUpdate();
    //        if (BaseInterface::iface()->reqBlockSync(bacnum, DataTypes::DataBlockTypes::BacBlock,
    //                AbsBac[bacnum]->block().block, AbsBac[bacnum]->block().blocksize)
    //            == Error::Msg::NoError)
    //            //        int res = Commands::GetBac(bacnum, AbsBac[bacnum].BacBlock, AbsBac[bacnum].BacBlockSize);
    //            //        if (res == Error::Msg::NoError)
    //            //            FillBac(bacnum);
    //            AbsBac[bacnum]->updateWidget();
    //    }
}

Error::Msg AbstractTuneDialog::writeTuneCoefsByBac(int bacnum)
{
    if (AbsBac.keys().contains(bacnum))
    {
        AbsBac[bacnum]->updateFromWidget();
        AbsBac[bacnum]->writeBlockToModule();
        return Error::Msg::NoError;
        //        FillBackBac(bacnum);
        //        return BaseInterface::iface()->writeBlockSync(
        //            bacnum, DataTypes::DataBlockTypes::BacBlock, AbsBac[bacnum].BacBlock,
        //            AbsBac[bacnum].BacBlockSize);
        //        return BaseInterface::iface()->writeBlockSync(bacnum, DataTypes::DataBlockTypes::BacBlock,
        //            AbsBac[bacnum]->block().block, AbsBac[bacnum]->block().blocksize);
    }
    return Error::Msg::GeneralError;
}

void AbstractTuneDialog::showTWTab(int index)
{
    QTabWidget *tw = this->findChild<QTabWidget *>("tunetw");
    if (tw != nullptr)
        tw->setCurrentIndex(index);
}

bool AbstractTuneDialog::writeTuneCoefs()
{
    //    int bacnum = sender()->objectName().toInt();
    //    if (CheckPassword() != Error::Msg::NoError)
    //        return false;

    if (QMessageBox::question(this, "Вопрос", "Сохранить регулировочные коэффициенты?") == false)
        return false;

    //    for (QMap<int, BlockStruct>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        //        it.value()->updateFromWidget();
        //        FillBackBac(it.key());
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
    //    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    QMessageBox::information(this, "Внимание", "Коэффициенты записаны успешно!");
    return true;
}

// void AbstractTuneDialog::scrollArea()
//{
//    QScrollArea *area = this->findChild<QScrollArea *>("scrollarea");
//    if (area != nullptr)
//        area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
//}

// Error::Msg AbstractTuneDialog::LoadTuneSequenceFile()
//{
//    return Error::Msg::NoError;
//}

Error::Msg AbstractTuneDialog::checkCalibrStep()
{
    QString cpuserialnum = Board::GetInstance().UID();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    if (!storedcalibrations.contains(cpuserialnum + "/step"))
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
            "Перед выполнением шага " + QString::number(m_tuneStep) + " необходимо\nвыполнить шаг "
                + QString::number(calibrstep) + "!");

        return Error::Msg::ResEmpty;
    }
    return Error::Msg::NoError;
}

void AbstractTuneDialog::saveTuneSequenceFile(int step)
{
    QString cpuserialnum = Board::GetInstance().UID();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    storedcalibrations.setValue(cpuserialnum + "/step", step);
}

Error::Msg AbstractTuneDialog::saveWorkConfig()
{
    //    return SaveBlocksToFiles(DataBlock::DataBlockTypes::BciBlock);
    //    BaseInterface::iface()->reqFile(Files::FilesEnum::Config, true);
    //    memcpy(&m_BciSaveBlock, &CKIV->Bci_block, sizeof(ConfigKIV::Bci));
    //    else return Error::Msg::GeneralError;
    QByteArray ba;
    if (BaseInterface::iface()->readFileSync(DataTypes::Config, ba) != Error::Msg::NoError)
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
        return BaseInterface::iface()->writeFileSync(DataTypes::Config, ba);
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
    //    emit Finished();
}

// void AbstractTuneDialog::MeasTimerTimeout()
//{
//    ReadBlocks(DataBlock::DataBlockTypes::BdBlock);
//}

// ##################### PROTECTED ####################

void AbstractTuneDialog::closeEvent(QCloseEvent *e)
{
    emit Finished();
    e->accept();
}

void AbstractTuneDialog::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
        m_finished = true;
        //        emit Finished();
    }
    if (e->key() == Qt::Key_Escape)
        StdFunc::cancel();
    QDialog::keyPressEvent(e);
}
