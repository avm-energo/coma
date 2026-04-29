#include "xml/xmleditor/models/xmldatamodel.h"

#include <avm-gen/settings.h>
#include <avm-gen/strings.h>
#include <avm-gen/xml/xmlparse.h>
#include <xml/xmltags.h>

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QMap>

XmlDataModel::XmlDataModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent) { }

std::tuple<QString, QString, std::function<void(QDomDocument &, QDomElement &, int &)>> XmlDataModel::getModelSettings()
{
    auto alarmHelper = [this](auto &doc, auto &item, auto &row)
    {
        makeElement(doc, item, tags::addr, data(index(row, 0)));
        makeElement(doc, item, tags::string, data(index(row, 1)));
        if (rowCount() > 2)
        {
            QVariant orderData(data(index(row, 2)));
            if ((!orderData.value<QString>().isEmpty()) && (orderData.value<int>() != 0))
                makeElement(doc, item, tags::order, orderData);
            auto highlights = data(index(row, 3)).toString().split(',');
            if (!(highlights.isEmpty() || (highlights.size() == 1 && highlights.first().isEmpty())))
            {
                auto highlightsNode = makeElement(doc, tags::highlights);
                for (const auto &value : std::as_const(highlights))
                    makeElement(doc, highlightsNode, tags::item, value);
                item.appendChild(highlightsNode);
            }
        }
    };

    switch (m_type)
    {
    case ModelType::Signals:
        return { tags::sigs, tags::sig, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::start_addr, data(index(row, 0)));
                makeElement(doc, item, tags::count, data(index(row, 1)));
                makeElement(doc, item, tags::id, data(index(row, 2)));
                makeElement(doc, item, tags::type, data(index(row, 3)));
            } };
    case ModelType::SectionTabs:
        return { tags::tabs, tags::tab, //
            [this](auto &doc, auto &item, auto &row)
            {
                QString dtypeText;
                makeElement(doc, item, tags::id, data(index(row, 0)));
                makeElement(doc, item, tags::name, data(index(row, 1)));
                dtypeText = data(index(row, 2)).toString();
                if (dtypeText != "0")
                    makeElement(doc, item, tags::dtype, "Yes");
            } };
    case ModelType::AlarmStateAll:
        return { tags::state_all, tags::item, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::addr, data(index(row, 0)));
                makeElement(doc, item, tags::string, data(index(row, 1)));
                makeElement(doc, item, tags::type, data(index(row, 2)));
            } };
        break;
    case ModelType::AlarmsCrit:
        return { tags::crit, tags::item, alarmHelper };
    case ModelType::AlarmsWarn:
        return { tags::warn, tags::item, alarmHelper };
    case ModelType::AlarmsInfo:
        return { tags::info, tags::item, alarmHelper };
    case ModelType::WorkJours:
        return { tags::work, tags::item, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::addr, data(index(row, 0)));
                makeElement(doc, item, tags::desc, data(index(row, 1)));
            } };
    case ModelType::MeasJours:
        return { tags::meas, tags::item, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::index, data(index(row, 0)));
                makeElement(doc, item, tags::header, data(index(row, 1)));
                makeElement(doc, item, tags::type, data(index(row, 2)));
                makeElement(doc, item, tags::visibility, data(index(row, 3)));
            } };
    case ModelType::Modbus:
        return { tags::modbus, tags::group, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::sig_id, data(index(row, 0)));
                makeElement(doc, item, tags::reg_type, data(index(row, 1)));
                makeElement(doc, item, tags::type, data(index(row, 2)));
                makeElement(doc, item, tags::desc, data(index(row, 3)));
            } };
    case ModelType::Protocom:
        return { tags::protocom, tags::group, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::block, data(index(row, 0)));
                makeElement(doc, item, tags::sig_id, data(index(row, 1)));
            } };
    case ModelType::IEC60870:
        return { tags::iec, tags::group, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::sig_id, data(index(row, 0)));
                makeElement(doc, item, tags::sig_type, data(index(row, 1)));
                makeElement(doc, item, tags::trans_type, data(index(row, 2)));
                makeElement(doc, item, tags::sig_group, data(index(row, 3)));
            } };
    case ModelType::Config:
        return { tags::config, tags::record, //
            [this](auto &doc, auto &item, auto &row)
            {
                const auto &names = s2RecordsNameMap();
                auto nameOrId = data(index(row, 0)).toString();
                auto id = names.key(nameOrId, -1);
                if (id >= 0)
                    makeElement(doc, item, tags::id, id);
                else
                    makeElement(doc, item, tags::id, nameOrId); // fallback: numeric id stored as string
                makeElement(doc, item, tags::def_val, data(index(row, 1)));
                QVariant countData(data(index(row, 2)));
                if ((!countData.value<QString>().isEmpty()) && (countData.value<int>() > 1))
                    makeElement(doc, item, tags::count, countData);
                QVariant orderData(data(index(row, 3)));
                if ((!orderData.value<QString>().isEmpty()) && (orderData.value<int>() != 0))
                    makeElement(doc, item, tags::order, orderData);
                makeElement(doc, item, tags::visibility, data(index(row, 4)));
            } };
    case ModelType::HiddenTab:
        return { tags::hidden_tab, tags::mwidget, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::index, data(index(row, 0)));
                setAttribute(doc, item, tags::title, data(index(row, 1)));
                makeElement(doc, item, tags::name, data(index(row, 2)));
                setAttribute(doc, item, tags::view, data(index(row, 3)));
                makeElement(doc, item, tags::type, data(index(row, 4)));
                makeElement(doc, item, tags::addr, data(index(row, 5)));
                makeElement(doc, item, tags::visibility, data(index(row, 6)));
            } };
    case ModelType::BsiExt:
        return { tags::bsi_ext, tags::item, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::addr, data(index(row, 0)));
                makeElement(doc, item, tags::desc, data(index(row, 1)));
                makeElement(doc, item, tags::type, data(index(row, 2)));
                makeElement(doc, item, tags::visibility, data(index(row, 3)));
            } };
    case ModelType::S2Tabs:
        return { tags::conf_tabs, tags::tab, //
            [this](auto &doc, auto &item, auto &row)
            {
                makeElement(doc, item, tags::id, data(index(row, 0)));
                makeElement(doc, item, tags::name, data(index(row, 1)));
            } };
    case ModelType::Includes:
        return { tags::includes, tags::include, //
            [this](auto &doc, auto &item, auto &row) { setAttribute(doc, item, tags::src, data(index(row, 0))); } };
    default:
        qWarning() << "Model settings not found!";
        return { "undefined", "undefined", //
            [](auto &doc, auto &item, auto &row)
            {
                Q_UNUSED(doc);
                Q_UNUSED(item);
                Q_UNUSED(row);
            } };
    }
}

