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
    parseAttribute(node, tags::header, row, 0); // Заголовок
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

QDomElement *XmlSectionsModel::toNode(QDomDocument &doc)
{
    auto sectionsNode = makeElement(doc, tags::sections);
    // Проход по дочерним моделям
    for (auto row = 1; row < rowCount(); row++)
    {
        auto dataVar = data(index(row, 0), ModelNodeRole);
        if (dataVar.isValid() && dataVar.canConvert<ChildModelNode>())
        {
            auto sectionModel = dataVar.value<ChildModelNode>();
            if (sectionModel.modelPtr != nullptr)
            {
                // Дочернюю XmlSectionModel в ноду
                auto sectionNode = sectionModel.modelPtr->toNode(doc);
                if (sectionNode != nullptr)
                {
                    // Добавляем описание (атрибут header)
                    setAttribute(doc, *sectionNode, tags::header, data(index(row, 0)));
                    sectionsNode->appendChild(*sectionNode);
                }
            }
        }
    }
    return sectionsNode;
}

// XmlSectionModel functions //

XmlSectionModel::XmlSectionModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlSectionModel::parseNode(QDomNode &node, int &row)
{
    parseAttribute(node, tags::header, row, 0); // Заголовок
    parseAttribute(node, tags::tab, row, 1);    // ID вкладки
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

QDomElement *XmlSectionModel::toNode(QDomDocument &doc)
{
    auto sectionNode = makeElement(doc, tags::section);
    // Проход по дочерним моделям
    for (auto row = 1; row < rowCount(); row++)
    {
        auto dataVar = data(index(row, 0), ModelNodeRole);
        if (dataVar.isValid() && dataVar.canConvert<ChildModelNode>())
        {
            auto sgroupModel = dataVar.value<ChildModelNode>();
            if (sgroupModel.modelPtr != nullptr)
            {
                // Дочернюю модель (section) в ноду
                auto sgroupNode = sgroupModel.modelPtr->toNode(doc);
                if (sgroupNode != nullptr)
                {
                    // Добавляем атрибуты
                    setAttribute(doc, *sgroupNode, tags::header, data(index(row, 0)));
                    setAttribute(doc, *sgroupNode, tags::tab, data(index(row, 1)));
                    sectionNode->appendChild(*sgroupNode);
                }
            }
        }
    }
    return sectionNode;
}

// XmlAlarmsModel functions //

XmlAlarmsModel::XmlAlarmsModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

void XmlAlarmsModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, tags::string, row, 0); // Строка с сообщением
    parseTag(node, tags::addr, row, 1);   // Адрес
}

QDomElement *XmlAlarmsModel::toNode(QDomDocument &doc)
{
    // Выбор имени узла
    QString nodeName = "";
    if (mType == ModelType::AlarmsCrit)
        nodeName = tags::critical;
    else if (mType == ModelType::AlarmsWarn)
        nodeName = tags::warning;
    else
        nodeName = tags::info;

    // Обход элементов
    auto alarmsNode = makeElement(doc, nodeName);
    for (auto row = 1; row < rowCount(); row++)
    {
        auto alarmItem = makeElement(doc, tags::item);
        makeElement(doc, alarmItem, tags::string, data(index(row, 0)));
        makeElement(doc, alarmItem, tags::addr, data(index(row, 1)));
        alarmsNode->appendChild(*alarmItem);
    }
    return alarmsNode;
}

// XmlWorkJoursModel functions //

XmlWorkJoursModel::XmlWorkJoursModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlWorkJoursModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, tags::addr, row, 0); // Адрес
    parseTag(node, tags::desc, row, 1); // Строка с сообщением
}

QDomElement *XmlWorkJoursModel::toNode(QDomDocument &doc)
{
    // Обход элементов
    auto workJoursNode = makeElement(doc, tags::work);
    for (auto row = 1; row < rowCount(); row++)
    {
        auto jourItem = makeElement(doc, tags::item);
        makeElement(doc, jourItem, tags::addr, data(index(row, 0)));
        makeElement(doc, jourItem, tags::desc, data(index(row, 1)));
        workJoursNode->appendChild(*jourItem);
    }
    return workJoursNode;
}

