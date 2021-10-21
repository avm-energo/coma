#include "modbus.h"

#include "../gen/error.h"
#include "../gen/registers.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "modbusthread.h"
#include "serialport.h"
#include "settingstypes.h"

#include <QDebug>
#include <QStandardPaths>
#include <QThread>
#include <QWaitCondition>
#include <algorithm>

using namespace CommandsMBS;

ModBus::ModBus(QObject *parent) : BaseInterface(parent)
{
}

ModBus::~ModBus()
{
}

bool ModBus::start(const ConnectStruct &connectStruct)
{
    Log->Init(QString(metaObject()->className()) + ".log");
    Log->info(logStart);
    Q_ASSERT(std::holds_alternative<SerialPortSettings>(connectStruct.settings));
    qInfo() << metaObject()->className() << "connect";
    if (!std::holds_alternative<SerialPortSettings>(connectStruct.settings))
        return false;

    const auto &st = std::get<SerialPortSettings>(connectStruct.settings);
    SerialPort *port = new SerialPort(this);
    ModbusThread *parser = new ModbusThread;
    parser->setDelay(obtainDelay(st.Baud));
    parser->setDeviceAddress(st.Address);
    QThread *thr = new QThread;
    thr->setObjectName("Modbus thread");

    connect(thr, &QThread::started, parser, &ModbusThread::run);
    connect(parser, &ModbusThread::finished, thr, &QThread::quit);
    connect(thr, &QThread::finished, port, &SerialPort::disconnect);
    connect(thr, &QThread::finished, thr, &QObject::deleteLater);
    connect(thr, &QThread::finished, parser, &QObject::deleteLater);
    connect(port, &SerialPort::read, parser, &ModbusThread::parseReply);
    connect(parser, &ModbusThread::write, port, &SerialPort::writeBytes);
    connect(parser, &ModbusThread::clearBuffer, port, &SerialPort::clear);
    connect(port, &SerialPort::error, this, &ModBus::sendReconnectSignal);
    connect(this, &BaseInterface::reconnect, port, &SerialPort::reconnect);
    connect(port, &SerialPort::connected, port, [=] {
        setState(BaseInterface::Run);
        parser->moveToThread(thr);
        thr->start();
        StdFunc::Wait(1000);
    });
    if (!port->init(st))
    {
        port->deleteLater();
        parser->deleteLater();
        thr->deleteLater();
        return false;
    }
    //  setState(State::Run);

    return true;
}

void ModBus::sendReconnectSignal()
{
    switch (state())
    {
    case BaseInterface::Run:
        setState(BaseInterface::State::Wait);
        break;
    case BaseInterface::Wait:
        emit reconnect();
        break;
    default:
        break;
    }
}

bool ModBus::isValidRegs(const CommandsMBS::CommandStruct &cmd) const
{
    const auto &st = settings<InterfaceInfo<CommandsMBS::ModbusGroup>>();
    Q_ASSERT(st.dictionary().contains(cmd.adr));
    const auto values = st.dictionary().values(cmd.adr);
    for (const auto &val : values)
    {
        if ((val.count == cmd.quantity) && (val.function == cmd.cmd))
            return true;
    }
    return false;
}

bool ModBus::isValidRegs(const quint32 sigAdr, const quint32 sigCount) const
{
    const auto &st = settings<InterfaceInfo<CommandsMBS::ModbusGroup>>();
    Q_ASSERT(st.dictionary().contains(sigAdr));
    const auto values = st.dictionary().values(sigAdr);
    for (const auto &val : values)
    {
        if ((val.count == sigCount) && (val.startAddr == sigAdr))
            return true;
    }
    return false;
}

CommandsMBS::TypeId ModBus::type(const quint32 addr, const quint32 count) const
{
    Q_UNUSED(count);
    const auto &st = settings<InterfaceInfo<CommandsMBS::ModbusGroup>>();
    return st.dictionary().value(addr).dataType;
}

TypeId ModBus::type(const quint32 addr, const quint32 count, const CommandsMBS::Commands cmd) const
{
    Q_UNUSED(count);
    const auto &st = settings<InterfaceInfo<CommandsMBS::ModbusGroup>>();
    const auto values = st.dictionary().values(addr);
    for (const auto &val : values)
    {
        if (val.function == cmd && val.count == count)
            return val.dataType;
    }
    return TypeId::None;
}

void ModBus::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    const quint16 addr = sigAdr;
    const quint8 count = (sigCount * 2);
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::MBS_READINPUTREGISTER, //
        addr,                                         //
        count,                                        //
        {},                                           //
        type(addr, count),                            //
        __PRETTY_FUNCTION__                           //
    };
    Q_ASSERT(isValidRegs(inp));
    DataManager::addToInQueue(inp);
}

void ModBus::reqBSI()
{
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::MBS_READINPUTREGISTER, //
        Regs::bsiStartReg,                            //
        static_cast<quint8>(Regs::bsiEndReg * 2),     //
        {},                                           //
        TypeId::Uint32,                               //
        __PRETTY_FUNCTION__                           //
    };
    DataManager::addToInQueue(inp);
}

void ModBus::reqBSIExt()
{
    constexpr auto regCount = sizeof(Modules::StartupInfoBlockExt0) / sizeof(quint32);
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::MBS_READINPUTREGISTER, //
        31,                                           //
        static_cast<quint8>(regCount * 2),            //
        {},                                           //
        TypeId::Uint32,                               //
        __PRETTY_FUNCTION__                           //
    };
    DataManager::addToInQueue(inp);
}

