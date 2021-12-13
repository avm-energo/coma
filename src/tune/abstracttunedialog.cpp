#include "abstracttunedialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/modules.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../interfaces/protocom.h"
#include "../widgets/epopup.h"
#include "../widgets/waitwidget.h"
#include "../widgets/wd_func.h"
#include "generaltunedialog.h"
//#include "limereport/lrreportengine.h"
#include "../gen/configv.h"
#include "tunesequencefile.h"

#include <QDebug>
#include <QMessageBox>
#include <QProgressBar>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <limereport/LimeReport>
namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char name[] = "tuneHash";
}
AbstractTuneDialog::AbstractTuneDialog(ConfigV *config, int tuneStep, QWidget *parent)
    : QDialog(parent), configV(config)
{
    TuneVariant = 0;
    IsNeededDefConf = false;
    m_blockCount = 0;
    m_tuneStep = tuneStep;
    m_finished = false;
    GeneralTuneDialog *dlg = qobject_cast<GeneralTuneDialog *>(parent);
    if (dlg)
        connect(this, &AbstractTuneDialog::Finished, dlg, &GeneralTuneDialog::setCalibrButtons);
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
        this, "starttune", "", this, &AbstractTuneDialog::startTune, ":/icons/tnstart.svg", "Начать настройку"));
    WDFunc::setMinimumSize(this, "starttune", 50, 50);

    hlyout->addWidget(WDFunc::NewPB(
        this, "stoptune", "", this, &AbstractTuneDialog::CancelTune, ":/icons/tnstop.svg", "Прервать настройку"));
    WDFunc::setMinimumSize(this, "stoptune", 50, 50);
    WDFunc::SetEnabled(this, "stoptune", false);
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    QScrollArea *area = new QScrollArea;
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget *w2 = new QWidget;
    QVBoxLayout *w2lyout = new QVBoxLayout;
    w2lyout->addWidget(WDFunc::NewLBL2(this, "Для запуска регулировки нажмите кнопку \"Начать настройку\""));
    for (i = 0; i < m_messages.size(); ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL2(w2, m_messages.at(i), "tunemsg" + QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBL2(w2, "", "tunemsgres" + QString::number(i)));
        hlyout->addStretch(1);
        w2lyout->addLayout(hlyout);
    }
    w2lyout->addSpacing(50);
    w2->setLayout(w2lyout);
    area->setWidgetResizable(true);
    area->setWidget(w2);
    lyout->addWidget(area);
    lyout->addWidget(WDFunc::NewLBL2(w2, "Настройка завершена!", "tunemsg" + QString::number(i)));
    for (i = 0; i < m_messages.size(); ++i)
    {
        WDFunc::SetVisible(w2, "tunemsg" + QString::number(i), false);
        WDFunc::SetVisible(w2, "tunemsgres" + QString::number(i), false);
    }
    WDFunc::SetVisible(w2, "tunemsg" + QString::number(i), false);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(300);
    hlyout->addWidget(WDFunc::NewHexagonPB(
        this, "finishpb",
        [this]() {
            emit Finished();
            this->hide();
        },
        ":/icons/tnyes.svg", "Готово"));
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
    prb->setMaximumHeight(10);
    connect(this, &AbstractTuneDialog::setProgressSize, this, &AbstractTuneDialog::setProgressSizeSlot);
    connect(this, &AbstractTuneDialog::setProgressCount, this, &AbstractTuneDialog::setProgressCountSlot);
    hlyout->addWidget(prb, 100);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);
    return w;
}

void AbstractTuneDialog::SetBac(DataBlock *block)
{
    AbsBac[block->block().blocknum] = block;
}

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
    connect(w, &WaitWidget::CountZero, &el, &QEventLoop::quit);
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
    return (dlg.CheckPassword(crypto::hash)) ? Error::Msg::NoError : Error::Msg::GeneralError;
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

