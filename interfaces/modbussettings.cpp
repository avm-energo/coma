#include "modbussettings.h"

#include <QDebug>
#include <QMetaEnum>
// ModbusSettings::ModbusSettings()
//{
//}

// void ModbusSettings::addGroup(const ModbusSettings::Group &gr)
//{
//    m_groups.append(gr);

//    m_dictionary.insert(gr.startAddr, gr);
//}

// void ModbusSettings::clearGroups()
//{
//    m_groups.clear();

//    m_dictionary.clear();
//}

// QList<ModbusSettings::Group> ModbusSettings::groups() const
//{
//    return m_groups;
//}

// QMap<quint32, ModbusSettings::Group> ModbusSettings::dictionary() const
//{
//    return m_dictionary;
//}

CommandsMBS::ModbusGroup::ModbusGroup(QDomElement domElement)
    : BaseGroup<CommandsMBS::Commands, CommandsMBS::TypeId>(domElement)
{
}
