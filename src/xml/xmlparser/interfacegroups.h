#ifndef INTERFACEGROUPS_H
#define INTERFACEGROUPS_H

#include "../../interfaces/iec104private.h"
#include "../../interfaces/modbusprivate.h"
#include "../../interfaces/protocomprivate.h"
#include "interfacesettings.h"

struct ProtocomGroup : BaseGroup<Proto::Commands, Proto::TypeId>
{
    quint8 block;

    ProtocomGroup() = default;
    ProtocomGroup(const quint32 &sigId, const quint16 &blk)
        : BaseGroup<Proto::Commands, Proto::TypeId>(sigId), block(blk)
    {
    }
};

struct ModbusGroup : BaseGroup<CommandsMBS::Commands, CommandsMBS::TypeId>
{
    ModbusGroup() = default;
    ModbusGroup(const quint32 &sigId, const quint16 &regType, const QString &type)
        : BaseGroup<CommandsMBS::Commands, CommandsMBS::TypeId>(sigId)
    {
        // Decimal to hex
        auto hexRegType = QString("%1").arg(regType).toUInt(nullptr, 16);
        function = static_cast<CommandsMBS::Commands>(hexRegType);
    }

    bool operator==(const ModbusGroup &rhs) const
    {
        return (BaseGroup<CommandsMBS::Commands, CommandsMBS::TypeId>::operator==(rhs));
    }

    bool operator!=(const ModbusGroup &rhs) const
    {
        return !(*this == rhs);
    }
};

struct Iec104Group : BaseGroup<Commands104::Commands, Commands104::TypeId>
{
    Iec104Group() = default;
    Iec104Group(const quint32 &sigId, const quint16 &sigType, const quint16 &transType, const quint16 &sigGroup)
        : BaseGroup<Commands104::Commands, Commands104::TypeId>(sigId)
    {
    }
};

Q_DECLARE_METATYPE(InterfaceInfo<ProtocomGroup>)
Q_DECLARE_METATYPE(InterfaceInfo<ModbusGroup>)
Q_DECLARE_METATYPE(InterfaceInfo<Iec104Group>)

template <class Group> const InterfaceSettings parseSettings(const QDomElement &domElement)
{
    const auto &nodes = domElement.childNodes();
    int i = 0;
    InterfaceInfo<Group> settings;
    while (i != nodes.count())
    {
        const auto &domElement = nodes.item(i++).toElement();
        if (domElement.tagName().contains("group", Qt::CaseInsensitive))
        {
            Group group(domElement);
            settings.addGroup(group);
        }
    }
    return InterfaceSettings { QVariant::fromValue(settings) };
}

#endif // INTERFACEGROUPS_H
