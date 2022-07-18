#include "modelfabric.h"

#include "xmlmodels.h"

int ElementsCount(QDomNode &node);

ModelFabric::ModelFabric()
{
}

void ModelFabric::CreateModel(ModelNode &mNode, QDomNode &root, QObject *parent)
{
    auto iter = XmlModel::headers.find(mNode.modelType);
    if (iter != XmlModel::headers.cend())
    {
        auto labels = iter->second;
        auto cols = labels.count(), rows = ElementsCount(root) + 1;
        switch (mNode.modelType)
        {
        case ModelType::Signals:
            mNode.modelPtr = new XmlSignalsModel(rows, cols, parent);
            break;
        case ModelType::SectionTabs:
            mNode.modelPtr = new XmlSectionTabsModel(rows, cols, parent);
            break;
        case ModelType::Sections:
            mNode.modelPtr = new XmlMainModel(rows, cols, parent);
            break;
        case ModelType::Section:
            mNode.modelPtr = new XmlMainModel(rows, cols, parent);
            break;
        case ModelType::SGroup:
            mNode.modelPtr = new XmlSectionGroupsModel(rows, cols, parent);
            break;
        case ModelType::Alarms:
            mNode.modelPtr = new XmlAlarmsModel(rows, cols, parent);
            break;
        case ModelType::CritAlarms:
            mNode.modelPtr = new XmlCritAlarmsModel(rows, cols, parent);
            break;
        case ModelType::WarnAlarms:
            mNode.modelPtr = new XmlWarnAlarmsModel(rows, cols, parent);
            break;
        case ModelType::InfoAlarms:
            mNode.modelPtr = new XmlWarnAlarmsModel(rows, cols, parent);
            break;
        case ModelType::Journals:
            mNode.modelPtr = new XmlJoursModel(rows, cols, parent);
            break;
        case ModelType::WorkJours:
            mNode.modelPtr = new XmlWorkJoursModel(rows, cols, parent);
            break;
        case ModelType::MeasJours:
            mNode.modelPtr = new XmlMeasJoursModel(rows, cols, parent);
            break;
        case ModelType::Modbus:
            mNode.modelPtr = new XmlModbusModel(rows, cols, parent);
            break;
        case ModelType::Protocom:
            mNode.modelPtr = new XmlProtocomModel(rows, cols, parent);
            break;
        case ModelType::IEC60870:
            mNode.modelPtr = new XmlIECModel(rows, cols, parent);
            break;
        case ModelType::Config:
            mNode.modelPtr = new XmlConfigModel(rows, cols, parent);
            break;
        default:
            break;
        }
        if (mNode.modelPtr != nullptr)
        {
            mNode.modelPtr->setHorizontalHeaderLabels(labels);
            mNode.modelPtr->setDataNode(true, root);
        }
    }
}

XmlModel *ModelFabric::CreateMainModel(QDomNode &root, QObject *parent)
{
    if (!root.isNull())
    {
        auto rootName = root.nodeName();
        if (rootName == "module")
        {
            auto res = root.firstChildElement("resources");
            if (!res.isNull())
            {
                auto type = ModelType::Resources;
                auto iter = XmlModel::headers.find(type);
                if (iter != XmlModel::headers.cend())
                {
                    auto labels = iter->second;
                    auto cols = labels.count(), rows = ElementsCount(res);
                    auto model = new XmlMainModel(rows, cols, parent);
                    model->setHorizontalHeaderLabels(labels);
                    model->setDataNode(false, res);
                    return model;
                }
            }
        }
        else if (rootName == "s2files")
        {
            // TODO: сделать особый обработчик для s2files ноды
        }
    }
    return nullptr;
}

int ElementsCount(QDomNode &node)
{
    auto childs = node.childNodes();
    int count = 0;
    for (auto i = 0; i < childs.count(); i++)
    {
        auto child = childs.item(i);
        if (!child.isComment() && child.isElement())
            count++;
    }
    return count;
}
