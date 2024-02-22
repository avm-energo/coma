#include "xmldatamodel.h"

XmlDataModel::XmlDataModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

/// \brief Returns saving settings for XML model in dependency of model type.
std::tuple<QString, QString, std::function<void(QDomDocument &, QDomElement &, int &)>> XmlDataModel::getModelSettings()
{
    auto alarmHelper = [this](auto &doc, auto &item, auto &row) {
        makeElement(doc, item, tags::addr, data(index(row, 0)));
        makeElement(doc, item, tags::string, data(index(row, 1)));
        if (rowCount() > 2)
        {
            auto highlights = data(index(row, 2)).toString().split(',');
            if (!(highlights.isEmpty() || (highlights.size() == 1 && highlights.first().isEmpty())))
            {
                auto highlightsNode = makeElement(doc, tags::highlights);
                for (const auto &value : qAsConst(highlights))
                    makeElement(doc, highlightsNode, tags::item, value);
                item.appendChild(highlightsNode);
            }
        }
    };

    switch (mType)
    {
    case ModelType::Signals:
        return { tags::sigs, tags::sig, //
            [this](auto &doc, auto &item, auto &row) {
                makeElement(doc, item, tags::start_addr, data(index(row, 0)));
                makeElement(doc, item, tags::count, data(index(row, 1)));
                makeElement(doc, item, tags::id, data(index(row, 2)));
                makeElement(doc, item, tags::type, data(index(row, 3)));
            } };
    case ModelType::SectionTabs:
        return { tags::tabs, tags::tab, //
            [this](auto &doc, auto &item, auto &row) {
                makeElement(doc, item, tags::id, data(index(row, 0)));
                makeElement(doc, item, tags::name, data(index(row, 1)));
            } };
    case ModelType::AlarmsCrit:
        return { tags::crit, tags::item, alarmHelper };
    case ModelType::AlarmsWarn:
        return { tags::warn, tags::item, alarmHelper };
    case ModelType::AlarmsInfo:
        return { tags::info, tags::item, alarmHelper };
    case ModelType::WorkJours:
        return { tags::work, tags::item, //
            [this](auto &doc, auto &item, auto &row) {
                makeElement(doc, item, tags::addr, data(index(row, 0)));
                makeElement(doc, item, tags::desc, data(index(row, 1)));
            } };
    case ModelType::MeasJours:
        return { tags::meas, tags::item, //
            [this](auto &doc, auto &item, auto &row) {
                makeElement(doc, item, tags::index, data(index(row, 0)));
                makeElement(doc, item, tags::header, data(index(row, 1)));
                makeElement(doc, item, tags::type, data(index(row, 2)));
                makeElement(doc, item, tags::visibility, data(index(row, 3)));
            } };
    case ModelType::Modbus:
        return { tags::modbus, tags::group, //
            [this](auto &doc, auto &item, auto &row) {
                makeElement(doc, item, tags::sig_id, data(index(row, 0)));
                makeElement(doc, item, tags::reg_type, data(index(row, 1)));
                makeElement(doc, item, tags::type, data(index(row, 2)));
                makeElement(doc, item, tags::desc, data(index(row, 3)));
            } };
    case ModelType::Protocom:
        return { tags::protocom, tags::group, //
            [this](auto &doc, auto &item, auto &row) {
                makeElement(doc, item, tags::block, data(index(row, 0)));
                makeElement(doc, item, tags::sig_id, data(index(row, 1)));
            } };
    case ModelType::IEC60870:
        return { tags::iec, tags::group, //
            [this](auto &doc, auto &item, auto &row) {
                makeElement(doc, item, tags::sig_id, data(index(row, 0)));
                makeElement(doc, item, tags::sig_type, data(index(row, 1)));
                makeElement(doc, item, tags::trans_type, data(index(row, 2)));
                makeElement(doc, item, tags::sig_group, data(index(row, 3)));
            } };
    case ModelType::Config:
        return { tags::config, tags::record, //
            [this](auto &doc, auto &item, auto &row) {
                makeElement(doc, item, tags::id, data(index(row, 0)));
                makeElement(doc, item, tags::def_val, data(index(row, 1)));
                QVariant countData(data(index(row, 2)));
                if (!countData.value<QString>().isEmpty())
                    makeElement(doc, item, tags::count, countData);
                makeElement(doc, item, tags::visibility, data(index(row, 3)));
            } };
    case ModelType::HiddenTab:
        return { tags::hidden_tab, tags::mwidget, //
            [this](auto &doc, auto &item, auto &row) {
                makeElement(doc, item, tags::index, data(index(row, 0)));
                setAttribute(doc, item, tags::title, data(index(row, 1)));
                makeElement(doc, item, tags::name, data(index(row, 2)));
                setAttribute(doc, item, tags::view, data(index(row, 3)));
                makeElement(doc, item, tags::type, data(index(row, 4)));
                makeElement(doc, item, tags::addr, data(index(row, 5)));
                makeElement(doc, item, tags::visibility, data(index(row, 6)));
            } };
    default:
        qWarning() << "Model settings not found!";
        return { "undefined", "undefined", //
            [](auto &doc, auto &item, auto &row) {
                Q_UNUSED(doc);
                Q_UNUSED(item);
                Q_UNUSED(row);
            } };
    }
}