// XmlMeasJoursModel functions //

XmlMeasJoursModel::XmlMeasJoursModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlMeasJoursModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, tags::header, row, 0); // Название
}

QDomElement *XmlMeasJoursModel::toNode(QDomDocument &doc)
{
    // Обход элементов
    auto measJoursNode = makeElement(doc, tags::meas);
    for (auto row = 1; row < rowCount(); row++)
    {
        auto jourItem = makeElement(doc, tags::item);
        makeElement(doc, jourItem, tags::header, data(index(row, 0)));
        measJoursNode->appendChild(*jourItem);
    }
    return measJoursNode;
}

// XmlModbusModel functions //

XmlModbusModel::XmlModbusModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

void XmlModbusModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, tags::sig_id, row, 0);   // ID сигнала
    parseTag(node, tags::reg_type, row, 1); // Тип регистра
    parseTag(node, tags::type, row, 2);     // Возвращаемый тип
    parseTag(node, tags::desc, row, 3);     // Описание
}

QDomElement *XmlModbusModel::toNode(QDomDocument &doc)
{
    // Обход элементов
    auto modbusNode = makeElement(doc, tags::modbus);
    for (auto row = 1; row < rowCount(); row++)
    {
        auto modbusItem = makeElement(doc, tags::group);
        makeElement(doc, modbusItem, tags::sig_id, data(index(row, 0)));
        makeElement(doc, modbusItem, tags::reg_type, data(index(row, 1)));
        makeElement(doc, modbusItem, tags::type, data(index(row, 2)));
        makeElement(doc, modbusItem, tags::desc, data(index(row, 3)));
        modbusNode->appendChild(*modbusItem);
    }
    return modbusNode;
}

// XmlProtocomModel functions //

XmlProtocomModel::XmlProtocomModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlProtocomModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, tags::block, row, 0);  // Номер блока
    parseTag(node, tags::sig_id, row, 1); // ID сигнала
}

QDomElement *XmlProtocomModel::toNode(QDomDocument &doc)
{
    // Обход элементов
    auto protocomNode = makeElement(doc, tags::protocom);
    for (auto row = 1; row < rowCount(); row++)
    {
        auto protocomItem = makeElement(doc, tags::group);
        makeElement(doc, protocomItem, tags::block, data(index(row, 0)));
        makeElement(doc, protocomItem, tags::sig_id, data(index(row, 1)));
        protocomNode->appendChild(*protocomItem);
    }
    return protocomNode;
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

QDomElement *XmlIecModel::toNode(QDomDocument &doc)
{
    // Обход элементов
    auto iecNode = makeElement(doc, tags::iec60870);
    for (auto row = 1; row < rowCount(); row++)
    {
        auto iecItem = makeElement(doc, tags::group);
        makeElement(doc, iecItem, tags::sig_id, data(index(row, 0)));
        makeElement(doc, iecItem, tags::sig_type, data(index(row, 1)));
        makeElement(doc, iecItem, tags::trans_type, data(index(row, 2)));
        makeElement(doc, iecItem, tags::sig_group, data(index(row, 3)));
        iecNode->appendChild(*iecItem);
    }
    return iecNode;
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

QDomElement *XmlConfigModel::toNode(QDomDocument &doc)
{
    // Обход элементов
    auto configNode = makeElement(doc, tags::config);
    for (auto row = 1; row < rowCount(); row++)
    {
        auto configItem = makeElement(doc, tags::record);
        makeElement(doc, configItem, tags::id, data(index(row, 0)));
        makeElement(doc, configItem, tags::def_val, data(index(row, 1)));
        configNode->appendChild(*configItem);
    }
    return configNode;
}