void AbstractTuneDialog::startTune()
{
    Error::Msg res;
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
        if (!EMessageBox::question("Сохранение настроечных коэффициентов не произведено, продолжать?"))
            return;
    }
    StdFunc::clearCancel();
    MsgClear(); // очистка экрана с сообщениями
    for (bStep = 0; bStep < m_messages.size(); ++bStep)
    {
        MsgSetVisible(NoMsg, bStep);
        res = (this->*m_tuneFunctions.at(bStep))();
        if ((res == Error::Msg::GeneralError) || (StdFunc::isCancelled()))
        {
            MsgSetVisible(ErMsg, bStep);
#ifndef DEBUGISON
            WDFunc::SetEnabled(this, "starttune", true);
            WDFunc::SetEnabled(this, "stoptune", false);
            WDFunc::SetEnabled(this, "finishpb", true);
            qWarning() << m_messages.at(bStep);
            loadAllTuneCoefs();
            EMessageBox::error(Error::MsgStr[res]);
            return;
#endif
        }
        else if (res == Error::Msg::ResEmpty)
            MsgSetVisible(SkMsg, bStep);
        else
            MsgSetVisible(OkMsg, bStep);
    }
    MsgSetVisible(NoMsg, bStep); // выдаём надпись "Настройка завершена!"
    WDFunc::SetEnabled(this, "starttune", true);
    WDFunc::SetEnabled(this, "stoptune", false);
    WDFunc::SetEnabled(this, "finishpb", true);
    EMessageBox::information("Настройка завершена!");
    TuneSequenceFile::saveTuneSequenceFile(m_tuneStep + 1); // +1 to let the next stage run
}

void AbstractTuneDialog::setProgressSizeSlot(int size)
{
    QProgressBar *prb = this->findChild<QProgressBar *>("prb");
    if (prb != nullptr)
    {
        prb->setMinimum(0);
        prb->setMaximum(size);
    }
}

void AbstractTuneDialog::setProgressCountSlot(int count)
{
    QProgressBar *prb = this->findChild<QProgressBar *>("prb");
    if (prb != nullptr)
    {
        prb->setValue(count);
    }
}

Error::Msg AbstractTuneDialog::saveAllTuneCoefs()
{
    QString tunenum;
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        tunenum = QString::number(it.key(), 16); // key is the number of Bac block
        QByteArray ba;
        ba.resize(it.value()->block().blocksize);
        memcpy(&(ba.data()[0]), it.value()->block().block, it.value()->block().blocksize);
        if (it.value()->saveToFile() != Error::Msg::NoError)
            //        if (Files::SaveToFile(StdFunc::GetSystemHomeDir() + "temptune.tn" + tunenum, ba) !=
            //        Error::Msg::NoError)
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::loadAllTuneCoefs()
{
    QString tunenum;
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        tunenum = QString::number(it.key(), 16); // key is the number of Bac block
        QByteArray ba;
        if (it.value()->readFromFile() == Error::Msg::NoError)
            //        if (Files::LoadFromFile(StdFunc::GetSystemHomeDir() + "temptune.tn" + tunenum, ba) ==
            //        Error::Msg::NoError)
            memcpy(it.value()->block().block, &(ba.data()[0]), it.value()->block().blocksize);
    }
    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::readTuneCoefs()
{
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
        readTuneCoefsByBac(it.key());
    return Error::Msg::NoError;
}

// void AbstractTuneDialog::loadTuneCoefsSlot()
//{
//    readTuneCoefs();
//}

// void AbstractTuneDialog::saveTuneCoefsSlot()
//{
//    saveAllTuneCoefs();
//}

// на будущее, если вдруг будем регулировать модуль по частям
void AbstractTuneDialog::readTuneCoefsByBac(int bacnum)
{
    if (AbsBac.contains(bacnum))
        AbsBac[bacnum]->readAndUpdate();
}

void AbstractTuneDialog::showTWTab(int index)
{
    QTabWidget *tw = this->findChild<QTabWidget *>("tunetw");
    if (tw != nullptr)
        tw->setCurrentIndex(index);
}

Error::Msg AbstractTuneDialog::writeTuneCoefs(bool isUserChoosingRequired)
{
    if (!isUserChoosingRequired)
        writeTuneCoefsSlot();
    else
    {
        QDialog *dlg = new QDialog(this);
        QVBoxLayout *lyout = new QVBoxLayout;
        QHBoxLayout *hlyout = new QHBoxLayout;
        lyout->addWidget(WDFunc::NewLBL2(this, "Вопрос", "Записать регулировочные коэффициенты?"));
        QTabWidget *tw = new QTabWidget;
        for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
            tw->addTab(it.value()->widget(false), it.value()->block().caption); // do not show buttons
        lyout->addWidget(tw);
        hlyout->addWidget(WDFunc::NewPB(this, "", "Записать", this, &AbstractTuneDialog::writeTuneCoefsSlot));
        hlyout->addWidget(WDFunc::NewPB(this, "", "Отмена", [this]() {
            CancelTune();
            emit generalEventReceived();
        }));

        lyout->addLayout(hlyout);
        dlg->setLayout(lyout);
        dlg->show();
        QEventLoop ev;
        connect(this, &AbstractTuneDialog::generalEventReceived, &ev, &QEventLoop::quit);
        ev.exec();
        dlg->close();
    }
    return (StdFunc::isCancelled() ? Error::Msg::GeneralError : Error::Msg::NoError);
}

void AbstractTuneDialog::writeTuneCoefsSlot()
{
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        it.value()->updateFromWidget();
        if (it.value()->writeBlockToModule() != Error::Msg::NoError)
            CancelTune();
    }
    EMessageBox::information("Коэффициенты записаны успешно!");
    emit generalEventReceived();
}

