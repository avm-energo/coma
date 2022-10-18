#include "xmlmodels.h"

XmlContainerModel::XmlContainerModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

/// \brief Returns name of XML DOM node in dependency of model type.
QString XmlContainerModel::getModelTagName() const
{
    switch (mType)
    {
    case ModelType::Resources:
        return tags::res;
    case ModelType::Alarms:
        return tags::alarms;
    case ModelType::Journals:
        return tags::journals;
    case ModelType::Sections:
        return tags::sections;
    case ModelType::Section:
        return tags::section;
    default:
        return "undefined";
    }
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
    if (mType == ModelType::Sections || mType == ModelType::Section)
    {
        IEditorModel::create(saved, row);
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
                    if (mType == ModelType::Section)
                        // Добавляем номер вкладки (атрибут tab)
                        setAttribute(doc, childNode, tags::tab, data(index(row, 1)));
                }
                // Для узлов <resources>, <alarms> и <journals>
                else
                    // Добавляем описание (атрибут desc)
                    setAttribute(doc, childNode, tags::desc, data(index(row, 1)));
                node.appendChild(childNode);
            }
        }
    }
    return node;
}
