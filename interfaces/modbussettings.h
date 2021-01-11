#pragma once
#include "modbusprivate.h"

#include <QtXml/QDomElement>
namespace CommandsMBS
{
// Q_NAMESPACE

struct ModbusGroup
{
    using FuncCode = CommandsMBS::Commands;
    ModbusGroup(QDomElement domElement);
    QString id;
    FuncCode function;
    TypeId dataType;
    quint32 startAddr;
    quint32 count;
};
}
class ModbusSettings
{
public:
    using Group = CommandsMBS::ModbusGroup;
    ModbusSettings();

    void addGroup(const Group &gr);
    void clearGroups();

    QList<Group> groups() const;

private:
    QList<Group> m_groups;
};
