#include "modbussettings.h"

#include <QDebug>
#include <QMetaEnum>
ModbusSettings::ModbusSettings()
{
}

void ModbusSettings::addGroup(const ModbusSettings::Group &gr)
{
    m_groups.append(gr);
}

void ModbusSettings::clearGroups()
{
    m_groups.clear();
}

QList<ModbusSettings::Group> ModbusSettings::groups() const
{
    return m_groups;
}

CommandsMBS::ModbusGroup::ModbusGroup(QDomElement domElement)
{
    qDebug() << domElement.attribute("id", "") << domElement.text();
    id = domElement.attribute("id", "");
    Q_ASSERT(!id.isEmpty());
    domElement = domElement.firstChildElement();
    while (!domElement.isNull())
    {
        if (domElement.tagName() == "function")
        {
            qDebug() << domElement.text().toUInt();
            domElement = domElement.nextSiblingElement();
            continue;
        }
        if (domElement.tagName() == "type")
        {
            QString buffer = domElement.text();
            Q_ASSERT(!buffer.isEmpty());
            qDebug() << buffer;

            auto types = QMetaEnum::fromType<CommandsMBS::TypeId>;
            Q_ASSERT(types().isValid());
            // buffer.toUpper();
            buffer[0] = buffer[0].toUpper();
            // buffer.data()->toUpper();
            int typeId = types().keyToValue(buffer.toStdString().c_str());
            Q_ASSERT(typeId != -1);
            dataType = static_cast<CommandsMBS::TypeId>(typeId);

            domElement = domElement.nextSiblingElement();
            continue;
        }
        if (domElement.tagName() == "start-addr")
        {
            qDebug() << domElement.text().toUInt();

            domElement = domElement.nextSiblingElement();
            continue;
        }
        if (domElement.tagName() == "count")
        {
            qDebug() << domElement.text().toUInt();
            domElement = domElement.nextSiblingElement();
            continue;
        }
        domElement = domElement.nextSiblingElement();
    }
}
