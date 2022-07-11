#include "modelfabric.h"

#include "xmlcheckmodel.h"
#include "xmlmainmodel.h"

ModelFabric::ModelFabric()
{
}

void ModelFabric::CreateModel(ModelNode &mNode, QDomNode &root, QObject *parent)
{
    auto iter = XmlModel::settings.find(mNode.modelType);
    if (iter != XmlModel::settings.cend())
    {
        auto labels = iter->second;
        auto cols = labels.count(), rows = root.childNodes().count() + 1;
        switch (mNode.modelType)
        {
        case GroupTypes::Check:
            mNode.modelPtr = new XmlCheckModel(rows, cols, parent);
            break;
        case GroupTypes::Groups:
            mNode.modelPtr = new XmlCheckGroupsModel(rows, cols, parent);
            break;
        case GroupTypes::Records:
            mNode.modelPtr = new XmlCheckRecordsModel(rows, cols, parent);
            break;
        case GroupTypes::Signals:
            mNode.modelPtr = new XmlCheckSignalsModel(rows, cols, parent);
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
        if (rootName == "modules")
        {
            auto module = root.firstChildElement("module");
            if (!module.isNull())
            {
                auto res = module.firstChildElement("resources");
                if (!res.isNull())
                {
                    auto type = GroupTypes::Resources;
                    auto iter = XmlModel::settings.find(type);
                    if (iter != XmlModel::settings.cend())
                    {
                        auto labels = iter->second;
                        auto cols = labels.count(), rows = res.childNodes().count();
                        auto model = new XmlMainModel(rows, cols, parent);
                        model->setHorizontalHeaderLabels(labels);
                        model->setDataNode(false, res);
                        return model;
                    }
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
