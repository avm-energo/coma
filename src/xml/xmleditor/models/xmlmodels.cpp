#include "xmlmodels.h"

// XmlBaseModel functions //

XmlBaseModel::XmlBaseModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

void XmlBaseModel::parseNode(QDomNode &node, int &row)
{
    auto itemIndex = index(row, 0);
    auto itemHeaderIndex = index(row, 1);
    auto nodeName = node.nodeName();
    auto desc = node.toElement().attribute(tags::desc, "");
    setData(itemIndex, nodeName);
    setData(itemHeaderIndex, desc);
}

QDomElement *XmlBaseModel::toNode(QDomDocument &doc)
{
    // Выбор имени тега исходя из типа модели
    QString name = "";
    if (mType == ModelType::Resources)
        name = tags::res;
    else if (mType == ModelType::Alarms)
        name = tags::alarms;
    else if (mType == ModelType::Journals)
        name = tags::journals;
    else
        name = "undefined";
    auto resNode = makeElement(doc, name);

    // Проход по всем дочерним моделям
    for (auto row = 0; row < rowCount(); row++)
    {
        auto childVar = data(index(row, 0), ModelNodeRole);
        if (childVar.isValid() && childVar.canConvert<ChildModelNode>())
        {
            auto child = childVar.value<ChildModelNode>();
            if (child.modelPtr != nullptr)
            {
                // Дочернюю модель в ноду
                auto childNode = child.modelPtr->toNode(doc);
                if (childNode != nullptr)
                {
                    // Добавляем описание (атрибут desc)
                    setAttribute(doc, *childNode, tags::desc, data(index(row, 1)));
                    resNode->appendChild(*childNode);
                }
            }
        }
    }
    return resNode;
}

// XmlSignalsModel functions //

XmlSignalsModel::XmlSignalsModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlSignalsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, tags::id, row, 0);         // ID сигнала
    parseTag(node, tags::start_addr, row, 1); // Стартовый адрес
    parseTag(node, tags::count, row, 2);      // Количество
}

QDomElement *XmlSignalsModel::toNode(QDomDocument &doc)
{
    auto sigNode = makeElement(doc, tags::sigs);
    // Записываем со второго элемента, т.к. первый элемент - возврат назад
    for (auto row = 1; row < rowCount(); row++)
    {
        auto sigItem = makeElement(doc, tags::sig);
        makeElement(doc, sigItem, tags::id, data(index(row, 0)));
        makeElement(doc, sigItem, tags::start_addr, data(index(row, 1)));
        makeElement(doc, sigItem, tags::count, data(index(row, 2)));
        sigNode->appendChild(*sigItem);
    }
    return sigNode;
}

// XmlSectionTabsModel functions //

XmlSectionTabsModel::XmlSectionTabsModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlSectionTabsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, tags::id, row, 0);    // ID вкладки
    parseTag(node, tags::name_, row, 1); // Имя вкладки
}

QDomElement *XmlSectionTabsModel::toNode(QDomDocument &doc)
{
    auto secTabNode = makeElement(doc, tags::tabs);
    for (auto row = 1; row < rowCount(); row++)
    {
        auto tabItem = makeElement(doc, tags::tab);
        makeElement(doc, tabItem, tags::id, data(index(row, 0)));
        makeElement(doc, tabItem, tags::name_, data(index(row, 1)));
        secTabNode->appendChild(*tabItem);
    }
    return secTabNode;
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

void XmlSectionsModel::create(const QStringList &saved, int *row)
{
    IEditorModel::create(saved, row);
    if (*row >= 0)
    {
        ChildModelNode node { nullptr, ModelType::Section };
        auto labels = XmlModel::headers.find(node.modelType)->second;
        node.modelPtr = new XmlSectionModel(1, labels.count(), node.modelType, this);
        node.modelPtr->setHorizontalHeaderLabels(labels);
        auto itemIndex = node.modelPtr->index(0, 0);
        node.modelPtr->setData(itemIndex, QString(".."));
        setData(index(*row, 0), QVariant::fromValue(node), ModelNodeRole);
    }
    emit modelChanged();
}

// TODO: Доделать
QDomElement *XmlSectionsModel::toNode(QDomDocument &doc)
{
    Q_UNUSED(doc);
    return nullptr;
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

void XmlSectionModel::create(const QStringList &saved, int *row)
{
    IEditorModel::create(saved, row);
    if (*row >= 0)
    {
        ChildModelNode node { nullptr, ModelType::SGroup };
        auto labels = XmlModel::headers.find(node.modelType)->second;
        node.modelPtr = new XmlSGroupModel(1, labels.count(), node.modelType, this);
        node.modelPtr->setHorizontalHeaderLabels(labels);
        auto itemIndex = node.modelPtr->index(0, 0);
        node.modelPtr->setData(itemIndex, QString(".."));
        setData(index(*row, 0), QVariant::fromValue(node), ModelNodeRole);
    }
    emit modelChanged();
}

// TODO: Доделать
QDomElement *XmlSectionModel::toNode(QDomDocument &doc)
{
    Q_UNUSED(doc);
    return nullptr;
}

// XmlAlarmsModel functions //

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
