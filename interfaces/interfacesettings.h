#pragma once
#include "iec104private.h"
#include "modbusprivate.h"
#include "protocomprivate.h"

#include <QDebug>
#include <QMap>
#include <QMetaEnum>
#include <QtXml/QDomElement>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

// Thanx to https://stackoverflow.com/questions/34672441
template <template <typename...> class base, typename derived> struct is_base_of_template_impl
{
    template <typename... Ts> static constexpr std::true_type test(const base<Ts...> *);
    static constexpr std::false_type test(...);
    using type = decltype(test(std::declval<derived *>()));
};

template <template <typename...> class base, typename derived>
using is_base_of_template = typename is_base_of_template_impl<base, derived>::type;

template <typename FuncCode, typename TypeId> struct BaseGroup
{
    BaseGroup()=default;
    BaseGroup(QDomElement domElement)
    {
        qDebug() << domElement.attribute("id", "") << domElement.text();
        id = domElement.attribute("id", "");
        //   Q_ASSERT(!id.isEmpty());
        domElement = domElement.firstChildElement();
        while (!domElement.isNull())
        {
            if (domElement.tagName() == "function")
            {
                bool ok;
                qDebug() << domElement.text().toUInt(&ok, 16);
                Q_ASSERT(ok);
                function = static_cast<FuncCode>(domElement.text().toUInt(&ok, 16));
                qDebug() << function;
                domElement = domElement.nextSiblingElement();
                continue;
            }
            if (domElement.tagName() == "type")
            {
                QString buffer = domElement.text();
                Q_ASSERT(!buffer.isEmpty());
                qDebug() << buffer;

                auto types = QMetaEnum::fromType<TypeId>;
                Q_ASSERT(types().isValid());
                buffer[0] = buffer[0].toUpper();
                int typeId = types().keyToValue(buffer.toStdString().c_str());
                Q_ASSERT(typeId != -1);
                dataType = static_cast<TypeId>(typeId);

                domElement = domElement.nextSiblingElement();
                continue;
            }
            if (domElement.tagName() == "start-addr")
            {
                qDebug() << domElement.text().toUInt();
                startAddr = domElement.text().toUInt();
                domElement = domElement.nextSiblingElement();
                continue;
            }
            if (domElement.tagName() == "count")
            {
                qDebug() << domElement.text().toUInt();
                count = domElement.text().toUInt();
                domElement = domElement.nextSiblingElement();
                continue;
            }
            domElement = domElement.nextSiblingElement();
        }
    }
    QString id;
    FuncCode function;
    TypeId dataType;
    quint32 startAddr;
    quint32 count;
};

// Class have to derived from BaseGroup due to dependent types
template <typename Group, typename = typename std::enable_if<is_base_of_template<BaseGroup, Group>::value>::type>
struct InterfaceInfo
{
    //    InterfaceInfo()
    //    {
    //    }

    void addGroup(const Group &gr)
    {
        m_groups.append(gr);

        m_dictionary.insert(gr.startAddr, gr);
    }
    void clearGroups()
    {
        m_groups.clear();

        m_dictionary.clear();
    }

    QList<Group> groups() const
    {
        return m_groups;
    }

    QMultiMap<quint32, Group> dictionary() const
    {
        return m_dictionary;
    }

private:
    // Realized two versions, only one will be stayed
    QList<Group> m_groups;
    QMultiMap<quint32, Group> m_dictionary;
};

namespace CommandsMBS
{
struct ModbusGroup : BaseGroup<Commands, TypeId>
{
    ModbusGroup()=default;
    ModbusGroup(QDomElement domElement) : BaseGroup<Commands, TypeId>(domElement)
    {
    }
    // NOTE Need more fileds?
};
}

namespace Proto
{
struct ProtocomGroup : BaseGroup<Commands, TypeId>
{
    ProtocomGroup()=default;
    ProtocomGroup(QDomElement domElement) : BaseGroup<Commands, TypeId>(domElement)
    {
        domElement = domElement.firstChildElement();
        while (!domElement.isNull())
        {
            if (domElement.tagName() == "block")
            {
                qDebug() << domElement.text().toUInt();
                block = domElement.text().toUInt();
            }
            domElement = domElement.nextSiblingElement();
        }
    }
    quint16 block;
    // NOTE Need more fileds?
};
}

namespace Commands104
{
struct Iec104Group : BaseGroup<Commands, TypeId>
{
    Iec104Group()=default;
    Iec104Group(QDomElement domElement) : BaseGroup<Commands, TypeId>(domElement)
    {
    }
    // NOTE Need more fileds?
};
}

// For template not in header
//#include "interfacesettings.cpp"
struct InterfaceSettings
{
    std::variant<InterfaceInfo<CommandsMBS::ModbusGroup>, //
        InterfaceInfo<Proto::ProtocomGroup>,              //
        InterfaceInfo<Commands104::Iec104Group>           //
        >
        settings;
};
