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

// XmlCheckModel functions //

XmlCheckModel::XmlCheckModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlCheckModel::parseNode(QDomNode &node, int &row)
{
    Q_UNUSED(node);
    Q_UNUSED(row);
}

// XmlCheckGroupsModel functions //

XmlCheckGroupsModel::XmlCheckGroupsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlCheckGroupsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "name", row, 0); // Имя группы
    parseTag(node, "id", row, 1);   // Номер группы
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

// XmlCheckSignalsModel functions //

XmlCheckSignalsModel::XmlCheckSignalsModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlCheckSignalsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, "type", row, 0);       // Тип сигнала
    parseTag(node, "start-addr", row, 1); // Стартовый адрес
    parseTag(node, "count", row, 2);      // Количество
}
