#include "modbus.h"

#include "../s2/s2.h"
#include "interfacesettings.h"
#include "modbusthread.h"
#include "serialport.h"
#include "settingstypes.h"

#include <QDebug>
#include <QStandardPaths>
#include <QThread>
#include <QWaitCondition>
#include <algorithm>
#include <gen/error.h>
#include <gen/registers.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>

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
    qInfo() << metaObject()->className() << "is connecting...";
    if (!std::holds_alternative<SerialPortSettings>(connectStruct.settings))
        return false;

    const auto &st = std::get<SerialPortSettings>(connectStruct.settings);
    auto thr = new QThread;
    auto port = new SerialPort(this);
    auto parser = new ModbusThread;
    parser->setDelay(obtainDelay(st.Baud));
    parser->setDeviceAddress(st.Address);
    thr->setObjectName("Modbus thread");

    connect(thr, &QThread::started, parser, &ModbusThread::run);
    connect(parser, &ModbusThread::finished, thr, &QThread::quit);
    connect(thr, &QThread::finished, port, &SerialPort::disconnect);
    connect(thr, &QThread::finished, port, &QObject::deleteLater);
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
    return true;
}

void ModBus::sendReconnectSignal()
{
    switch (state())
    {
    case BaseInterface::State::Run:
        setState(BaseInterface::State::Wait);
        break;
    case BaseInterface::State::Wait:
        emit reconnect();
        break;
    default:
        break;
    }
}

/// \brief Проверка отправляемой посылки на наличие описания таковой в конфигурации модуля

bool ModBus::isValidRegs(CommandsMBS::CommandStruct &cmd) const
{
    const auto &st = settings<InterfaceInfo<ModbusGroup>>();
    Q_ASSERT(st.dictionary().contains(cmd.adr));
    if (st.dictionary().contains(cmd.adr))
    {
        const auto values = st.dictionary().values(cmd.adr);
        for (const auto &val : values)
        {
            if ((val.function == cmd.cmd) && (val.count == cmd.quantity))
                //            {
                //                if (val.count == cmd.quantity * 2)
                //                    return true;

                //                if (val.count == cmd.quantity)
                return true;
            //            }
        }
    }
    return false;
}

/// \brief Проверка регистров на наличие описания таковых в конфигурации модуля

bool ModBus::isValidRegs(const quint32 sigAdr, const quint32 sigCount) const
{
    const auto &st = settings<InterfaceInfo<ModbusGroup>>();
    if (!st.dictionary().contains(sigAdr))
        return false;
    const auto values = st.dictionary().values(sigAdr);
    for (const auto &val : values)
    {
        if ((val.startAddr == sigAdr) && (val.count == sigCount))
            return true;
    }
    return false;
}

CommandsMBS::TypeId ModBus::type(const quint32 addr) const
{
    const auto &st = settings<InterfaceInfo<ModbusGroup>>();
    return st.dictionary().value(addr).dataType;
}

TypeId ModBus::type(const quint32 addr, const quint32 count, const CommandsMBS::Commands cmd) const
{
    const auto &st = settings<InterfaceInfo<ModbusGroup>>();
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
    const quint8 count = (sigCount * 2); // startup registers are float (2 ints long)
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::ReadInputRegister, //
        addr,                                     //
        count,                                    //
        {},                                       //
        type(addr),                               //
        __PRETTY_FUNCTION__                       //
    };
    Q_ASSERT(isValidRegs(inp));
    DataManager::GetInstance().addToInQueue(inp);
}

/// TODO: Переписать
void ModBus::reqBSI()
{
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::ReadInputRegister, //
        Regs::bsiStartReg,                        //
        static_cast<quint8>(Regs::bsiEndReg * 2), //
        {},                                       //
        TypeId::Uint32,                           //
        __PRETTY_FUNCTION__                       //
    };
    DataManager::GetInstance().addToInQueue(inp);
}

/// TODO: Переписать
void ModBus::reqBSIExt()
{
    constexpr auto regCount = sizeof(Modules::StartupInfoBlockExt0) / sizeof(quint32);
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::ReadInputRegister, //
        Regs::bsiExtStartReg,                     //
        static_cast<quint8>(regCount * 2),        //
        {},                                       //
        TypeId::Uint32,                           //
        __PRETTY_FUNCTION__                       //
    };
    DataManager::GetInstance().addToInQueue(inp);
}

