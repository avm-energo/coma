#pragma once

#include <xml/xmleditor/models/mastermodel.h>
#include <xml/xmleditor/models/xmlmodel.h>

/// \brief Class with static members for creating XML models.
class ModelFabric
{
protected:
    /// \brief Returns count of child nodes for given XML DOM node,
    /// that are not comments and are DOM elements.
    static int elementsCount(QDomNode &node);

public:
    ModelFabric() = delete;

    /// \brief Creates child XML model.
    /// \details Creates child XML model for given parent XML model at base of given XML DOM node.
    /// After creating new model stores in given ChildModelNode structure.
    /// \param mNode [out] - structure that stores pointer to created child XML model.
    /// \param root [in] - XML DOM node for calling XmlModel::setDataNode function.
    /// \param parent [in] - pointer to parent XML model.
    static void createChildModel(ChildModelNode &mNode, QDomNode &root, QObject *parent = nullptr);

    /// \brief Creates the root XML model at base of given XML DOM node.
    static XmlModel *createRootModel(QDomNode &root, QObject *parent = nullptr);
    /// \brief Creates the master XML model.
    static MasterModel *createMasterModel(QObject *parent = nullptr);
};
