#pragma once

#include "../../widgets/delegate_common.h"
#include "xmlbaseparser.h"

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

public:
    explicit S2Parser(QObject *parent = nullptr);
    void parse(const QDomNode &content);

signals:
    void typeDataSending(const quint16 &id, const std::uint64_t &type);
    void widgetDataSending(const quint16 &id, const config::itemVariant &widget);
    void configTabDataSending(const quint32 &id, const QString &tabName);

private:
    std::uint64_t parseType(const QDomElement &typeNode);

    void parseConfigTab(const QDomNode &tabNode);
    void parseConfigTabs(const QDomElement &s2node);
    void dSpinBoxParse(delegate::DoubleSpinBoxWidget &dsbw, const QDomElement &widgetNode);
    void groupParse(delegate::Group &group, const QDomElement &widgetNode, const QStringList &items);
    void comboBoxParse(delegate::QComboBox &comboBox, const QDomElement &widgetNode, const QStringList &items);
    config::Item parseItem(const QDomElement &domElement, //
        const QString &className, const ctti::unnamed_type_id_t &type);
    config::itemVariant parseWidget(const QDomElement &widgetNode);
};

}
