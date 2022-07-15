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
        case ModelType::Check:
            mNode.modelPtr = new XmlCheckModel(rows, cols, parent);
            break;
        case ModelType::Groups:
            mNode.modelPtr = new XmlCheckGroupsModel(rows, cols, parent);
            break;
        // case GroupTypes::Records:
        //    mNode.modelPtr = new XmlCheckRecordsModel(rows, cols, parent);
        //    break;
        case ModelType::Signals:
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
