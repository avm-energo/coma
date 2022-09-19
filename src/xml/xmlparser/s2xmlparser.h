#pragma once

#include "../../widgets/delegate_common.h"
#include "basexmlparser.h"

template <typename T> constexpr auto is_comboBox = std::is_base_of_v<delegate::QComboBox, T>;

class S2XmlParser : public BaseXmlParser
{
    Q_OBJECT
private:
    static const QHash<QString, std::uint64_t> nameTypeMap;

public:
    explicit S2XmlParser(QObject *parent = nullptr);
    void parse(const QDomNode &content);

signals:
    void typeDataSending(const quint16 &id, const std::uint64_t &type);
    void widgetDataSending(const quint16 &id, const config::itemVariant &widget);

private:
    std::uint64_t parseType(const QDomElement &typeNode);

    void dSpinBoxParse(delegate::DoubleSpinBoxWidget &dsbw, const QDomElement &widgetNode);
    void groupParse(delegate::Group &group, const QDomElement &widgetNode, const QStringList &items);
    void comboBoxParse(delegate::QComboBox &comboBox, const QDomElement &widgetNode, const QStringList &items);
    config::Item parseItem(const QDomElement &domElement, //
        const QString &className, const ctti::unnamed_type_id_t &type);
    config::itemVariant parseWidget(const QDomElement &widgetNode);
};
