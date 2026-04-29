#include "xml/xmleditor/models/xmlcontainermodel.h"

#include <xml/xmleditor/models/modelfabric.h>
#include <xml/xmleditor/models/xmldatamodel.h>
#include <xml/xmleditor/models/xmlhidedatamodel.h>
#include <xml/xmltags.h>

XmlContainerModel::XmlContainerModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

void XmlContainerModel::parseDataNode(QDomNode &child, int &row)
{
    if (m_type == ModelType::Overlay)
    {
        auto childNodeName = child.nodeName();
        ModelType childType = ModelType::None;
        if (childNodeName == tags::records)
            childType = ModelType::OverlayRecords;
        else
        {
            auto it = XmlModel::s_types.find(childNodeName);
            if (it != XmlModel::s_types.cend())
                childType = it->second;
        }
        if (childType != ModelType::None)
        {
            ChildModelNode modelNode { nullptr, childType };
            ModelFabric::createChildModel(modelNode, child, this);
            auto itemIndex = index(row, 0);
            setData(itemIndex, QVariant::fromValue(modelNode), ModelNodeRole);
        }
        parseNode(child, row);
        row++;
    }
    else
        XmlModel::parseDataNode(child, row);
}

QString XmlContainerModel::getModelTagName() const
{
    static const std::map<ModelType, QString> tagByModelType = {
        { ModelType::Resources, tags::res },     //
        { ModelType::Alarms, tags::alarms },     //
        { ModelType::Journals, tags::journals }, //
        { ModelType::Sections, tags::sections }, //
        { ModelType::Section, tags::section },   //
        { ModelType::Hidden, tags::hidden },     //
        { ModelType::S2Files, tags::s2files },   //
        { ModelType::Overlay, tags::overlay },   //
    };
    auto search = tagByModelType.find(m_type);
    if (search != tagByModelType.cend())
        return search->second;
    else
        return "undefined";
}

void XmlContainerModel::parseNode(QDomNode &node, int &row)
{
    // Для узлов <sections> и <section>
    if (m_type == ModelType::Sections || m_type == ModelType::Section)
    {
        int count = 0;
        parseAttribute(node, tags::header, row, count++);  // Заголовок
        if (m_type == ModelType::Section)                  //
            parseAttribute(node, tags::tab, row, count++); // ID вкладки
        parseAttribute(node, tags::order, row, count++);   // Приоритет внутри вкладки
    }
    // Для узлов <hidden>
    else if (m_type == ModelType::Hidden)
    {
        parseAttribute(node, tags::desc, row, 0);
        parseAttribute(node, tags::prefix, row, 1);
        parseAttribute(node, tags::flag, row, 2, "0");
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

void XmlContainerModel::create(const QStringList &saved, int *row)
{
    // Создание нового дочернего узла под <resources>: saved[0] - имя тега,
    // saved[1] - описание (может быть пустым).
    if (m_type == ModelType::Resources)
    {
        if (saved.isEmpty())
            return;
        const auto typeIt = XmlModel::s_types.find(saved.first());
        if (typeIt == XmlModel::s_types.cend())
            return;
        BaseEditorModel::create(saved, row);
        if (*row >= 0)
        {
            ChildModelNode node { nullptr, typeIt->second };
            node.m_model = ModelFabric::createEmptyChildModel(typeIt->second, this);
            if (node.m_model != nullptr)
                setData(index(*row, 0), QVariant::fromValue(node), ModelNodeRole);
        }
        emit modelChanged();
        return;
    }
    // Создание дочерних элементов доступно для узлов <sections>, <section>, <hidden>, <overlay>
    if (m_type == ModelType::Sections || m_type == ModelType::Section //
        || m_type == ModelType::Hidden || m_type == ModelType::Overlay)
    {
        BaseEditorModel::create(saved, row);
        if (*row >= 0)
        {
            ChildModelNode node { nullptr, ModelType::None };
            QStringList labels;
            if (m_type == ModelType::Sections)
            {
                node.m_type = ModelType::Section;
                labels = XmlModel::s_headers.find(node.m_type)->second;
                node.m_model = new XmlContainerModel(1, labels.count(), node.m_type, this);
            }
            else if (m_type == ModelType::Hidden)
            {
                node.m_type = ModelType::HiddenTab;
                labels = XmlModel::s_headers.find(node.m_type)->second;
                node.m_model = new XmlDataModel(1, labels.count(), node.m_type, this);
            }
            else if (m_type == ModelType::Overlay)
            {
                node.m_type = ModelType::OverlayRecords;
                labels = XmlModel::s_headers.find(node.m_type)->second;
                node.m_model = new XmlHideDataModel(1, labels.count(), node.m_type, this);
            }
            else
            {
                node.m_type = ModelType::SGroup;
                labels = XmlModel::s_headers.find(node.m_type)->second;
                node.m_model = new XmlHideDataModel(1, labels.count(), node.m_type, this);
            }
            node.m_model->setHorizontalHeaderLabels(labels);
            auto itemIndex = node.m_model->index(0, 0);
            node.m_model->setData(itemIndex, QString(".."));
            setData(index(*row, 0), QVariant::fromValue(node), ModelNodeRole);
        }
        emit modelChanged();
    }
}

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
            if (child.m_model != nullptr)
            {
                // Дочернюю модель в ноду
                auto childNode = child.m_model->toNode(doc);
                // Для узлов <sections> и <section>
                if (m_type == ModelType::Sections || m_type == ModelType::Section)
                {
                    int colCount = 0;
                    // Добавляем описание (атрибут header)
                    setAttribute(doc, childNode, tags::header, data(index(row, colCount++)));
                    // Добавляем номер вкладки (атрибут tab) и приоритет (атрибут order)
                    if ((m_type == ModelType::Section) && (columnCount() > colCount))
                    {
                        setAttribute(doc, childNode, tags::tab, data(index(row, colCount++)));
                    }
                    if (columnCount() > colCount)
                    {
                        auto order = data(index(row, colCount++)).value<QString>();
                        if ((!order.isEmpty()) && (order != "0"))
                            setAttribute(doc, childNode, tags::order, order);
                    }
                }
                // Для узлов <hidden>
                else if ((m_type == ModelType::Hidden) && (columnCount() > 2))
                {
                    setAttribute(doc, childNode, tags::desc, data(index(row, 0)));
                    setAttribute(doc, childNode, tags::prefix, data(index(row, 1)));
                    setAttribute(doc, childNode, tags::flag, data(index(row, 2)));
                }
                // Для узлов <resources>, <alarms> и <journals>
                else if (columnCount() > 1)
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

QStringList XmlContainerModel::getRowData(const int row)
{
    QStringList response;
    if (row >= 0 && row < rowCount())
    {
        if (m_type == ModelType::Resources)
        {
            auto desc = data(index(row, 1));
            if (desc.isValid() && desc.canConvert<QString>())
                response.append(desc.value<QString>());
        }
        else
            response = XmlModel::getRowData(row);
    }
    return response;
}

void XmlContainerModel::update(const QStringList &saved, const int row)
{
    if (row >= 0 && row < rowCount())
    {
        if (m_type == ModelType::Resources)
        {
            setData(index(row, 1), saved[0]);
            emit modelChanged();
        }
        else
            XmlModel::update(saved, row);
    }
}
