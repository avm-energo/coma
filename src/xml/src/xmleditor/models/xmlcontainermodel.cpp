#include "xml/xmleditor/models/xmlcontainermodel.h"

#include <xml/xmleditor/models/xmldatamodel.h>
#include <xml/xmleditor/models/xmlhidedatamodel.h>
#include <xml/xmltags.h>

XmlContainerModel::XmlContainerModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

/// \brief Returns name of XML DOM node in dependency of model type.
QString XmlContainerModel::getModelTagName() const
{
    static const std::map<ModelType, QString> tagByModelType = {
        { ModelType::Resources, tags::res },     //
        { ModelType::Alarms, tags::alarms },     //
        { ModelType::Journals, tags::journals }, //
        { ModelType::Sections, tags::sections }, //
        { ModelType::Section, tags::section },   //
        { ModelType::Hidden, tags::hidden },     //
    };
    auto search = tagByModelType.find(mType);
    if (search != tagByModelType.cend())
        return search->second;
    else
        return "undefined";
}

/// \brief Parsing input XML nodes of file in model items.
void XmlContainerModel::parseNode(QDomNode &node, int &row)
{
    // Для узлов <sections> и <section>
    if (mType == ModelType::Sections || mType == ModelType::Section)
    {
        parseAttribute(node, tags::header, row, 0);  // Заголовок
        if (mType == ModelType::Section)             //
            parseAttribute(node, tags::tab, row, 1); // ID вкладки
    }
    // Для узлов <hidden>
    else if (mType == ModelType::Hidden)
    {
        parseAttribute(node, tags::desc, row, 0);
        parseAttribute(node, tags::prefix, row, 1);
        parseAttribute(node, tags::flag, row, 2, "0");
        parseAttribute(node, tags::background, row, 3);
    }
    // Для узлов <resources>, <alarms> и <journals>
    else
    {
        auto nodeName = node.nodeName();
        auto desc = node.toElement().attribute(tags::desc, "");
        setData(index(row, 0), nodeName);
        setData(index(row, 1), desc);
    }
}

/// \brief Slot for inserting a new item in the model.
void XmlContainerModel::create(const QStringList &saved, int *row)
{
    // Создание дочерних элементов доступно для узлов <sections> и <section>
    if (mType == ModelType::Sections || mType == ModelType::Section || mType == ModelType::Hidden)
    {
        BaseEditorModel::create(saved, row);
        if (*row >= 0)
        {
            ChildModelNode node { nullptr, ModelType::None };
            QStringList labels;
            if (mType == ModelType::Sections)
            {
                node.modelType = ModelType::Section;
                labels = XmlModel::headers.find(node.modelType)->second;
                // Так как узел <sections> содержит узлы <section>
                node.modelPtr = new XmlContainerModel(1, labels.count(), node.modelType, this);
            }
            else if (mType == ModelType::Hidden)
            {
                node.modelType = ModelType::HiddenTab;
                labels = XmlModel::headers.find(node.modelType)->second;
                node.modelPtr = new XmlDataModel(1, labels.count(), node.modelType, this);
            }
            else
            {
                node.modelType = ModelType::SGroup;
                labels = XmlModel::headers.find(node.modelType)->second;
                // Узел <section> содержит узлы <sgroup>
                node.modelPtr = new XmlHideDataModel(1, labels.count(), node.modelType, this);
            }
            node.modelPtr->setHorizontalHeaderLabels(labels);
            auto itemIndex = node.modelPtr->index(0, 0);
            node.modelPtr->setData(itemIndex, QString(".."));
            setData(index(*row, 0), QVariant::fromValue(node), ModelNodeRole);
        }
        emit modelChanged();
    }
}

/// \brief Creates XML DOM node representation of current model.
QDomElement XmlContainerModel::toNode(QDomDocument &doc)
{
    // Выбор имени тега исходя из типа модели
    const auto tagName = getModelTagName();
    auto node = makeElement(doc, tagName);
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
                // Для узлов <sections> и <section>
                if (mType == ModelType::Sections || mType == ModelType::Section)
                {
                    // Добавляем описание (атрибут header)
                    setAttribute(doc, childNode, tags::header, data(index(row, 0)));
                    // Добавляем номер вкладки (атрибут tab)
                    if (mType == ModelType::Section)
                        setAttribute(doc, childNode, tags::tab, data(index(row, 1)));
                }
                // Для узлов <hidden>
                else if (mType == ModelType::Hidden)
                {
                    setAttribute(doc, childNode, tags::desc, data(index(row, 0)));
                    setAttribute(doc, childNode, tags::prefix, data(index(row, 1)));
                    setAttribute(doc, childNode, tags::flag, data(index(row, 2)));
                    setAttribute(doc, childNode, tags::background, data(index(row, 3)));
                }
                // Для узлов <resources>, <alarms> и <journals>
                else
                {
                    // Добавляем описание (атрибут desc)
                    setAttribute(doc, childNode, tags::desc, data(index(row, 1)));
                }
                node.appendChild(childNode);
            }
        }
    }
    return node;
}

/// \brief Slot for receiving a request from dialog and emits signal with response.
/// \details Override needed for XmlResourceModel.
void XmlContainerModel::getDialogRequest(const int row)
{
    if (row >= 0 && row < rowCount())
    {
        if (mType == ModelType::Resources)
        {
            QStringList response;
            auto desc = data(index(row, 1));
            if (desc.isValid() && desc.canConvert<QString>())
                response.append(desc.value<QString>());
            emit sendDialogResponse(response);
        }
        else
            XmlModel::getDialogRequest(row);
    }
}

/// \brief Slot for updating an item's data in the model.
/// \details Override needed for XmlResourceModel.
void XmlContainerModel::update(const QStringList &saved, const int row)
{
    if (row >= 0 && row < rowCount())
    {
        if (mType == ModelType::Resources)
        {
            setData(index(row, 1), saved[0]);
            emit modelChanged();
        }
        else
            XmlModel::update(saved, row);
    }
}
