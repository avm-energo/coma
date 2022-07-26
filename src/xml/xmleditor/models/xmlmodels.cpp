#include "xmlmodels.h"

// XmlMainModel functions //

XmlBaseModel::XmlBaseModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
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

XmlSignalsModel::XmlSignalsModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlSignalsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "id", row, 0);         // ID сигнала
    parseTag(node, "start-addr", row, 1); // Стартовый адрес
    parseTag(node, "count", row, 2);      // Количество
}

// XmlSectionTabsModel functions //

XmlSectionTabsModel::XmlSectionTabsModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlSectionTabsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "id", row, 0);   // ID вкладки
    parseTag(node, "name", row, 1); // Имя вкладки
}

// XmlSectionsModel functions //

XmlSectionsModel::XmlSectionsModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlSectionsModel::parseNode(QDomNode &node, int &row)
{
    parseAttribute(node, "header", row, 0); // Заголовок
}

// XmlSectionModel functions //

XmlSectionModel::XmlSectionModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlSectionModel::parseNode(QDomNode &node, int &row)
{
    parseAttribute(node, "header", row, 0); // Заголовок
    parseAttribute(node, "tab", row, 1);    // ID вкладки
}

// XmlCritAlarmsModel functions //

XmlAlarmsModel::XmlAlarmsModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

void XmlAlarmsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "string", row, 0); // Строка с сообщением
    parseTag(node, "addr", row, 1);   // Адрес
}

// XmlWorkJoursModel functions //

XmlWorkJoursModel::XmlWorkJoursModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlWorkJoursModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "addr", row, 0); // Адрес
    parseTag(node, "desc", row, 1); // Строка с сообщением
}

// XmlMeasJoursModel functions //

XmlMeasJoursModel::XmlMeasJoursModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlMeasJoursModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "header", row, 0); // Адрес
}

// XmlModbusModel functions //

XmlModbusModel::XmlModbusModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
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

XmlProtocomModel::XmlProtocomModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlProtocomModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "block", row, 0);     // Номер блока
    parseTag(node, "signal-id", row, 1); // ID сигнала
}

// XmlIECModel functions //

XmlIecModel::XmlIecModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

void XmlIecModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "signal-id", row, 0);  // ID сигнала
    parseTag(node, "sig-type", row, 1);   // Тип сигнала
    parseTag(node, "trans-type", row, 2); // Тип передачи
    parseTag(node, "sig-group", row, 3);  // Группа
}

// XmlConfigModel functions //

XmlConfigModel::XmlConfigModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

void XmlConfigModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "id", row, 0);           // ID
    parseTag(node, "defaultValue", row, 1); // Значение по умолчанию
}