/// TODO: требуется реализация
void ModBus::reqFile(quint32 filenum, FileFormat format)
{
    Q_UNUSED(filenum)
    Q_UNUSED(format)
}

/// TODO: требуется реализация
void ModBus::writeFile(quint32 filenum, const QByteArray &file)
{
    Q_UNUSED(filenum)
    Q_UNUSED(file)
}

void ModBus::reqTime()
{
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::ReadHoldingRegisters, //
        Regs::timeReg,                               //
        2,                                           //
        {},                                          //
        type(Regs::timeReg),                         //
        __PRETTY_FUNCTION__                          //
    };
    Q_ASSERT(isValidRegs(inp));
    DataManager::GetInstance().addToInQueue(inp);
}

void ModBus::writeTime(quint32 time)
{
    QByteArray timeArray = packReg(time);

    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::WriteMultipleRegisters, //
        Regs::timeReg,                                 //
        2,                                             //
        timeArray,                                     //
        TypeId::None,                                  //
        __PRETTY_FUNCTION__                            //
    };
    Q_ASSERT(isValidRegs(inp));
    DataManager::GetInstance().addToInQueue(inp);
}

void ModBus::writeFloat(const DataTypes::FloatStruct &flstr)
{
    //    Q_ASSERT(false && "Dont use this before test");
    // now write floats to the out sigArray
    QByteArray sigArray = packReg(flstr.sigVal);
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::WriteMultipleRegisters, //
        static_cast<quint16>(flstr.sigAdr),            //
        static_cast<quint16>(sigArray.size()),         //
        sigArray,                                      //
        TypeId::None,                                  //
        __PRETTY_FUNCTION__                            //
    };
    Q_ASSERT(isValidRegs(inp));
    DataManager::GetInstance().addToInQueue(inp);
}

void ModBus::writeInt16(const quint32 addr, const quint16 value)
{
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::WriteMultipleRegisters, //
        static_cast<quint16>(addr),                    //
        1,                                             //
        packReg(value),                                //
        TypeId::None,                                  //
        __PRETTY_FUNCTION__                            //
    };
    // Q_ASSERT(isValidRegs(inp));
    DataManager::GetInstance().addToInQueue(inp);
}

const quint8 ModBus::obtainDelay(const quint32 baudRate) const
{
    switch (baudRate)
    {
    case 2400:
        return 16;
    case 4800:
        return 8;
    case 9600:
        return 4;
    case 19200:
        return 3;
    default:
        return 2;
    }
}

CommandsMBS::CommandRegisters ModBus::getAddrByCommand(Queries::Commands cmd)
{
    static const QMap<Queries::Commands, CommandsMBS::CommandRegisters> cmdMap {
        { Queries::QC_SetStartupValues, CommandsMBS::SetStartupValues },       //
        { Queries::QC_SetStartupPhaseA, CommandsMBS::SetStartupPhaseA },       //
        { Queries::QC_SetStartupPhaseB, CommandsMBS::SetStartupPhaseB },       //
        { Queries::QC_SetStartupPhaseC, CommandsMBS::SetStartupPhaseC },       //
        { Queries::QC_SetStartupUnbounced, CommandsMBS::SetStartupUnbounced }, //
        { Queries::QC_SetTransOff, CommandsMBS::SetTransOff },                 //
        { Queries::QC_ClearStartupValues, CommandsMBS::ClearStartupValues },   //
        { Queries::QC_ClearStartupError, CommandsMBS::ClearStartupSetError }   //
    };
    return cmdMap.value(cmd);
}

