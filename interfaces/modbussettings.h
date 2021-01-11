#pragma once
#include "modbusprivate.h"

#include <QtXml/QDomElement>
namespace CommandsMBS
{
Q_NAMESPACE
enum TypeId : int
/// 1 register is int16
{
    /// 1 register contains 1 bit
    Bool,
    /// 2 registers contain uint32 (bitstring)
    Uint32,
    /// 2 registers contain float
    Float,
    // Smth else
};
Q_ENUM_NS(TypeId)

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