void ModBus::reqFile(quint32 filenum, FileFormat format)
{
    Q_UNUSED(filenum)
    Q_UNUSED(format)
}

void ModBus::writeFile(quint32 filenum, const QByteArray &file)
{
    Q_UNUSED(filenum)
    Q_UNUSED(file)
}

void ModBus::reqTime()
{
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::MBS_READHOLDINGREGISTERS, //
        Regs::timeReg,                                   //
        2,                                               //
        {},                                              //
        type(Regs::timeReg, 2),                          //
        __PRETTY_FUNCTION__                              //
    };
    Q_ASSERT(isValidRegs(inp));
    DataManager::addToInQueue(inp);
}

void ModBus::writeTime(quint32 time)
{
    QByteArray timeArray = packReg(time);

    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::MBS_WRITEMULTIPLEREGISTERS, //
        Regs::timeReg,                                     //
        2,                                                 //
        timeArray,                                         //
        TypeId::None,                                      //
        __PRETTY_FUNCTION__                                //
    };
    Q_ASSERT(isValidRegs(inp));
    DataManager::addToInQueue(inp);
}

void ModBus::writeFloat(const DataTypes::FloatStruct &flstr)
{
    Q_ASSERT(false && "Dont use this before test");
    // now write floats to the out sigArray
    QByteArray sigArray = packReg(flstr.sigVal);

    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::MBS_WRITEMULTIPLEREGISTERS, //
        static_cast<quint16>(flstr.sigAdr),                //
        static_cast<quint16>(sigArray.size() / 2),         //
        sigArray,                                          //
        TypeId::None,                                      //
        __PRETTY_FUNCTION__                                //
    };
    // Q_ASSERT(isValidRegs(inp));
    DataManager::addToInQueue(inp);
}

void ModBus::writeCommand(Queries::Commands cmd, QVariant item)
{
    switch (cmd)
    {
    case Queries::QC_WriteUserValues:
    {
        Q_ASSERT(item.canConvert<DataTypes::FloatStruct>());
        if (!item.canConvert<DataTypes::FloatStruct>())
            return;
        writeFloat(item.value<DataTypes::FloatStruct>());
        break;
    }
    case Queries::QC_ReqAlarms:
    {
        Q_ASSERT(item.canConvert<DataTypes::Signal>());
        // get sigAdr from the first var and sigCount - from the second one
        if (!item.canConvert<DataTypes::Signal>()) // must be sigAdr & sigCount
            return;
        const auto signal = qvariant_cast<DataTypes::Signal>(item);
        CommandsMBS::CommandStruct inp {
            CommandsMBS::Commands::MBS_READCOILS, //
            signal.addr,                          //
            static_cast<quint16>(signal.value),   //
            {},                                   //
            type(signal.addr, signal.value),      //
            __PRETTY_FUNCTION__                   //
        };
        // Q_ASSERT(isValidRegs(inp));
        DataManager::addToInQueue(inp);
        break;
    }
    default:
        Q_ASSERT(false && "Unsupported in Modbus");
        break;
    }
}

void ModBus::writeCommand(Queries::Commands cmd, const QVariantList &list)
{
    switch (cmd)
    {
    case Queries::QC_WriteUserValues:
    {
        Q_ASSERT(list.first().canConvert<DataTypes::FloatStruct>());
        const quint16 start_addr = list.first().value<DataTypes::FloatStruct>().sigAdr;
        const auto &st = settings<InterfaceInfo<CommandsMBS::ModbusGroup>>();
        Q_ASSERT(isValidRegs(start_addr, list.size() * 2));
        auto group = st.dictionary().value(start_addr);
        bool found = false;
        auto dictionary = st.dictionary();
        auto it = dictionary.cbegin();
        while (it != dictionary.cend() && !found)
        {
            if (it.value().id.contains(group.id.remove(0, 1)))
                if (it.value() != group)
                {
                    group = it.value();
                    found = true;
                }
            ++it;
        }
        Q_ASSERT(found);
        QByteArray sigArray;
        for (auto &i : list)
        {
            auto flstr = i.value<DataTypes::FloatStruct>();
            // now write floats to the out sigArray

            sigArray.push_back(packReg(flstr.sigVal));
        }

        CommandsMBS::CommandStruct inp {
            CommandsMBS::Commands::MBS_WRITEMULTIPLEREGISTERS, //
            static_cast<quint16>(group.startAddr),             //
            static_cast<quint16>(sigArray.size() / 2),         //
            sigArray,                                          //
            TypeId::None,                                      //
            __PRETTY_FUNCTION__                                //
        };
        qDebug() << inp;
        Q_ASSERT(isValidRegs(inp));
        DataManager::addToInQueue(inp);
        break;
    }
    default:
        Q_ASSERT(false && "Not realized");
        break;
    }
}

void ModBus::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::MBS_READINPUTREGISTER, //
        static_cast<quint16>(sigAdr),                 //
        static_cast<quint8>(sigCount * 2),            //
        {},                                           //
        type(sigAdr, sigCount * 2),                   //
        __PRETTY_FUNCTION__                           //
    };
    Q_ASSERT(isValidRegs(inp));
    DataManager::addToInQueue(inp);
}

InterfaceSettings ModBus::parseSettings(QDomElement domElement) const
{
    return BaseInterface::parseSettings<CommandsMBS::ModbusGroup>(domElement);
}
