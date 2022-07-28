#ifndef MODELFABRIC_H
#define MODELFABRIC_H

#include "mastermodel.h"
#include "xmlmodel.h"

/// \brief Class with static members for creating XML models.
class ModelFabric
{
protected:
    static int ElementsCount(QDomNode &node);

public:
    explicit ModelFabric() = delete;
    static void CreateChildModel(ChildModelNode &mNode, QDomNode &root, QObject *parent = nullptr);
    static XmlModel *CreateRootModel(QDomNode &root, QObject *parent = nullptr);
    static MasterModel *CreateMasterModel(QObject *parent = nullptr);
};

#endif // MODELFABRIC_H