void XmlDataModel::parseNode(QDomNode &node, int &row)
{
    QString dtypeText;
    switch (m_type)
    {
    case ModelType::Signals:
        parseTag(node, tags::start_addr, row, 0, "", true);       // Стартовый адрес
        parseTag(node, tags::count, row, 1);                      // Количество
        parseTag(node, tags::id, row, 2);                         // ID сигнала
        parseTag(node, tags::type, row, 3, "Float");              // Тип сигнала
        break;                                                    //
    case ModelType::SectionTabs:                                  //
        parseTag(node, tags::id, row, 0, "", true);               // ID вкладки
        parseTag(node, tags::name, row, 1);                       // Имя вкладки
        dtypeText = XmlParse::parseString(node, tags::dtype);
        setData(index(row, 2), (dtypeText == "Yes") ? "1" : "0"); // Признак дебажности
        break;                                                    //
    case ModelType::AlarmStateAll:                                //
        parseTag(node, tags::addr, row, 0, "", true);             // Адрес
        parseTag(node, tags::string, row, 1);                     // Строка с сообщением
        parseTag(node, tags::type, row, 2);                       // Тип отображения
        break;                                                    //
    case ModelType::AlarmsCrit:                                   //
    case ModelType::AlarmsWarn:                                   //
    case ModelType::AlarmsInfo:                                   //
        parseTag(node, tags::addr, row, 0, "", true);             // Адрес
        parseTag(node, tags::string, row, 1);                     // Строка с сообщением
        parseTag(node, tags::order, row, 2);                      // Order
        parseAlarmHighlights(node, row, 3);                       // Highlights
        break;                                                    //
    case ModelType::WorkJours:                                    //
        parseTag(node, tags::addr, row, 0, "", true);             // Адрес
        parseTag(node, tags::desc, row, 1);                       // Строка с описанием
        break;                                                    //
    case ModelType::MeasJours:                                    //
        parseTag(node, tags::index, row, 0, "0", true);           // Индекс
        parseTag(node, tags::header, row, 1);                     // Заголовок столбца
        parseTag(node, tags::type, row, 2, "float32");            // Тип данных
        parseTag(node, tags::visibility, row, 3, "true");         // Видимость
        break;                                                    //
    case ModelType::Modbus:                                       //
        parseTag(node, tags::sig_id, row, 0, "", true);           // ID сигнала
        parseTag(node, tags::reg_type, row, 1);                   // Тип регистра
        parseTag(node, tags::type, row, 2);                       // Возвращаемый тип
        parseTag(node, tags::desc, row, 3);                       // Описание
        break;                                                    //
    case ModelType::Protocom:                                     //
        parseTag(node, tags::block, row, 0, "", true);            // Номер блока
        parseTag(node, tags::sig_id, row, 1);                     // ID сигнала
        break;                                                    //
    case ModelType::IEC60870:                                     //
        parseTag(node, tags::sig_id, row, 0, "", true);           // ID сигнала
        parseTag(node, tags::sig_type, row, 1);                   // Тип передачи
        parseTag(node, tags::trans_type, row, 2);                 // Тип передачи
        parseTag(node, tags::sig_group, row, 3);                  // Группа
        break;                                                    //
    case ModelType::Config:                                       //
        parseTag(node, tags::id, row, 0, "", true);               // ID (stored temporarily as int)
        {
            const auto &names = s2RecordsNameMap();
            auto id = data(index(row, 0)).toInt();
            auto nameIt = names.find(id);
            if (nameIt != names.cend())
                setData(index(row, 0), nameIt.value());       // replace id with human-readable name
        }
        parseTag(node, tags::def_val, row, 1);                // Значение по умолчанию
        parseTag(node, tags::count, row, 2, "");              // new count
        parseTag(node, tags::order, row, 3, "");              // Приоритет
        parseTag(node, tags::visibility, row, 4, "true");     // Видимость
        break;                                                //
    case ModelType::HiddenTab:                                //
        parseTag(node, tags::index, row, 0, "1", true);       // Индекс данных в структуре
        parseAttribute(node, tags::title, row, 1, "");        // Отображаемое название
        parseTag(node, tags::name, row, 2, "");               // Имя виджета в системе Qt
        parseAttribute(node, tags::view, row, 3, "LineEdit"); // Тип виджета для отображения
        parseTag(node, tags::type, row, 4, "uint32");         // Тип данных, хранимые в виджете
        parseTag(node, tags::addr, row, 5, "1", true);        // Адрес в блоке устройства
        parseTag(node, tags::visibility, row, 6, "true");     // Видимость
        break;                                                //
    case ModelType::BsiExt:                                   //
        parseTag(node, tags::addr, row, 0, "40", true);       // Адрес сигнала
        parseTag(node, tags::desc, row, 1, "");               // Описание сигнала
        parseTag(node, tags::type, row, 2, "uint32");         // Тип данных сигнала
        parseTag(node, tags::visibility, row, 3, "true");     // Видимость
        break;                                                //
    case ModelType::S2Tabs:                                   //
        parseTag(node, tags::id, row, 0, "", true);           // ID
        parseTag(node, tags::name, row, 1);                   // Наименование
        break;                                                //
    case ModelType::Includes:                                 //
        parseAttribute(node, tags::src, row, 0);              // Путь к файлу
        break;                                                //
    default:
        qWarning() << "Can't parse undefined tag of XML model!";
        break;
    }
}

