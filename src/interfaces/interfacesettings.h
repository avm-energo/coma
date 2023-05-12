#pragma once

#include "iec104private.h"
#include "modbusprivate.h"
#include "protocomprivate.h"

#include <QMetaEnum>

///< /brief Базовая группа сигналов для абстрактного протокола: код функции, тип данных, начальный адрес сигнала и
///< количество сигналов

struct parseXChangeStruct
{
    QVariant interfaceType;
    quint32 sigId;
    QVariant par2;
    QVariant par3;
    QVariant par4;
};

struct BaseGroup
{
    QVariant function;
    QVariant dataType;
    quint32 startAddr;
    quint32 count;

    BaseGroup() = default;
    BaseGroup(const quint32 &sigAddr, const quint32 &sigCount) : startAddr(sigAddr), count(sigCount)
    {
    }
};

// Набор групп сигналов для выбранного протокола

class ProtocolDescription
{
public:
    void addGroup(const BaseGroup &gr)
    {
        m_dictionary.insert(gr.startAddr, gr);
    }

    void clear()
    {
        m_dictionary.clear();
    }

    auto &dictionary() const
    {
        return m_dictionary;
    }

    void merge(const ProtocolDescription &rhs)
    {
        for (auto it = rhs.m_dictionary.cbegin(); it != rhs.m_dictionary.cend(); it++)
            m_dictionary.insert(it.key(), it.value());
    }

private:
    QMultiMap<quint32, BaseGroup> m_dictionary;
};

struct ProtocomGroup : BaseGroup
{
    //    quint8 block;

    ProtocomGroup() = default;
    ProtocomGroup(const quint32 &sigAddr, const quint32 &sigCount, const quint16 &blk)
        : BaseGroup(sigAddr, sigCount) //, block(blk)
    {
        dataType = blk;
    }
};

struct ModbusGroup : BaseGroup
{
    ModbusGroup() = default;
    ModbusGroup(const quint32 &sigAddr, const quint32 &sigCount, const quint16 &regType, const QString &type)
        : BaseGroup(sigAddr, sigCount)
    {
        count = count * 2;
        // Decimal to hex
        auto hexRegType = QString("%1").arg(regType).toUInt(nullptr, 16);
        function = hexRegType;
        // Getting TypeId from QString
        auto buffer(type);
        auto types = QMetaEnum::fromType<MBS::TypeId>;
        if (!buffer.isEmpty() && types().isValid())
        {
            buffer[0] = buffer[0].toUpper();
            bool state = false;
            auto typeId = types().keyToValue(buffer.toStdString().c_str(), &state);
            if (typeId != -1 && state)
                dataType = typeId;
            else
                qWarning("Undefined modbus type");
        }
    }

    bool operator==(const ModbusGroup &rhs) const
    {
        return ((function == rhs.function) && (dataType == rhs.dataType) && (startAddr == rhs.startAddr));
    }

    bool operator!=(const ModbusGroup &rhs) const
    {
        return !(*this == rhs);
    }
};

struct Iec104Group : BaseGroup
{
    Iec104Group() = default;
    Iec104Group(const quint32 &sigAddr, const quint32 &sigCount, const quint16 &sigType, const quint16 &transType,
        const quint16 &sigGroup)
        : BaseGroup(sigAddr, sigCount)
    {
        if (sigType > 0 && sigType < 129)
            dataType = sigType;
        // TODO: Any fields?
        Q_UNUSED(transType);
        Q_UNUSED(sigGroup);
    }
};

Q_DECLARE_METATYPE(ProtocomGroup)
Q_DECLARE_METATYPE(ModbusGroup)
Q_DECLARE_METATYPE(Iec104Group)
