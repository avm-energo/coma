#include "tune/abstracttunedialog.h"

#include <gen/datatypes.h>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/settings.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <tune/generaltunedialog.h>
#include <tune/tunesequencefile.h>
#include <widgets/emessagebox.h>
#include <widgets/hexpbfunc.h>
#include <widgets/lblfunc.h>
#include <widgets/pbfunc.h>
#include <widgets/waitwidget.h>
#include <widgets/wdfunc.h>

#include <LimeReport>
#include <QDebug>
#include <QMessageBox>
#include <QProgressBar>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

ReportData AbstractTuneDialog::s_reportData {};

AbstractTuneDialog::AbstractTuneDialog(Device::CurrentDevice *device, QWidget *parent)
    : QDialog(parent)
    , m_device(device)
    , m_config(m_device->getS2Datamanager()->getCurrentConfiguration().m_workingConfig)
    , m_async(m_device->async())
    , m_sync(m_device->sync())
    , m_typeB(static_cast<Device::BaseBoard>(m_device->getBaseType()))
    , m_typeM(static_cast<Device::MezzanineBoard>(m_device->getMezzType()))
{
    Q_ASSERT(device != nullptr);
    TuneVariant = 0;
    IsNeededDefConf = false;
    m_blockCount = 0;
    m_finished = false;
    m_tuneTabWidget = new TuneTabWidget;
    GeneralTuneDialog *dlg = qobject_cast<GeneralTuneDialog *>(parent);
    if (dlg)
        connect(this, &AbstractTuneDialog::Finished, dlg, &GeneralTuneDialog::setCalibrButtons);
    QSettings settings;
    m_tuneRequestCount = settings.value("TuneRequestCount", "60").toInt();
}

void AbstractTuneDialog::setupUI()
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    hlyout->addWidget(tuneUI());
    hlyout->addWidget(mainUI());
    vlyout->addLayout(hlyout);
    vlyout->addWidget(bottomUI());
    setLayout(vlyout);
}

