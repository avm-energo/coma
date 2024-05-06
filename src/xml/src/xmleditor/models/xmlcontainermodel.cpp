#include "xml/xmleditor/models/xmlcontainermodel.h"

#include <xml/xmleditor/models/xmldatamodel.h>
#include <xml/xmleditor/models/xmlhidedatamodel.h>
#include <xml/xmltags.h>

XmlContainerModel::XmlContainerModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
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
        parseAttribute(node, tags::header, row, 0);  // Заголовок
        if (m_type == ModelType::Section)            //
            parseAttribute(node, tags::tab, row, 1); // ID вкладки
    }
    // Для узлов <hidden>
    else if (m_type == ModelType::Hidden)
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

void XmlContainerModel::create(const QStringList &saved, int *row)
{
    // Создание дочерних элементов доступно для узлов <sections> и <section>
    if (m_type == ModelType::Sections || m_type == ModelType::Section || m_type == ModelType::Hidden)
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
                // Так как узел <sections> содержит узлы <section>
                node.m_model = new XmlContainerModel(1, labels.count(), node.m_type, this);
            }
            else if (m_type == ModelType::Hidden)
            {
                node.m_type = ModelType::HiddenTab;
                labels = XmlModel::s_headers.find(node.m_type)->second;
                node.m_model = new XmlDataModel(1, labels.count(), node.m_type, this);
            }
            else
            {
                node.m_type = ModelType::SGroup;
                labels = XmlModel::s_headers.find(node.m_type)->second;
                // Узел <section> содержит узлы <sgroup>
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
                    // Добавляем описание (атрибут header)
                    setAttribute(doc, childNode, tags::header, data(index(row, 0)));
                    // Добавляем номер вкладки (атрибут tab)
                    if (m_type == ModelType::Section)
                        setAttribute(doc, childNode, tags::tab, data(index(row, 1)));
                }
                // Для узлов <hidden>
                else if (m_type == ModelType::Hidden)
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
