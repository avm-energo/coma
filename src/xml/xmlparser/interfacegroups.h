#ifndef INTERFACEGROUPS_H
#define INTERFACEGROUPS_H

#include "../../interfaces/iec104private.h"
#include "../../interfaces/modbusprivate.h"
#include "../../interfaces/protocomprivate.h"
#include "../../s2/s2settings.h"
#include "interfacesettings.h"

struct Iec104Group : BaseGroup<Commands104::Commands, Commands104::TypeId>
{
    Iec104Group() = default;
    Iec104Group(QDomElement domElement) : BaseGroup<Commands104::Commands, Commands104::TypeId>(domElement)
    {
    }
};
Q_DECLARE_METATYPE(InterfaceInfo<Iec104Group>)

struct ModbusGroup : BaseGroup<CommandsMBS::Commands, CommandsMBS::TypeId>
{
    ModbusGroup() = default;
    ModbusGroup(QDomElement domElement) : BaseGroup<CommandsMBS::Commands, CommandsMBS::TypeId>(domElement)
    {
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
Q_DECLARE_METATYPE(InterfaceInfo<ModbusGroup>)

// QDebug operator<<(QDebug debug, const ModbusGroup &settings)
//{
//    debug.nospace() << settings.id << ":" << settings.function << ":"
//                    << QVariant::fromValue(settings.dataType).toString() << ":" << settings.startAddr << ":"
//                    << settings.count;
//    return debug.maybeSpace();
//}

struct ProtocomGroup : BaseGroup<Proto::Commands, Proto::TypeId>
{
    ProtocomGroup() = default;
    ProtocomGroup(QDomElement domElement) : BaseGroup<Proto::Commands, Proto::TypeId>(domElement)
    {
        domElement = domElement.firstChildElement();
        while (!domElement.isNull())
        {
            if (domElement.tagName() == "block")
                block = domElement.text().toUInt();
            domElement = domElement.nextSiblingElement();
        }
    }
    quint8 block;
};
Q_DECLARE_METATYPE(InterfaceInfo<ProtocomGroup>)

template <class Group> const InterfaceSettings parseSettings(QDomElement domElement)
{
    using Register = typename InterfaceInfo<Group>::Register;
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
        else if (domElement.tagName().contains("register", Qt::CaseInsensitive))
        {
            Register reg(domElement);
            settings.addReg(reg);
        }
    }
    return InterfaceSettings { QVariant::fromValue(settings) };
}

#endif // INTERFACEGROUPS_H
