#ifndef MODELFABRIC_H
#define MODELFABRIC_H

#include "xmlmodel.h"

class ModelFabric
{
public:
    explicit ModelFabric();

    static void CreateModel(ModelNode &mNode, QDomNode &root, QObject *parent = nullptr);
    static XmlModel *CreateMainModel(QDomNode &root, QObject *parent = nullptr);
};

#endif // MODELFABRIC_H
