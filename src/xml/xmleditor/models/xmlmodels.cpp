#include "xmlmodels.h"

// XmlMainModel functions //

XmlMainModel::XmlMainModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlMainModel::parseNode(QDomNode &node, int &row)
{
    Q_UNUSED(node);
    Q_UNUSED(row);
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

// XmlSectionGroupsModel functions //

XmlSectionGroupsModel::XmlSectionGroupsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlSectionGroupsModel::parseNode(QDomNode &node, int &row)
{
    parseAttribute(node, "desc", row, 0); // Заголовок
    parseTag(node, "start-addr", row, 1); // Адрес
}

/*
// XmlChecksModel functions //

XmlChecksModel::XmlChecksModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlChecksModel::parseNode(QDomNode &node, int &row)
{
    Q_UNUSED(node);
    Q_UNUSED(row);
}

// XmlCheckRecordsModel functions //
XmlCheckRecordsModel::XmlCheckRecordsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlCheckRecordsModel::parseNode(QDomNode &node, int &row)
{
    parseAttribute(node, "header", row, 0); // Заголовок
    parseAttribute(node, "desc", row, 1);   // Описание
    parseTag(node, "group", row, 2);        // Группа
    parseTag(node, "start-addr", row, 3);   // Стартовый адрес
    parseTag(node, "count", row, 4);        // Количество
}
*/

// XmlAlarmsModel functions //

XmlAlarmsModel::XmlAlarmsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlAlarmsModel::parseNode(QDomNode &node, int &row)
{
    Q_UNUSED(node);
    Q_UNUSED(row);
}

// XmlCritAlarmsModel functions //

XmlCritAlarmsModel::XmlCritAlarmsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlCritAlarmsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "addr", row, 0);   // Адрес
    parseTag(node, "string", row, 1); // Строка с сообщением
}

// XmlWarnAlarmsModel functions //

XmlWarnAlarmsModel::XmlWarnAlarmsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlWarnAlarmsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "addr", row, 0);   // Адрес
    parseTag(node, "string", row, 1); // Строка с сообщением
}

// XmlJoursModel functions //

XmlJoursModel::XmlJoursModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlJoursModel::parseNode(QDomNode &node, int &row)
{
    Q_UNUSED(node);
    Q_UNUSED(row);
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
    parseTag(node, "signal-id", row, 0); // ID сигнала
    parseTag(node, "block", row, 1);     // Номер блока
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
