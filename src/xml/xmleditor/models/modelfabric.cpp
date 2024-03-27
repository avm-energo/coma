#include "modelfabric.h"

#include "xmlmodels.h"

/*! \brief Creates child XML model.
 *  \details Creates child XML model for given parent XML model at base of given XML DOM node.
 *  After creating new model stores in given ChildModelNode structure.
 *  \param mNode [out] - structure that stores pointer to created child XML model.
 *  \param root [in] - XML DOM node for calling XmlModel::setDataNode function.
 *  \param parent [in] - pointer to parent XML model.
 */
void ModelFabric::CreateChildModel(ChildModelNode &mNode, QDomNode &root, QObject *parent)
{
    auto iter = XmlModel::headers.find(mNode.modelType);
    if (iter != XmlModel::headers.cend())
    {
        auto labels = iter->second;
        int cols = labels.count(), rows = ElementsCount(root) + 1;
        switch (mNode.modelType)
        {
        case ModelType::SGroup:
            mNode.modelPtr = new XmlHideDataModel(rows, cols, mNode.modelType, parent);
            break;
        case ModelType::Alarms:
        case ModelType::Sections:
        case ModelType::Section:
        case ModelType::Journals:
        case ModelType::Hidden:
            mNode.modelPtr = new XmlContainerModel(rows, cols, mNode.modelType, parent);
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
            mNode.modelPtr = new XmlDataModel(rows, cols, mNode.modelType, parent);
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

/// \brief Creates the root XML model at base of given XML DOM node.
XmlModel *ModelFabric::CreateRootModel(QDomNode &root, QObject *parent)
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
                    int cols = labels.count(), rows = ElementsCount(res);
                    auto model = new XmlContainerModel(rows, cols, type, parent);
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

/// \brief Creates the master XML model.
MasterModel *ModelFabric::CreateMasterModel(QObject *parent)
{
    auto masterModel = new MasterModel(parent);
    masterModel->setHorizontalHeaderLabels({ "Устройство", "Type B", "Type M", "Версия", "Файл" });
    return masterModel;
}

/// \brief Returns count of child nodes for given XML DOM node,
/// that are not comments and are DOM elements.
int ModelFabric::ElementsCount(QDomNode &node)
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
