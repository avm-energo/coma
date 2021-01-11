#include "protocomsettings.h"
// void ProtocomSettings::addGroup(const ProtocomSettings::Group &gr)
//{
//    m_groups.push_back(gr);

//    m_dictionary.insert(gr.startAddr, gr);
//}

// void ProtocomSettings::clearGroups()
//{
//    m_groups.clear();

//    m_dictionary.clear();
//}

// QMap<quint32, ProtocomSettings::Group> ProtocomSettings::dictionary() const
//{
//    return m_dictionary;
//}

// Proto::ProtocomGroup::ProtocomGroup(QDomElement domElement)
//{
//}

Proto::ProtocomGroup::ProtocomGroup(QDomElement domElement) : BaseGroup<Proto::Commands, Proto::TypeId>(domElement)
{
}
