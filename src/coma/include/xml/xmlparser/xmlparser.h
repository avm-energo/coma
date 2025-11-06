#pragma once

#include <device/xml_settings.h>
#include <s2/delegate_common.h>

#include <QDomElement>
#include <QDomNode>
#include <QObject>

namespace Xml
{

// /// \brief Template check in compile time whether T is derived from the delegate::QComboBox class.
template <typename T> constexpr bool is_comboBox = std::is_base_of_v<delegate::ComboBox, T>;

class XmlParser : public QObject
{
    Q_OBJECT
public:
    const static QHash<QString, ctti::detail::hash_t> s_nameTypeMap;

    explicit XmlParser(QObject *parent = nullptr);

    /// \brief Возвращаем хэш типа для его идентификации.
    ctti::detail::hash_t parseType(const QDomElement &typeNode);
    /// \brief Парсинг всех дочерних узлов <tab> узла <config-tabs>.
    void parseConfigTab(const QDomNode &tabNode);
    /// \brief Парсинг узла <config-tabs>.
    void parseRecord(const QDomNode &recordNode);
    /// \brief Парсинг тегов для структуры delegate::DoubleSpinBoxWidget и её потомков.
    void dSpinBoxParse(delegate::DoubleSpinBoxWidget &dsbw, const QDomElement &widgetNode);
    /// \brief Парсинг тегов для потомков структуры delegate::Group.
    void groupParse(delegate::Group &group, const QDomElement &widgetNode, const QStringList &items);
    /// \brief Парсинг тегов для структуры delegate::QComboBox и её потомков.
    void comboBoxParse(delegate::ComboBox &comboBox, const QDomElement &widgetNode, const QStringList &items);
    /// \brief Парсинг тегов для структуры config::Item.
    config::Item parseItem(const QDomElement &domElement, //
        const QString &className, const ctti::unnamed_type_id_t &type);
    /// \brief Парсинг ноды <widget> у <record>.
    config::itemVariant parseWidget(const QDomElement &widgetNode);

signals:
    void nameDataSending(const u32 id, const QString &itemName);
    void typeDataSending(const u32 id, const ctti::detail::hash_t type);
    void dtypeDataSending(const u32 id, bool dtype);
    void widgetDataSending(const u32 id, const config::itemVariant &widget);
    void configTabDataSending(const u32 id, const QString &tabName);
};

}