/// \brief Parsing input XML nodes of file in model items.
void XmlDataModel::parseNode(QDomNode &node, int &row)
{
    switch (mType)
    {
    case ModelType::Signals:
        parseTag(node, tags::start_addr, row, 0, "", true);   // Стартовый адрес
        parseTag(node, tags::count, row, 1);                  // Количество
        parseTag(node, tags::id, row, 2);                     // ID сигнала
        parseTag(node, tags::type, row, 3, "Float");          // Тип сигнала
        break;                                                //
    case ModelType::SectionTabs:                              //
        parseTag(node, tags::id, row, 0, "", true);           // ID вкладки
        parseTag(node, tags::name, row, 1);                   // Имя вкладки
        break;                                                //
    case ModelType::AlarmsCrit:                               //
    case ModelType::AlarmsWarn:                               //
    case ModelType::AlarmsInfo:                               //
        parseTag(node, tags::addr, row, 0, "", true);         // Адрес
        parseTag(node, tags::string, row, 1);                 // Строка с сообщением
        if (rowCount() > 2)                                   // Highlights
            parseAlarmHighlights(node, row, 2);               //
        break;                                                //
    case ModelType::WorkJours:                                //
        parseTag(node, tags::addr, row, 0, "", true);         // Адрес
        parseTag(node, tags::desc, row, 1);                   // Строка с описанием
        break;                                                //
    case ModelType::MeasJours:                                //
        parseTag(node, tags::index, row, 0, "0", true);       // Индекс
        parseTag(node, tags::header, row, 1);                 // Заголовок столбца
        parseTag(node, tags::type, row, 2, "float32");        // Тип данных
        parseTag(node, tags::visibility, row, 3, "true");     // Видимость
        break;                                                //
    case ModelType::Modbus:                                   //
        parseTag(node, tags::sig_id, row, 0, "", true);       // ID сигнала
        parseTag(node, tags::reg_type, row, 1);               // Тип регистра
        parseTag(node, tags::type, row, 2);                   // Возвращаемый тип
        parseTag(node, tags::desc, row, 3);                   // Описание
        break;                                                //
    case ModelType::Protocom:                                 //
        parseTag(node, tags::block, row, 0, "", true);        // Номер блока
        parseTag(node, tags::sig_id, row, 1);                 // ID сигнала
        break;                                                //
    case ModelType::IEC60870:                                 //
        parseTag(node, tags::sig_id, row, 0, "", true);       // ID сигнала
        parseTag(node, tags::sig_type, row, 1);               // Тип передачи
        parseTag(node, tags::trans_type, row, 2);             // Тип передачи
        parseTag(node, tags::sig_group, row, 3);              // Группа
        break;                                                //
    case ModelType::Config:                                   //
        parseTag(node, tags::id, row, 0, "", true);           // ID
        parseTag(node, tags::def_val, row, 1);                // Значение по умолчанию
        parseTag(node, tags::count, row, 2, "");              // new count
        parseTag(node, tags::visibility, row, 3, "true");     // Видимость
        break;                                                //
    case ModelType::HiddenTab:                                //
        parseTag(node, tags::index, row, 0, "1", true);       // Индекс данных в структуре
        parseAttribute(node, tags::title, row, 1, "");        // Отображаемое название
        parseTag(node, tags::name, row, 2, "");               // Имя виджета в системе Qt
        parseAttribute(node, tags::view, row, 3, "LineEdit"); // Тип виджета для отображения
        parseTag(node, tags::type, row, 4, "uint32");         // Тип данных, хранимые в виджете
        parseTag(node, tags::addr, row, 5, "1", true);        // Адрес в блоке устройства
        parseTag(node, tags::visibility, row, 6, "true");     // Видимость
        break;
    default:
        qWarning() << "Can't parse undefined tag of XML model!";
        break;
    }
}

/// \brief Parsing <highlights> node from alarm DOM nodes (<crit> and <warn> only).
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

/// \brief Creates XML DOM node representation of current model.
QDomElement XmlDataModel::toNode(QDomDocument &doc)
{
    const auto prefs = getModelSettings();
    auto node = makeElement(doc, std::get<0>(prefs));
    for (auto row = 0; row < rowCount(); row++)
    {
        // TODO: костыль
        if (data(index(row, 0)).value<QString>() != "..")
        {
            auto item = makeElement(doc, std::get<1>(prefs));
            auto fillNode = std::get<2>(prefs);
            fillNode(doc, item, row);
            node.appendChild(item);
        }
    }
    return node;
}