Error::Msg AbstractTuneDialog::checkCalibrStep()
{
    //    QString cpuserialnum = Board::GetInstance().UID();
    //    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    //    if (!storedcalibrations.contains(cpuserialnum + "/step"))
    if (!TuneSequenceFile::contains("step"))
    {
        EMessageBox::warning("Не выполнены предыдущие шаги регулировки, пожалуйста,\n"
                             "начните заново с шага 1");
        return Error::Msg::ResEmpty;
    }
    //    int calibrstep = storedcalibrations.value(cpuserialnum + "/step", "1").toInt();
    int calibrstep = TuneSequenceFile::value("step").toInt();
    if (calibrstep < m_tuneStep)
    {
        EMessageBox::warning("Перед выполнением шага " + QString::number(m_tuneStep) + " необходимо\nвыполнить шаг "
            + QString::number(calibrstep) + "!");

        return Error::Msg::ResEmpty;
    }
    return Error::Msg::NoError;
}

// void AbstractTuneDialog::saveTuneSequenceFile(int step)
//{
//    QString cpuserialnum = Board::GetInstance().UID();
//    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
//    int calibrstep = storedcalibrations.value(cpuserialnum + "/step", "1").toInt();
//    if (step > calibrstep)
//        storedcalibrations.setValue(cpuserialnum + "/step", step);
//}

Error::Msg AbstractTuneDialog::saveWorkConfig()
{
    QByteArray ba;
    if (BaseInterface::iface()->readFileSync(DataTypes::Config, ba) != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    return Files::SaveToFile(StdFunc::GetSystemHomeDir() + Board::GetInstance().UID() + ".cf", ba);
}

Error::Msg AbstractTuneDialog::loadWorkConfig()
{
    QByteArray ba;
    if (Files::LoadFromFile(StdFunc::GetSystemHomeDir() + Board::GetInstance().UID() + ".cf", ba)
        != Error::Msg::NoError)
        return BaseInterface::iface()->writeFileSync(DataTypes::Config, ba);
    return Error::Msg::GeneralError;
}

void AbstractTuneDialog::CancelTune()
{
    StdFunc::cancel();
}

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
    }
    if (e->key() == Qt::Key_Escape)
        StdFunc::cancel();
    QDialog::keyPressEvent(e);
}
