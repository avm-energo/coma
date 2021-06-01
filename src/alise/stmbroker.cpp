#include "stmbroker.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/helper.h"
#include "../gen/modules.h"
#include "../gen/stdfunc.h"
#include "../interfaces/baseinterface.h"
#include "../interfaces/protocom.h"
#include "../interfaces/usbhidportinfo.h"
#include "controller.h"
#include "grpc_sync_server.h"
#include "helper.h"
#include "timesyncronizer.h"

#include <QRandomGenerator>
void printbs(const DataTypes::BitStringStruct &st)
{
    std::cout << "BitString {"
              << "Addr:" << st.sigAdr << ","
              << "Val:" << st.sigVal << ","
              << "Qual:" << st.sigQuality << " }" << std::endl;
}

void alise_test()
{
    const auto devices = UsbHidPortInfo::devicesFound(0x0483);
    if (devices.isEmpty())
    {
        std::cout << "No devices" << std::endl;
        return;
    }
    for (const auto &device : devices)
    {
        std::cout << "Vendor id:" << std::hex << device.vendor_id << std::dec << " : "
                  << "Product id:" << std::hex << device.product_id << std::dec << " : "
                  << "Serial number:" << device.serial.toStdString() << std::endl;
    }
    BaseInterface::InterfacePointer device;
    device = BaseInterface::InterfacePointer(new Protocom());
    BaseInterface::setIface(std::move(device));

    auto protocom = static_cast<Protocom *>(BaseInterface::iface());
    if (!protocom->start(devices.first()))
        std::cout << "Couldnt connect" << std::endl;
    const auto &manager = DataManager::GetInstance();
    QObject::connect(&manager, &DataManager::bitStringReceived, &printbs);

    TimeSyncronizer timeSync;
    if (timeSync.isNtpSync())
        std::cout << "Ntp enabled";
    QObject::connect(&manager, &DataManager::timeReceived, &timeSync, &TimeSyncronizer::handleTime);
    QObject::connect(&timeSync, &TimeSyncronizer::sendTime, protocom, [&](const timespec &time) {
        protocom->writeTime(time);
        //  protocom->reqTime();
    });
    protocom->reqBSI();
    protocom->reqTime();
    StdFunc::Wait(1000);
    std::cout << "Finished" << std::endl;
    //    QTimer::singleShot(0, []() {
    //        // do stuff
    //        QCoreApplication::exit(0);
    //    });
}

StmBroker::StmBroker(QObject *parent) : QObject(parent)
{
}

bool StmBroker::connectToStm()
{
    const auto devices = UsbHidPortInfo::devicesFound(0x0483);
    if (devices.isEmpty())
    {
        std::cout << "No devices" << std::endl;
        return false;
    }
    for (const auto &device : devices)
    {
        std::cout << "Vendor id:" << std::hex << device.vendor_id << std::dec << " : "
                  << "Product id:" << std::hex << device.product_id << std::dec << " : "
                  << "Serial number:" << device.serial.toStdString() << std::endl;
    }
    BaseInterface::InterfacePointer device;
    device = BaseInterface::InterfacePointer(new Protocom());
    BaseInterface::setIface(std::move(device));

    m_interface = static_cast<Protocom *>(BaseInterface::iface());
    if (!m_interface->start(devices.first()))
    {
        std::cout << "Couldn't connect" << std::endl;
        return false;
    }
    const auto &board = Board::GetInstance();
    const auto &manager = DataManager::GetInstance();
    QObject::connect(&manager, &DataManager::bitStringReceived, &board, &Board::update);
    QObject::connect(&manager, &DataManager::blockReceived, this, [](const auto bs) { qDebug() << bs; });
    m_timer.setInterval(1000);
    m_testTimer.setInterval(10000);
    m_timer.start();
    m_testTimer.start();
    QObject::connect(&m_timer, &QTimer::timeout, this, &StmBroker::checkPowerUnit);
    QObject::connect(&m_testTimer, &QTimer::timeout, this,
        [this] { setIndication(static_cast<alise::Health_Code>(QRandomGenerator::global()->bounded(0, 8))); });
    return true;
}

void StmBroker::checkPowerUnit()
{
    Q_CHECK_PTR(m_interface);
    m_interface->writeCommand(Queries::QUSB_ReqBlkData, AVTUK_14::DiscretePowerBlock);
}

void StmBroker::setIndication(alise::Health_Code code)
{
    const auto indication = transform(code);
    DataTypes::BlockStruct block;
    block.ID = AVTUK_14::IndicationBlock;
    block.data.resize(sizeof(decltype(indication)));
    memcpy(block.data.data(), &indication, sizeof(indication));
    qDebug() << block;
    m_interface->writeCommand(Queries::QC_WriteUserValues, QVariant::fromValue(block));
}

void StmBroker::setTime(alise::UnixTimestamp time)
{
    Q_CHECK_PTR(m_interface);
    m_interface->writeTime(transform(time));
}

void StmBroker::getTime(alise::UnixTimestamp &time)
{
    Q_CHECK_PTR(m_interface);
    timespec ts;
    auto status = m_interface->reqTimeSync(&ts, sizeof(timespec));
    Q_ASSERT(status == Error::Msg::NoError);
    if (status == Error::Msg::NoError)
    {
        time.set_sec(ts.tv_sec);
        time.set_nsec(ts.tv_nsec);
    }
}

AVTUK_14::Indication StmBroker::transform(alise::Health_Code code) const
{
    constexpr auto maxFreq = 4;
    constexpr auto minFreq = 1;
    switch (code)
    {
    case alise::Health_Code_Startup:
    {
        return { 0, 0, 1, maxFreq };
    }
    case alise::Health_Code_Work:
    {
        return { 1, minFreq, 0, 0 };
    }
    case alise::Health_Code_Update:
    {
        return { 1, minFreq, static_cast<uint8_t>(code), maxFreq };
    }
    case alise::Health_Code_StartupFail:
    {
        return { 1, minFreq, static_cast<uint8_t>(code), maxFreq };
    }
    case alise::Health_Code_NoProject:
    {
        return { 1, minFreq, static_cast<uint8_t>(code), maxFreq };
    }
    case alise::Health_Code_ProjectError:
    {
        return { 1, minFreq, static_cast<uint8_t>(code), maxFreq };
    }
    case alise::Health_Code_NoFirmware:
    {
        return { 1, minFreq, static_cast<uint8_t>(code), maxFreq };
    }
    case alise::Health_Code_BootloaderError:
    {
        return { 1, minFreq, static_cast<uint8_t>(code), maxFreq };
    }
    case alise::Health_Code_SettingsError:
    {
        return { 1, minFreq, static_cast<uint8_t>(code), maxFreq };
    }
    default:
        return transform(alise::Health_Code_SettingsError);
    }
}
