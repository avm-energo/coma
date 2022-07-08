#ifndef MODELFABRIC_H
#define MODELFABRIC_H

#include "xmlcheckmodel.h"
#include "xmlmainmodel.h"

class ModelFabric
{
public:
    explicit ModelFabric();

    static void CreateModel(ModelNode &mNode, QDomNode &root, QObject *parent)
    {
        auto iter = XmlModel::settings.find(mNode.modelType);
        if (iter != XmlModel::settings.cend())
        {
            auto labels = iter->second;
            auto cols = labels.count(), rows = root.childNodes().count();
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
                mNode.modelPtr->setDataNode(mNode.modelType, root);
            }
        }
    }
};

#endif // MODELFABRIC_H
