#pragma once

#include <interfaces/types/common_types.h>
#include <interfaces/types/iec104_types.h>
#include <interfaces/types/modbus_types.h>
#include <interfaces/types/protocom_types.h>

///< /brief Базовая группа сигналов для абстрактного протокола: код функции, тип данных, начальный адрес сигнала и
///< количество сигналов

struct AbstractGroup
{
    Interface::IfaceType ifaceType;
    quint32 sigId;
    quint16 arg1;
    quint16 arg2;
};

struct BaseGroup
{
    QVariant function;
    QVariant block;
    quint32 startAddr;
    quint32 count;

    BaseGroup() = default;
    BaseGroup(const quint32 sigAddr, const quint32 sigCount = 0) : startAddr(sigAddr), count(sigCount)
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
    ProtocomGroup(const quint32 sigAddr, const quint32 sigCount, const quint16 blk)
        : BaseGroup(sigAddr, sigCount) //, block(blk)
    {
        block = blk;
    }
};

struct ModbusGroup : BaseGroup
{
    ModbusGroup() = default;
    ModbusGroup(const quint32 sigAddr, const quint32 sigCount, const quint16 regType) : BaseGroup(sigAddr, sigCount)
    {
        // Decimal to hex
        auto hexRegType = QString("%1").arg(regType).toUInt(nullptr, 16);
        function = hexRegType;
    }

    bool operator==(const ModbusGroup &rhs) const
    {
        return ((function == rhs.function) && (startAddr == rhs.startAddr));
    }

    bool operator!=(const ModbusGroup &rhs) const
    {
        return !(*this == rhs);
    }
};

struct Iec104Group : BaseGroup
{
    Iec104Group() = default;
    Iec104Group(const quint32 sigAddr, const quint32 sigCount, const quint16 transType, const quint16 sigGroup)
        : BaseGroup(sigAddr, sigCount)
    {
        // TODO: Any fields?
        Q_UNUSED(transType);
        Q_UNUSED(sigGroup);
    }
};
