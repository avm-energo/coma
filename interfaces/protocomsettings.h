#pragma once
#include "interfacesettings.h"
#include "protocomprivate.h"

#include <QMap>
#include <QtXml/QDomElement>
namespace Proto
{
// struct ProtocomGroup
//{
//    using FuncCode = Proto::Commands;
//    ProtocomGroup(QDomElement domElement);
//    QString id;
//    FuncCode function;
//    TypeId dataType;
//    quint32 startAddr;
//    quint32 count;
//    quint16 block;
//};
struct ProtocomGroup : BaseGroup<Proto::Commands, Proto::TypeId>
{
    ProtocomGroup(QDomElement domElement);
    quint16 block;
};
}
// class ProtocomSettings
//{
// public:
//    using Group = Proto::ProtocomGroup;
//    ProtocomSettings();

//    void addGroup(const Group &gr);
//    void clearGroups();

//    QList<Group> groups() const;

//    QMap<quint32, Group> dictionary() const;

// private:
//    QList<Group> m_groups;
//    QMap<quint32, Group> m_dictionary;
//};
