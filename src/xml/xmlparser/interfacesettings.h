#pragma once

#include <QMetaEnum>
#include <QtXml/QDomElement>
#include <type_traits>
#include <utility>

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
    BaseGroup() = default;
    BaseGroup(QDomElement domElement)
    {
        domElement = domElement.firstChildElement();
        while (!domElement.isNull())
        {
            if (domElement.tagName() == "function")
            {
                bool ok;
                function = static_cast<FuncCode>(domElement.text().toUInt(&ok, 16));
                Q_ASSERT(ok);
                domElement = domElement.nextSiblingElement();
                continue;
            }
            if (domElement.tagName() == "type")
            {
                QString buffer = domElement.text();
                Q_ASSERT(!buffer.isEmpty());
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
                startAddr = domElement.text().toUInt();
                domElement = domElement.nextSiblingElement();
                continue;
            }
            if (domElement.tagName() == "count")
            {
                count = domElement.text().toUInt();
                domElement = domElement.nextSiblingElement();
                continue;
            }
            domElement = domElement.nextSiblingElement();
        }
    }

    bool operator==(const BaseGroup &rhs) const
    {
        return ((id == rhs.id) && (function == rhs.function) && //
            (dataType == rhs.dataType) && (startAddr == rhs.startAddr));
    }

    bool operator!=(const BaseGroup &rhs) const
    {
        return !(*this == rhs);
    }

    QString id;
    FuncCode function;
    TypeId dataType;
    quint32 startAddr;
    quint32 count;
};

// Class have to derived from BaseGroup due to dependent types
template <typename Group, typename = typename std::enable_if<is_base_of_template<BaseGroup, Group>::value>::type>
class InterfaceInfo
{
public:
    void addGroup(const Group &gr)
    {
        m_dictionary.insert(gr.startAddr, gr);
    }

    void clear()
    {
        m_dictionary.clear();
    }

    const auto &dictionary() const
    {
        return m_dictionary;
    }

    void merge(const InterfaceInfo<Group> &rhs)
    {
        for (auto it = rhs.m_dictionary.cbegin(); it != rhs.m_dictionary.cend(); it++)
            m_dictionary.insert(it.key(), it.value());
    }

private:
    QMultiMap<quint32, Group> m_dictionary;
};