QWidget *AbstractTuneDialog::tuneUI()
{
    m_tuneFunctions.clear();

    setTuneFunctions();
    int i;
    // CP1 - НАСТРОЙКА ПРИБОРА/МОДУЛЯ
    QWidget *w = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;

    auto pushbutton = PBFunc::New(
        this, "starttune", "", this, &AbstractTuneDialog::startTune, ":/icons/tnstart.svg", "Начать настройку");
    pushbutton->setMinimumSize(50, 50);
    pushbutton->setIconSize(QSize(40, 40));
    hlyout->addWidget(pushbutton);
    pushbutton = PBFunc::New(
        this, "stoptune", "", this, &AbstractTuneDialog::CancelTune, ":/icons/tnstop.svg", "Прервать настройку");
    pushbutton->setMinimumSize(50, 50);
    pushbutton->setIconSize(QSize(40, 40));
    pushbutton->setEnabled(false);
    hlyout->addWidget(pushbutton);
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    QScrollArea *area = new QScrollArea;
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget *w2 = new QWidget;
    QVBoxLayout *w2lyout = new QVBoxLayout;
    w2lyout->addWidget(LBLFunc::New(this, "Для запуска регулировки нажмите кнопку \"Начать настройку\""));
    for (i = 0; i < m_tuneFunctions.size(); ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(LBLFunc::New(w2, m_tuneFunctions.at(i).message, "tunemsg" + QString::number(i)));
        hlyout->addWidget(LBLFunc::New(w2, "", "tunemsgres" + QString::number(i)));
        hlyout->addStretch(1);
        w2lyout->addLayout(hlyout);
    }
    w2lyout->addSpacing(50);
    w2->setLayout(w2lyout);
    area->setWidgetResizable(true);
    area->setWidget(w2);
    lyout->addWidget(area);
    lyout->addWidget(LBLFunc::New(w2, "Настройка завершена!", "tunemsg" + QString::number(i)));
    for (i = 0; i < m_tuneFunctions.size(); ++i)
    {
        WDFunc::SetVisible(w2, "tunemsg" + QString::number(i), false);
        WDFunc::SetVisible(w2, "tunemsgres" + QString::number(i), false);
    }
    WDFunc::SetVisible(w2, "tunemsg" + QString::number(i), false);
    hlyout = new QHBoxLayout;
    hlyout->addStretch(300);
    hlyout->addWidget(HexPBFunc::New(
        this, "finishpb",
        [this]()
        {
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

QWidget *AbstractTuneDialog::mainUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(m_tuneTabWidget->set());
    w->setLayout(lyout);
    return w;
}

QWidget *AbstractTuneDialog::bottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(LBLFunc::New(this, "Регулировка"), 0);
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

void AbstractTuneDialog::setBac(DataBlock *block)
{
    AbsBac[block->block().blocknum] = block;
}

void AbstractTuneDialog::setTuneStep(u8 tuneStep)
{
    m_tuneStep = tuneStep;
}

void AbstractTuneDialog::waitNSeconds(int Seconds, bool isAllowedToStop)
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
    return (EMessageBox::password(this) ? Error::Msg::NoError : Error::Msg::PswCheckError);
}

int AbstractTuneDialog::addWidgetToTabWidget(QWidget *w, const QString &caption)
{
    return m_tuneTabWidget->addTabWidget(w, caption);
}

void AbstractTuneDialog::MsgSetVisible(AbstractTuneDialog::MsgTypes type, int msg, bool visible)
{
    QPixmap pm;
    switch (type)
    {
    case OkMsg:
        pm = QPixmap(":/tunes/ok.png");
        break;
    case ErMsg:
        pm = QPixmap(":/tunes/cross.png");
        break;
    case SkMsg:
        pm = QPixmap(":/tunes/hr.png");
        break;
    case NoMsg:
        WDFunc::SetVisible(this, "tunemsg" + QString::number(msg), visible);
        return;
    }
    WDFunc::SetVisible(this, "tunemsgres" + QString::number(msg), visible);
    LBLFunc::SetImage(this, "tunemsgres" + QString::number(msg), &pm);
}

void AbstractTuneDialog::MsgClear()
{
    int i;
    for (i = 0; i < m_tuneFunctions.size(); ++i)
    {
        MsgSetVisible(NoMsg, i, false);
        MsgSetVisible(OkMsg, i, false);
    }
    MsgSetVisible(NoMsg, i, false);
}

void AbstractTuneDialog::startTune()
{
    Error::Msg res;
    if (CheckPassword() != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Неверный пароль");
        return;
    }
    if (checkCalibrStep() != Error::Msg::NoError)
        return;
    WDFunc::SetEnabled(this, "starttune", false);
    WDFunc::SetEnabled(this, "finishpb", false);
    WDFunc::SetEnabled(this, "stoptune", true);
    // сохраняем на всякий случай настроечные коэффициенты
    readTuneCoefs();
    if (saveAllTuneCoefs() != Error::Msg::NoError)
    {
        if (!EMessageBox::question(this, "Сохранение настроечных коэффициентов не произведено, продолжать?"))
            return;
    }
    StdFunc::ClearCancel();
    MsgClear();    // очистка экрана с сообщениями
    setTuneMode(); // задание режима регулировки
    for (bStep = 0; bStep < m_tuneFunctions.size(); ++bStep)
    {
        MsgSetVisible(NoMsg, bStep);
        res = (this->*m_tuneFunctions.at(bStep).func)();
        switch (res)
        {
        case Error::Msg::NoError:
            MsgSetVisible(OkMsg, bStep);
            break;
        case Error::Msg::ResEmpty:
            MsgSetVisible(SkMsg, bStep);
            break;
        default:
        {
            if (StdFunc::IsCancelled())
                res = Error::Cancelled;
            MsgSetVisible(ErMsg, bStep);
#ifndef DEBUGISON
            WDFunc::SetEnabled(this, "starttune", true);
            WDFunc::SetEnabled(this, "stoptune", false);
            WDFunc::SetEnabled(this, "finishpb", true);
            qWarning() << m_tuneFunctions.at(bStep).message;
            loadAllTuneCoefs();
            setWorkMode();
            EMessageBox::error(this, Error::MsgStr[res]);
            return;
#endif
        }
        }
    }
    setWorkMode();
    MsgSetVisible(NoMsg, bStep); // выдаём надпись "Настройка завершена!"
    WDFunc::SetEnabled(this, "starttune", true);
    WDFunc::SetEnabled(this, "stoptune", false);
    WDFunc::SetEnabled(this, "finishpb", true);
    EMessageBox::information(this, "Настройка завершена!");
    TuneSequenceFile::saveTuneSequenceFile(m_tuneStep + 1); // +1 to let the next stage run
}

Error::Msg AbstractTuneDialog::setTuneMode()
{
    m_async->writeCommand(Interface::Commands::C_SetMode, 0x02);
    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::setWorkMode()
{
    m_async->writeCommand(Interface::Commands::C_SetMode, 0x00);
    return Error::Msg::NoError;
}

bool AbstractTuneDialog::checkFloat(const QString &name, double var, double base, double tolerance)
{
    if (!StdFunc::FloatIsWithinLimits(var, base, tolerance))
    {
        EMessageBox::warning(
            this, name + " лежит вне диапазона " + QString::number(base) + " +/- " + QString::number(tolerance));
        return false;
    }
    return true;
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
            return Error::Msg::FileWriteError;
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
        else
            return Error::Msg::ReadError;
    }
    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::writeTuneCoefs()
{
    return writeTuneCoefs(true);
}

Error::Msg AbstractTuneDialog::readTuneCoefs()
{
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
        readTuneCoefsByBac(it.key());
    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::sendChangedConfig(const std::vector<std::pair<QString, S2::valueType>> &changes) const
{
    S2::Configuration configCopy(m_config);
    for (const auto &[name, value] : changes)
        configCopy.setRecord(name, value);
    auto s2file = configCopy.toByteArray();
    return m_sync->writeFileSync(S2::FilesEnum::Config, s2file);
}

ReportData &AbstractTuneDialog::getReportData()
{
    return s_reportData;
}

Error::Msg AbstractTuneDialog::setCurrentsTo(const float value)
{
    S2::FLOAT_6t i2NomConfig { value, value, value, value, value, value };
    m_config.setRecord("I2nom", i2NomConfig);
    auto result = m_sync->writeConfigurationSync(m_config.toByteArray());
    // StdFunc::Wait(5000);
    // waitNSeconds(5);
    return result;
}

void AbstractTuneDialog::writeReportData(const QString &name, const QString &value)
{
    s_reportData.insert({ name, value });
}

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
        lyout->addWidget(LBLFunc::NewLBL(this, "Вопрос", "Записать регулировочные коэффициенты?"));
        QTabWidget *tw = new QTabWidget;
        for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
        {
            tw->addTab(it.value()->widget(false), it.value()->block().caption); // do not show buttons
            it.value()->updateWidget();
        }
        lyout->addWidget(tw);
        hlyout->addWidget(PBFunc::New(this, "", "Записать", this, &AbstractTuneDialog::writeTuneCoefsSlot));
        hlyout->addWidget(PBFunc::New(this, "", "Отмена",
            [this]()
            {
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
    return (StdFunc::IsCancelled() ? Error::Msg::Cancelled : Error::Msg::NoError);
}

void AbstractTuneDialog::writeTuneCoefsSlot()
{
    for (QMap<int, DataBlock *>::Iterator it = AbsBac.begin(); it != AbsBac.end(); ++it)
    {
        if (it.value()->writeBlockToModule() != Error::Msg::NoError)
        {
            CancelTune();
            return;
        }
    }
    EMessageBox::information(this, "Коэффициенты записаны успешно!");
    emit generalEventReceived();
}

Error::Msg AbstractTuneDialog::checkCalibrStep()
{
    if (!TuneSequenceFile::contains("step"))
    {
        EMessageBox::warning(this,
            "Не выполнены предыдущие шаги регулировки, пожалуйста,\n"
            "начните заново с шага 1");
        return Error::Msg::ResEmpty;
    }
    int calibrstep = TuneSequenceFile::value("step").toInt();
    if (calibrstep < m_tuneStep)
    {
        EMessageBox::warning(this,
            "Перед выполнением шага " + QString::number(m_tuneStep) + " необходимо\nвыполнить шаг "
                + QString::number(calibrstep) + "!");

        return Error::Msg::ResEmpty;
    }
    return Error::Msg::NoError;
}

Error::Msg AbstractTuneDialog::saveWorkConfig()
{
    QByteArray ba;
    auto status = m_sync->readFileSync(S2::FilesEnum::Config, ba);
    if (status != Error::Msg::NoError)
        return Error::Msg::ReadError;
    return Files::SaveToFile(Settings::workDir() + m_device->getUID() + ".cf", ba);
}

Error::Msg AbstractTuneDialog::loadWorkConfig()
{
    QByteArray ba;
    auto res = Files::LoadFromFile(Settings::workDir() + m_device->getUID() + ".cf", ba);
    if (res != Error::Msg::NoError)
        return res;
    else
    {
        res = m_sync->writeFileSync(S2::FilesEnum::Config, ba);
        // StdFunc::Wait(5000);
        // waitNSeconds(5); // device reconnect
        return res;
    }
}

void AbstractTuneDialog::CancelTune()
{
    StdFunc::Cancel();
    emit cancelled();
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
        StdFunc::Cancel();
    QDialog::keyPressEvent(e);
}
