#include "modelmanager.h"

#include "modelfabric.h"

/// \brief Main ctor for model manager.
ModelManager::ModelManager(QObject *parent) : QObject(parent), root(nullptr), curModel(nullptr), curPath("")
{
}

/*! \brief Clears stack with models and label with current path.
 *  \see SetDocument
 */
void ModelManager::ClearStorage()
{
    while (!storage.empty())
    {
        curModel = storage.top();
        storage.pop();
    }
    curModel->deleteLater();
    curModel = nullptr;
    curPath = "";
    emit PathChanged(curPath);
}

/// \brief Replaces the current XML model by given model.
void ModelManager::ChangeModel(XmlModel *model)
{
    curModel = model;
    emit ModelChanged(curModel);
}

/*! \brief Slot that is called when user selects a file in the master model.
 *  \details Creates root XML model at base of given XML document
 *  and sets it as current model in table view.
 *  \see ClearStorage, ChangeModel
 */
void ModelManager::SetDocument(QDomNode &doc)
{
    // Если уже есть модель
    if (curModel != nullptr)
    {
        ClearStorage();
        emit SaveModule();
    }
    root = ModelFabric::CreateRootModel(doc, this);
    ChangeModel(root);
}

/// \brief Returns current XML root model.
XmlModel *ModelManager::GetRootModel() const
{
    return root;
}

/*! \brief Slot that is called when user selects item in current XML model.
 *  \details This function has 3 behaviours:
 *  - selected item in the current model contains child XML model:
 *  calling ChangeModel with child model as  a function argument;
 *  - selected item in the current model is a string ".." for go back:
 *  returns previous model from stack and call ChangeModel with this model as a function argument;
 *  - selected item in the current model is usual item: emits EditQuery signal.
 * \see ChangeModel, PathChanged, EditQuery
 */
void ModelManager::ViewModelItemClicked(const QModelIndex &index)
{
    auto pureIndex = curModel->index(index.row(), 0);
    auto data = curModel->data(pureIndex, ModelNodeRole);
    // Если кликнули по item-у, который содержит подмодель
    if (data.isValid() && data.canConvert<ChildModelNode>())
    {
        auto modelNode = data.value<ChildModelNode>();
        if (modelNode.modelPtr != nullptr && modelNode.modelType != ModelType::None)
        {
            auto name = curModel->data(pureIndex).value<QString>();
            curPath += "\\" + name;
            storage.push(curModel);
            ChangeModel(modelNode.modelPtr);
            emit PathChanged(curPath);
        }
    }
    else
    {
        data = curModel->data(pureIndex);
        if (data.isValid() && data.canConvert<QString>())
        {
            auto str = data.value<QString>();
            // Если пользователь хочет вернуться назад
            if (str == "..")
            {
                curPath = curPath.left(curPath.lastIndexOf('\\'));
                curModel = storage.top();
                storage.pop();
                emit ModelChanged(curModel);
                emit PathChanged(curPath);
            }
            else
                emit EditQuery();
        }
    }
}
