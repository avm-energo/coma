#pragma once

#include <s2/delegate_common.h>
#include <xml/xmlparser/xmlbaseparser.h>

namespace Xml
{
/// \brief Template check in compile time whether T is derived from the delegate::QComboBox class.
template <typename T> constexpr bool is_comboBox = std::is_base_of_v<delegate::QComboBox, T>;

/// \brief Class for parsing "s2files.xml" file.
class S2Parser : public BaseParser
{
    Q_OBJECT
private:
    static const QHash<QString, std::uint64_t> nameTypeMap;
    QDomElement content;

public:
    explicit S2Parser(QObject *parent = nullptr);
    /// \brief Парсинг всех нод <record> файла s2files.xml.
    void parse();

signals:
    void nameDataSending(const quint32 id, const QString &itemName);
    void typeDataSending(const quint32 id, const std::uint64_t type);
    void widgetDataSending(const quint32 id, const config::itemVariant &widget);
    void configTabDataSending(const quint32 id, const QString &tabName);

private:
    /// \brief Возвращаем хэш типа для его идентификации.
    std::uint64_t parseType(const QDomElement &typeNode);
    /// \brief Парсинг всех дочерних узлов <tab> узла <config-tabs>.
    void parseConfigTab(const QDomNode &tabNode);
    /// \brief Парсинг узла <config-tabs>.
    void parseConfigTabs(const QDomElement &s2node);
    /// \brief Парсинг тегов для структуры delegate::DoubleSpinBoxWidget и её потомков.
    void dSpinBoxParse(delegate::DoubleSpinBoxWidget &dsbw, const QDomElement &widgetNode);
    /// \brief Парсинг тегов для потомков структуры delegate::Group.
    void groupParse(delegate::Group &group, const QDomElement &widgetNode, const QStringList &items);
    /// \brief Парсинг тегов для структуры delegate::QComboBox и её потомков.
    void comboBoxParse(delegate::QComboBox &comboBox, const QDomElement &widgetNode, const QStringList &items);
    /// \brief Парсинг тегов для структуры config::Item.
    config::Item parseItem(const QDomElement &domElement, //
        const QString &className, const ctti::unnamed_type_id_t &type);
    /// \brief Парсинг ноды <widget> у <record>.
    config::itemVariant parseWidget(const QDomElement &widgetNode);
};

}
