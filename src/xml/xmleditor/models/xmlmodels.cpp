#include "xmlmodels.h"

// XmlMainModel functions //

XmlBaseModel::XmlBaseModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlBaseModel::parseNode(QDomNode &node, int &row)
{
    auto itemIndex = index(row, 0);
    auto itemHeaderIndex = index(row, 1);
    auto nodeName = node.nodeName();
    auto desc = node.toElement().attribute("desc", "");
    setData(itemIndex, nodeName);
    setData(itemHeaderIndex, desc);
}

// XmlCheckSignalsModel functions //

XmlSignalsModel::XmlSignalsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlSignalsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "id", row, 0);         // ID сигнала
    parseTag(node, "start-addr", row, 1); // Стартовый адрес
    parseTag(node, "count", row, 2);      // Количество
}

// XmlSectionTabsModel functions //

XmlSectionTabsModel::XmlSectionTabsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlSectionTabsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "name", row, 0); // Имя вкладки
    parseTag(node, "id", row, 1);   // ID вкладки
}

// XmlSectionsModel functions //

XmlSectionsModel::XmlSectionsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlSectionsModel::parseNode(QDomNode &node, int &row)
{
    auto itemHeaderIndex = index(row, 0);
    auto header = node.toElement().attribute("header", "");
    setData(itemHeaderIndex, header);
}

// XmlSectionModel functions //

XmlSectionModel::XmlSectionModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlSectionModel::parseNode(QDomNode &node, int &row)
{
    auto itemHeaderIndex = index(row, 0);
    auto header = node.toElement().attribute("header", "");
    auto itemTabIndex = index(row, 1);
    auto tab = node.toElement().attribute("tab", "");
    setData(itemHeaderIndex, header);
    setData(itemTabIndex, tab);
}

// XmlSectionGroupsModel functions //

XmlSectionGroupsModel::XmlSectionGroupsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlSectionGroupsModel::parseNode(QDomNode &node, int &row)
{
    parseAttribute(node, "desc", row, 0); // Заголовок
    parseTag(node, "start-addr", row, 1); // Адрес
}

// XmlCritAlarmsModel functions //

XmlAlarmsModel::XmlAlarmsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlAlarmsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "string", row, 0); // Строка с сообщением
    parseTag(node, "addr", row, 1);   // Адрес
}

// XmlWorkJoursModel functions //

XmlWorkJoursModel::XmlWorkJoursModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlWorkJoursModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "addr", row, 0); // Адрес
    parseTag(node, "desc", row, 1); // Строка с сообщением
}

// XmlMeasJoursModel functions //

XmlMeasJoursModel::XmlMeasJoursModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlMeasJoursModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "header", row, 0); // Адрес
}

// XmlModbusModel functions //

XmlModbusModel::XmlModbusModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlModbusModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "signal-id", row, 0); // ID сигнала
    parseTag(node, "reg-type", row, 1);  // Тип регистра
    parseTag(node, "type", row, 2);      // Возвращаемый тип
    parseTag(node, "desc", row, 3);      // Описание
}

// XmlProtocomModel functions //

XmlProtocomModel::XmlProtocomModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlProtocomModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "block", row, 0);     // Номер блока
    parseTag(node, "signal-id", row, 1); // ID сигнала
}

// XmlIECModel functions //

XmlIECModel::XmlIECModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlIECModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "signal-id", row, 0);  // ID сигнала
    parseTag(node, "sig-type", row, 1);   // Тип сигнала
    parseTag(node, "trans-type", row, 2); // Тип передачи
    parseTag(node, "sig-group", row, 3);  // Группа
}

// XmlConfigModel functions //

XmlConfigModel::XmlConfigModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlConfigModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "id", row, 0);           // ID
    parseTag(node, "defaultValue", row, 1); // Значение по умолчанию
}
