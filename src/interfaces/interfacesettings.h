#pragma once

#include <QDebug>
#include <QMap>
#include <QMetaEnum>
#include <QtXml/QDomElement>
#include <any>
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

//#define XML_DEBUG
template <typename FuncCode, typename TypeId> struct BaseRegister
{
    BaseRegister() = default;
    BaseRegister(QDomElement domElement)
    {
#ifdef XML_DEBUG
        qDebug() << domElement.attribute("id", "") << domElement.text();
#endif
        id = domElement.attribute("id", "");
        //   Q_ASSERT(!id.isEmpty());
        domElement = domElement.firstChildElement();
        while (!domElement.isNull())
        {
            if (domElement.tagName() == "function")
            {
                bool ok;
#ifdef XML_DEBUG
                qDebug() << domElement.text().toUInt(&ok, 16);
#endif
                function = static_cast<FuncCode>(domElement.text().toUInt(&ok, 16));
                Q_ASSERT(ok);
#ifdef XML_DEBUG
                qDebug() << function;
#endif
                domElement = domElement.nextSiblingElement();
                continue;
            }
            if (domElement.tagName() == "type")
            {
                QString buffer = domElement.text();
                Q_ASSERT(!buffer.isEmpty());
#ifdef XML_DEBUG
                qDebug() << buffer;
#endif
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
#ifdef XML_DEBUG
                qDebug() << domElement.text().toUInt();
#endif
                startAddr = domElement.text().toUInt();
                domElement = domElement.nextSiblingElement();
                continue;
            }
            domElement = domElement.nextSiblingElement();
        }
    }
    bool operator==(const BaseRegister &rhs) const
    {
        return (
            (id == rhs.id) && (function == rhs.function) && (dataType == rhs.dataType) && (startAddr == rhs.startAddr));
    }
    bool operator!=(const BaseRegister &rhs) const
    {
        return !(*this == rhs);
    }
    QString id;
    FuncCode function;
    TypeId dataType;
    quint32 startAddr;
};

template <typename FuncCode, typename TypeId> struct BaseGroup : BaseRegister<FuncCode, TypeId>
{
    typedef BaseRegister<FuncCode, TypeId> base;
    BaseGroup() = default;
    BaseGroup(QDomElement domElement) : BaseRegister<FuncCode, TypeId>(domElement)
    {
#ifdef XML_DEBUG
        qDebug() << domElement.attribute("id", "") << domElement.text();
#endif
        domElement = domElement.firstChildElement();
        while (!domElement.isNull())
        {
            if (domElement.tagName() == "count")
            {
#ifdef XML_DEBUG
                qDebug() << domElement.text().toUInt();
#endif
                count = domElement.text().toUInt();
                domElement = domElement.nextSiblingElement();
                continue;
            }
            domElement = domElement.nextSiblingElement();
        }
    }
    bool operator==(const BaseGroup &rhs) const
    {
        return (BaseRegister<FuncCode, TypeId>::operator==(rhs) && (count == rhs.count));
    }
    bool operator!=(const BaseGroup &rhs) const
    {
        return !(*this == rhs);
    }
    quint32 count;
};

// Class have to derived from BaseGroup due to dependent types
template <typename Group, typename = typename std::enable_if<is_base_of_template<BaseGroup, Group>::value>::type>
class InterfaceInfo
{
public:
    using Register = typename Group::base;
    void addGroup(const Group &gr)
    {
        m_groups.append(gr);

        m_dictionary.insert(gr.startAddr, gr);
    }
    void addReg(const Register &reg)
    {
        m_regs.append(reg);

        m_dictionaryRegs.insert(reg.startAddr, reg);
    }
    void clear()
    {
        m_groups.clear();
        m_regs.clear();
        m_dictionary.clear();
        m_dictionaryRegs.clear();
    }

    const auto &groups() const
    {
        return m_groups;
    }

    const auto &dictionary() const
    {
        return m_dictionary;
    }

    const auto &regs() const
    {
        return m_regs;
    }

    const auto &dictionaryRegs() const
    {
        return m_dictionaryRegs;
    }

    void merge(const InterfaceInfo<Group> &rhs)
    {
        m_groups.append(rhs.m_groups);
        for (auto it = rhs.m_dictionary.cbegin(); it != rhs.m_dictionary.cend(); it++)
        {
            m_dictionary.insert(it.key(), it.value());
        }

        m_regs.append(rhs.m_regs);
        for (auto it = rhs.m_dictionaryRegs.cbegin(); it != rhs.m_dictionaryRegs.cend(); it++)
        {
            m_dictionaryRegs.insert(it.key(), it.value());
        }
    }

private:
    // Realized two versions, only one will be stayed
    QList<Group> m_groups;
    QMultiMap<quint32, Group> m_dictionary;
    //
    QList<Register> m_regs;
    QMultiMap<quint32, Register> m_dictionaryRegs;
};
