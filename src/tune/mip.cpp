#include "mip.h"

#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QEventLoop>
#include <QSettings>
#include <QThread>
#include <gen/settings.h>
#include <gen/stdfunc.h>
#include <interfaces/connection.h>
#include <interfaces/ifaces/ethernet.h>
#include <interfaces/parsers/iec104parser.h>
#include <interfaces/types/settingstypes.h>

Mip::Mip(bool withGUI, MType moduleType, QWidget *parent)
    : QObject(parent), m_parent(parent), m_withGUI(withGUI), m_moduleType(moduleType)
{
}

void Mip::updateData(const DataTypes::FloatStruct &fl)
{
    if (fl.sigAdr < 46)
    {
        float *mipdata = reinterpret_cast<float *>(&m_mipData);
        *(mipdata + fl.sigAdr) = fl.sigVal;
    }
    if (m_withGUI)
        m_widget->updateFloatData(fl);
}

Mip::MipDataStruct Mip::getData()
{
    return m_mipData;
}

void Mip::setupWidget()
{
    m_widget = new UWidget;
    QGridLayout *lyout = new QGridLayout;
    for (int i = 0; i < 3; ++i)
    {
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 1),
                             "Частота ф. " + QString::number(i + 10, 36).toUpper(), true),
            0, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 4),
                             "Фазное напряжение ф. " + QString::number(i + 10, 36).toUpper(), true),
            1, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 19),
                             "Фазное напряжение ф. " + QString::number(i + 10, 36).toUpper(), true),
            2, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 7),
                             "Фазный ток ф. " + QString::number(i + 10, 36).toUpper(), true),
            3, i);
    }
    lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, "10", "Ток N", true), 4, 0);
    for (int i = 0; i < 3; ++i)
    {
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 11),
                             "Угол ф. " + QString::number(i + 10, 36).toUpper(), true),
            5, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 22),
                             "Активная мощность ф. " + QString::number(i + 10, 36).toUpper(), true),
            6, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 26),
                             "Реактивная мощность ф. " + QString::number(i + 10, 36).toUpper(), true),
            7, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 30),
                             "Полная мощность ф. " + QString::number(i + 10, 36).toUpper(), true),
            8, i);
    }
    lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, "17", "Температура", true), 9, 0);
    lyout->addWidget(WDFunc::NewPB(m_widget, "", "Далее",
                         [=] {
                             stop();
                             m_widget->close();
                         }),
        10, 0);
    m_widget->setLayout(lyout);
}

bool Mip::start()
{
    using namespace settings;
    QSettings sets;
    IEC104Settings settings;
    settings.ip = sets.value(regMap[MIPIP].name, regMap[MIPIP].defValue).toString();
    settings.port = 2404; // TODO: в настройки...
    settings.bsAddress = sets.value(regMap[MIPAddress].name, regMap[MIPAddress].defValue).toUInt();
    if (!initConnection(settings))
        return false;
    if (m_withGUI)
    {
        setupWidget();
        m_widget->engine()->addFloat({ 0, 46 });
        connect(m_widget->engine(), &ModuleDataUpdater::itsTimeToUpdateFloatSignal, this, &Mip::updateData);
        m_widget->engine()->setUpdatesEnabled(true);
        m_widget->show();
    }
    else
    {
        // Отключим обновление виджета по умолчанию
        m_updater->setUpdatesEnabled(false);
        m_updater->addFloat({ 0, 46 });
        connect(m_updater, &ModuleDataUpdater::itsTimeToUpdateFloatSignal, this, &Mip::updateData);
        m_updater->setUpdatesEnabled(true);
        m_updateTimer = new QTimer;
        m_updateTimer->setInterval(1000);
        connect(m_updateTimer, &QTimer::timeout, m_updater, &ModuleDataUpdater::requestUpdates);
        m_updateTimer->start();
    }
    return true;
}

void Mip::stop()
{
    if (m_withGUI)
        m_widget->engine()->setUpdatesEnabled(false);
    else
    {
        m_updateTimer->stop();
        m_updateTimer->deleteLater();
        m_updater->setUpdatesEnabled(false);
    }
    m_iface->close();
    emit finished();
}