void ModBus::writeCommand(Queries::Commands cmd, QVariant item)
{
    switch (cmd)
    {
    case Queries::QC_ReqAlarms:
    {
        Q_ASSERT(item.canConvert<DataTypes::Signal>());
        // get sigAdr from the first var and sigCount - from the second one
        if (!item.canConvert<DataTypes::Signal>()) // must be sigAdr & sigCount
            return;
        const auto signal = qvariant_cast<DataTypes::Signal>(item);
        CommandsMBS::CommandStruct inp {
            CommandsMBS::Commands::ReadCoils,   //
            signal.addr,                        //
            static_cast<quint16>(signal.value), //
            {},                                 //
            type(signal.addr),                  //
            __PRETTY_FUNCTION__                 //
        };
        // Q_ASSERT(isValidRegs(inp));
        DataManager::GetInstance().addToInQueue(inp);
        break;
    }
    case Queries::QC_WriteUserValues:
    {
        Q_ASSERT(item.canConvert<DataTypes::FloatStruct>());
        if (!item.canConvert<DataTypes::FloatStruct>())
            return;
        writeFloat(item.value<DataTypes::FloatStruct>());
        break;
    }
    default:
    {
        auto cmdAddr = getAddrByCommand(cmd);
        if (cmdAddr)
        {
            auto value = static_cast<quint16>(item.value<bool>());
            writeInt16(cmdAddr, value);
        }
        else
            qCritical() << "Unsupported command in Modbus: " << Error::WrongCommandError;
        break;
    }
    }
}

void ModBus::writeCommand(Queries::Commands cmd, const QVariantList &list)
{
    switch (cmd)
    {
    case Queries::QC_WriteUserValues:
    {
        Q_ASSERT(list.first().canConvert<DataTypes::FloatStruct>());
        quint16 some_addr = list.first().value<DataTypes::FloatStruct>().sigAdr;
        const auto &st = settings<InterfaceInfo<ModbusGroup>>();
        //        Q_ASSERT(isValidRegs(some_addr, list.size() * 2)); не имеет смысла, если адрес не является начальным

        if (!st.dictionary().contains(some_addr))
            qDebug() << "Описание регистров ModBus не содержит адрес " + QString::number(some_addr);
        //        auto group = st.dictionary().value(some_addr);
        //        bool found = false;
        //        auto dictionary = st.dictionary();
        //        auto it = dictionary.cbegin();
        //        while (it != dictionary.cend() && !found)
        //        {
        //            if (it.value() != group)
        //            {
        //                group = it.value();
        //                found = true;
        //            }
        //            ++it;
        //        }
        //        Q_ASSERT(found);
        QByteArray sigArray;
        for (auto &i : list)
        {
            auto flstr = i.value<DataTypes::FloatStruct>();
            if (flstr.sigAdr < some_addr)
                some_addr = flstr.sigAdr; // search for minimum sig addr
            // now write floats to the out sigArray
            sigArray.push_back(packReg(flstr.sigVal));
        }

        CommandsMBS::CommandStruct inp {
            CommandsMBS::Commands::WriteMultipleRegisters, //
            static_cast<quint16>(some_addr),               //
            static_cast<quint16>(list.size() * 2),         // количество регистров типа int16
            sigArray,                                      //
            TypeId::None,                                  //
            __PRETTY_FUNCTION__                            //
        };
        qDebug() << inp;
        //        Q_ASSERT(isValidRegs(inp)); // закомментарено, т.к. на WRite в xml ещё нет вариантов, и ассерт
        //        вылетает постоянно на проверке кода функции
        DataManager::GetInstance().addToInQueue(inp);
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
        CommandsMBS::Commands::ReadInputRegister, //
        static_cast<quint16>(sigAdr),             //
        static_cast<quint8>(sigCount * 2),        //
        {},                                       //
        type(sigAdr),                             //
        __PRETTY_FUNCTION__                       //
    };
    Q_ASSERT(isValidRegs(inp));
    DataManager::GetInstance().addToInQueue(inp);
}

void ModBus::reqBitStrings(quint32 sigAdr, quint32 sigCount)
{
    CommandsMBS::CommandStruct inp {
        CommandsMBS::Commands::ReadInputRegister, //
        static_cast<quint16>(sigAdr),             //
        static_cast<quint8>(sigCount * 2),        //
        {},                                       //
        TypeId::Uint32,                           //
        __PRETTY_FUNCTION__                       //
    };
    DataManager::GetInstance().addToInQueue(inp);
}
