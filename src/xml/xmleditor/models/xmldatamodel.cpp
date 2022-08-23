#include "xmlmodels.h"

XmlDataModel::XmlDataModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

std::tuple<QString, QString, std::function<void(QDomDocument &, QDomElement *, int &)>>
XmlDataModel::getModelPreferences()
{
    auto alarmHelper = [&](QDomDocument &doc, QDomElement *item, int &row) {
        makeElement(doc, item, tags::string, data(index(row, 0)));
        makeElement(doc, item, tags::addr, data(index(row, 1)));
    };

    switch (mType)
    {
    case ModelType::Signals:
        return { tags::sigs, tags::sig, //
            [&](QDomDocument &doc, QDomElement *item, int &row) {
                makeElement(doc, item, tags::id, data(index(row, 0)));
                makeElement(doc, item, tags::start_addr, data(index(row, 1)));
                makeElement(doc, item, tags::count, data(index(row, 2)));
            } };
    case ModelType::SectionTabs:
        return { tags::tabs, tags::tab, //
            [&](QDomDocument &doc, QDomElement *item, int &row) {
                makeElement(doc, item, tags::id, data(index(row, 0)));
                makeElement(doc, item, tags::name, data(index(row, 1)));
            } };
    case ModelType::AlarmsCrit:
        return { tags::critical, tags::item, alarmHelper };
    case ModelType::AlarmsWarn:
        return { tags::warning, tags::item, alarmHelper };
    case ModelType::AlarmsInfo:
        return { tags::info, tags::item, alarmHelper };
    case ModelType::WorkJours:
        return { tags::work, tags::item, //
            [&](QDomDocument &doc, QDomElement *item, int &row) {
                makeElement(doc, item, tags::addr, data(index(row, 0)));
                makeElement(doc, item, tags::desc, data(index(row, 1)));
            } };
    case ModelType::MeasJours:
        return { tags::meas, tags::item, //
            [&](QDomDocument &doc, QDomElement *item, int &row) {
                makeElement(doc, item, tags::header, data(index(row, 0)));
            } };
    case ModelType::Modbus:
        return { tags::modbus, tags::group, //
            [&](QDomDocument &doc, QDomElement *item, int &row) {
                makeElement(doc, item, tags::sig_id, data(index(row, 0)));
                makeElement(doc, item, tags::reg_type, data(index(row, 1)));
                makeElement(doc, item, tags::type, data(index(row, 2)));
                makeElement(doc, item, tags::desc, data(index(row, 3)));
            } };
    case ModelType::Protocom:
        return { tags::protocom, tags::group, //
            [&](QDomDocument &doc, QDomElement *item, int &row) {
                makeElement(doc, item, tags::block, data(index(row, 0)));
                makeElement(doc, item, tags::sig_id, data(index(row, 1)));
            } };
    case ModelType::IEC60870:
        return { tags::iec60870, tags::group, //
            [&](QDomDocument &doc, QDomElement *item, int &row) {
                makeElement(doc, item, tags::sig_id, data(index(row, 0)));
                makeElement(doc, item, tags::sig_type, data(index(row, 1)));
                makeElement(doc, item, tags::trans_type, data(index(row, 2)));
                makeElement(doc, item, tags::sig_group, data(index(row, 3)));
            } };
    case ModelType::Config:
        return { tags::config, tags::record, //
            [&](QDomDocument &doc, QDomElement *item, int &row) {
                makeElement(doc, item, tags::id, data(index(row, 0)));
                makeElement(doc, item, tags::def_val, data(index(row, 1)));
            } };
    default:
        return { "undefined", "undefined", //
            [](QDomDocument &doc, QDomElement *item, int &row) {
                Q_UNUSED(doc);
                Q_UNUSED(item);
                Q_UNUSED(row);
            } };
    }
}

void XmlDataModel::parseNode(QDomNode &node, int &row)
{
    if (mType == ModelType::Signals)
    {
        parseTag(node, tags::id, row, 0);         // ID сигнала
        parseTag(node, tags::start_addr, row, 1); // Стартовый адрес
        parseTag(node, tags::count, row, 2);      // Количество
    }
    else if (mType == ModelType::SectionTabs)
    {

        parseTag(node, tags::id, row, 0);   // ID вкладки
        parseTag(node, tags::name, row, 1); // Имя вкладки
    }
    else if (mType == ModelType::AlarmsCrit || //
        mType == ModelType::AlarmsWarn ||      //
        mType == ModelType::AlarmsInfo)        //
    {
        parseTag(node, tags::string, row, 0); // Строка с сообщением
        parseTag(node, tags::addr, row, 1);   // Адрес
    }
    else if (mType == ModelType::WorkJours)
    {
        parseTag(node, tags::addr, row, 0); // Адрес
        parseTag(node, tags::desc, row, 1); // Строка с сообщением
    }
    else if (mType == ModelType::MeasJours)
    {
        parseTag(node, tags::header, row, 0); // Название
    }
    else if (mType == ModelType::Modbus)
    {
        parseTag(node, tags::sig_id, row, 0);   // ID сигнала
        parseTag(node, tags::reg_type, row, 1); // Тип регистра
        parseTag(node, tags::type, row, 2);     // Возвращаемый тип
        parseTag(node, tags::desc, row, 3);     // Описание
    }
    else if (mType == ModelType::Protocom)
    {
        parseTag(node, tags::block, row, 0);  // Номер блока
        parseTag(node, tags::sig_id, row, 1); // ID сигнала
    }
    else if (mType == ModelType::IEC60870)
    {
        parseTag(node, "signal-id", row, 0);  // ID сигнала
        parseTag(node, "sig-type", row, 1);   // Тип сигнала
        parseTag(node, "trans-type", row, 2); // Тип передачи
        parseTag(node, "sig-group", row, 3);  // Группа
    }
    else
    {
        parseTag(node, "id", row, 0);           // ID
        parseTag(node, "defaultValue", row, 1); // Значение по умолчанию
    }
}

QDomElement *XmlDataModel::toNode(QDomDocument &doc)
{
    const auto prefs = getModelPreferences();
    auto node = makeElement(doc, std::get<0>(prefs));
    // Записываем со второго элемента, т.к. первый элемент - возврат назад
    for (auto row = 1; row < rowCount(); row++)
    {
        auto item = makeElement(doc, std::get<1>(prefs));
        auto fillNode = std::get<2>(prefs);
        fillNode(doc, item, row);
        node->appendChild(*item);
    }
    return node;
}
