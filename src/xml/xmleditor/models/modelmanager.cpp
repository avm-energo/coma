#include "modelmanager.h"

ModelManager::ModelManager(QObject *parent) : QObject(parent), curModel(nullptr), curPath("")
{
}

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

void ModelManager::SetDocument(QDomNode &doc)
{
    // Если уже есть модель
    if (curModel != nullptr)
        ClearStorage();

    curModel = ModelFabric::CreateMainModel(doc, this);
    emit ModelChanged(curModel);
}

void ModelManager::ViewModelItemClicked(const QModelIndex &index)
{
    auto data = curModel->data(index, ModelNodeRole);
    if (data.isValid() && data.canConvert<ModelNode>())
    {
        auto modelNode = data.value<ModelNode>();
        if (modelNode.modelPtr != nullptr && modelNode.modelType != ModelType::None)
        {
            auto nameIndex = curModel->index(index.row(), 0);
            auto name = curModel->data(nameIndex).value<QString>();
            curPath += "\\" + name;
            storage.push(curModel);
            curModel = modelNode.modelPtr;
            emit ModelChanged(curModel);
            emit PathChanged(curPath);
        }
    }
    else
    {
        data = curModel->data(curModel->index(index.row(), 0));
        if (data.isValid() && data.canConvert<QString>())
        {
            auto str = data.value<QString>();
            if (str == "..")
            {
                curPath = curPath.left(curPath.lastIndexOf('\\'));
                curModel = storage.top();
                storage.pop();
                emit ModelChanged(curModel);
                emit PathChanged(curPath);
            }
        }
    }
}