void XmlDataModel::parseAlarmHighlights(QDomNode &node, int row, int col)
{
    QStringList highlightsValues = {};
    auto highlightsNode = node.firstChildElement(tags::highlights);
    if (!highlightsNode.isNull())
    {
        auto valueNode = highlightsNode.firstChild();
        while (!valueNode.isNull())
        {
            if (!valueNode.isComment())
            {
                auto value = valueNode.firstChild().toText().data();
                highlightsValues.append(value);
            }
            valueNode = valueNode.nextSibling();
        }
    }
    if (!highlightsValues.isEmpty())
        setData(index(row, col), highlightsValues.join(','));
}

QDomElement XmlDataModel::toNode(QDomDocument &doc)
{
    const auto prefs = getModelSettings();
    auto node = makeElement(doc, std::get<0>(prefs));
    for (auto row = 0; row < rowCount(); row++)
    {
        // Обходим элемент для возвращения назад стороной
        if (data(index(row, 0)).value<QString>() == "..")
            continue;

        auto item = makeElement(doc, std::get<1>(prefs));
        auto fillNode = std::get<2>(prefs);
        fillNode(doc, item, row);
        node.appendChild(item);
    }
    return node;
}

const QMap<int, QString> &XmlDataModel::s2RecordsNameMap()
{
    static QMap<int, QString> map;
    static QString loadedFrom;
    auto configDir = Settings::configDir();
    if (loadedFrom != configDir)
    {
        QFile file(QDir(configDir).filePath("s2files.xml"));
        if (file.open(QIODevice::ReadOnly))
        {
            map.clear();
            QDomDocument doc;
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
            QString errMsg;
            auto line = 0, column = 0;
            if (doc.setContent(&file, &errMsg, &line, &column))
#else
            QDomDocument::ParseResult result = doc.setContent(&file);
            if (result.errorMessage.isEmpty())
#endif
            {
                XmlParse::parseNode(doc.documentElement(), tags::records,
                    [&](const QDomNode &node)
                    {
                        int id = XmlParse::parseNumFromNode<int>(node, tags::id);
                        QString name = XmlParse::parseString(node, tags::name);
                        if (id > 0 && name != STRINF)
                            map[id] = name;
                    });
            }
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
            else
                qWarning() << errMsg;
#else
            else
                qWarning() << result.errorMessage;
#endif
            file.close();
            loadedFrom = configDir;
        }
    }
    return map;
}