bool Mip::initConnection(const IEC104Settings &settings)
{
    using namespace DataTypes;
    auto conn = new Connection(this);
    // conn->settings()->addGroup(Iec104Group { 0, 46, 0, 0 });
    conn->connection(this, //
        [this](const FloatWithTimeStruct &fs) {
            updateData(FloatStruct { fs.sigAdr, fs.sigVal, fs.sigQuality });
        });
    m_iface = new Ethernet(settings);
    m_updater = new ModuleDataUpdater(conn, this);
    auto ifaceThread = new QThread;
    auto parserThread = new QThread;
    auto parser = new IEC104Parser(conn->getQueue());
    parser->setBaseAdr(settings.bsAddress);
    // Обмен данными
    QObject::connect(m_iface, &BaseInterface::dataReceived, //
        parser, &IEC104Parser::processReadBytes, Qt::QueuedConnection);
    QObject::connect(parser, &IEC104Parser::writeData, //
        m_iface, &BaseInterface::writeData, Qt::QueuedConnection);
    QObject::connect(m_iface, &BaseInterface::finished, //
        parser, &IEC104Parser::stop, Qt::DirectConnection);
    QObject::connect(parser, &IEC104Parser::responseSend, //
        conn, &Connection::responseHandle, Qt::DirectConnection);
    // Потоки
    QObject::connect(ifaceThread, &QThread::started, m_iface, &BaseInterface::poll);
    QObject::connect(parserThread, &QThread::started, parser, &IEC104Parser::run);
    QObject::connect(m_iface, &BaseInterface::finished, ifaceThread, &QThread::quit);
    QObject::connect(m_iface, &BaseInterface::finished, parserThread, &QThread::quit);
    QObject::connect(ifaceThread, &QThread::finished, m_iface, &QObject::deleteLater);
    QObject::connect(parserThread, &QThread::finished, parser, &QObject::deleteLater);
    QObject::connect(ifaceThread, &QThread::finished, &QObject::deleteLater);
    QObject::connect(parserThread, &QThread::finished, &QObject::deleteLater);
    QObject::connect(m_iface, &BaseInterface::started, m_iface, [=] {
        qInfo() << m_iface->metaObject()->className() << " connected";
        m_iface->moveToThread(ifaceThread);
        parser->moveToThread(parserThread);
        ifaceThread->start();
        parserThread->start();
    });

    if (!m_iface->connect())
    {
        m_iface->close();
        m_iface->deleteLater();
        parser->deleteLater();
        ifaceThread->deleteLater();
        parserThread->deleteLater();
        return false;
    }
    return true;
}

Error::Msg Mip::check()
{
    double ithr, u, uthr;

    assert((m_moduleType == MType::MTM_81) || (m_moduleType == MType::MTM_82) || (m_moduleType == MType::MTM_83));
    switch (m_moduleType)
    {
    case MType::MTM_81:
        u = 60.0;
        uthr = 0.05;
        iNom = 0;
        ithr = MAXFLOAT;
        break;
    case MType::MTM_82:
        u = 60.0;
        uthr = 0.05;
        ithr = 0.05;
        break;
    case MType::MTM_83:
        u = 0;
        uthr = MAXFLOAT;
        ithr = 0.05;
        break;
    default:
        return Error::GeneralError;
    }

    double ValuesToCheck[10] = { 0.0, 50.0, 50.0, 50.0, u, u, u, iNom, iNom, iNom };
    double ThresholdsToCheck[10] = { 0.0, 0.05, 0.05, 0.05, uthr, uthr, uthr, ithr, ithr, ithr };
    double *VTC, *TTC;
    VTC = ValuesToCheck;
    TTC = ThresholdsToCheck;
    for (int i = 0; i < 10; i++)
    {
        float *mipdata = reinterpret_cast<float *>(&m_mipData);
        if (!StdFunc::FloatIsWithinLimits(*(mipdata + i), *VTC, *TTC))
        {
            if (m_withGUI)
                EMessageBox::warning(m_parent,
                    "Несовпадение МИП по параметру " + QString::number(i)
                        + ". Измерено: " + QString::number(*mipdata, 'f', 4)
                        + ", должно быть: " + QString::number(*VTC, 'f', 4) + " +/- " + QString::number(*TTC, 'f', 4));
            else
                qDebug() << "Несовпадение МИП по параметру " + QString::number(i)
                         << ". Измерено: " + QString::number(*mipdata, 'f', 4)
                         << ", должно быть: " + QString::number(*VTC, 'f', 4) << " +/- "
                         << QString::number(*TTC, 'f', 4);
            return Error::Msg::GeneralError;
        }
        ++VTC;
        ++TTC;
    }
    return Error::Msg::NoError;
}

void Mip::setModuleType(MType type)
{
    m_moduleType = type;
}

void Mip::setNominalCurrent(float inom)
{
    iNom = inom;
}

UWidget *Mip::widget()
{
    return m_widget;
}

Mip::MipDataStruct Mip::takeOneMeasurement(float i2nom)
{
    setNominalCurrent(i2nom);
    start();
    QEventLoop el;
    connect(this, &Mip::finished, &el, &QEventLoop::quit);
    el.exec();
    stop();
    return m_mipData;
}
