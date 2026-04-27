#include "xml/xmleditor/models/modelfabric.h"

#include <xml/xmleditor/models/xmlcontainermodel.h>
#include <xml/xmleditor/models/xmldatamodel.h>
#include <xml/xmleditor/models/xmlhidedatamodel.h>
#include <xml/xmltags.h>

void ModelFabric::createChildModel(ChildModelNode &mNode, QDomNode &root, QObject *parent)
{
    auto iter = XmlModel::s_headers.find(mNode.m_type);
    if (iter != XmlModel::s_headers.cend())
    {
        auto labels = iter->second;
        int cols = labels.count(), rows = elementsCount(root) + 1;
        switch (mNode.m_type)
        {
        case ModelType::SGroup:
        case ModelType::S2Records:
        case ModelType::Overlay:
            mNode.m_model = new XmlHideDataModel(rows, cols, mNode.m_type, parent);
            break;
        case ModelType::Alarms:
        case ModelType::Sections:
        case ModelType::Section:
        case ModelType::Journals:
        case ModelType::Hidden:
            mNode.m_model = new XmlContainerModel(rows, cols, mNode.m_type, parent);
            break;
        case ModelType::Signals:
        case ModelType::SectionTabs:
        case ModelType::AlarmStateAll:
        case ModelType::AlarmsCrit:
        case ModelType::AlarmsWarn:
        case ModelType::AlarmsInfo:
        case ModelType::WorkJours:
        case ModelType::MeasJours:
        case ModelType::Modbus:
        case ModelType::Protocom:
        case ModelType::IEC60870:
        case ModelType::Config:
        case ModelType::HiddenTab:
        case ModelType::BsiExt:
        case ModelType::S2Tabs:
        case ModelType::Includes:
            mNode.m_model = new XmlDataModel(rows, cols, mNode.m_type, parent);
            break;
        default:
            break;
        }
        if (mNode.m_model != nullptr)
        {
            mNode.m_model->setHorizontalHeaderLabels(labels);
            mNode.m_model->setDataNode(true, root);
        }
    }
}

XmlModel *ModelFabric::createRootModel(QDomNode &root, QObject *parent)
{
    if (!root.isNull())
    {
        ModelType type = ModelType::None;
        auto rootName = root.nodeName();
        if (rootName == tags::module)
        {
            auto res = root.firstChildElement(tags::res);
            if (!res.isNull())
            {
                type = ModelType::Resources;
                root = res;
            }
        }
        else if (rootName == tags::s2files)
        {
            type = ModelType::S2Files;
        }
        auto iter = XmlModel::s_headers.find(type);
        if (iter != XmlModel::s_headers.cend())
        {
            auto labels = iter->second;
            int cols = labels.count(), rows = elementsCount(root);
            auto model = new XmlContainerModel(rows, cols, type, parent);
            model->setHorizontalHeaderLabels(labels);
            model->setDataNode(false, root);
            return model;
        }
    }
    return nullptr;
}

MasterModel *ModelFabric::createMasterModel(QObject *parent)
{
    auto masterModel = new MasterModel(parent);
    masterModel->setHorizontalHeaderLabels({ "Устройство", "Type B", "Type M", "Версия", "Файл" });
    return masterModel;
}

XmlModel *ModelFabric::createEmptyChildModel(ModelType type, QObject *parent)
{
    auto iter = XmlModel::s_headers.find(type);
    if (iter == XmlModel::s_headers.cend())
        return nullptr;
    const auto labels = iter->second;
    const int cols = labels.count();
    XmlModel *model = nullptr;
    switch (type)
    {
    case ModelType::SGroup:
    case ModelType::S2Records:
        model = new XmlHideDataModel(1, cols, type, parent);
        break;
    case ModelType::Alarms:
    case ModelType::Sections:
    case ModelType::Section:
    case ModelType::Journals:
    case ModelType::Hidden:
        model = new XmlContainerModel(1, cols, type, parent);
        break;
    case ModelType::Signals:
    case ModelType::SectionTabs:
    case ModelType::AlarmStateAll:
    case ModelType::AlarmsCrit:
    case ModelType::AlarmsWarn:
    case ModelType::AlarmsInfo:
    case ModelType::WorkJours:
    case ModelType::MeasJours:
    case ModelType::Modbus:
    case ModelType::Protocom:
    case ModelType::IEC60870:
    case ModelType::Config:
    case ModelType::HiddenTab:
    case ModelType::BsiExt:
    case ModelType::S2Tabs:
    case ModelType::Includes:
        model = new XmlDataModel(1, cols, type, parent);
        break;
    default:
        return nullptr;
    }
    model->setHorizontalHeaderLabels(labels);
    model->setData(model->index(0, 0), QString(".."));
    return model;
}

int ModelFabric::elementsCount(QDomNode &node)
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
