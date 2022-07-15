#include "modelmanager.h"

ModelManager::ModelManager(QTableView *view, QObject *parent) : QObject(parent), curModel(nullptr), tableView(view)
{
    QObject::connect(tableView, &QTableView::clicked, this, &ModelManager::ViewModelItemClicked);
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
}

void ModelManager::SetDocument(QDomNode &doc)
{
    // Если уже есть модель
    if (curModel != nullptr)
        ClearStorage();

    curModel = ModelFabric::CreateMainModel(doc, this);
    tableView->setModel(curModel);
}

void ModelManager::ViewModelItemClicked(const QModelIndex &index)
{
    auto data = curModel->data(index, ModelNodeRole);
    if (data.isValid() && data.canConvert<ModelNode>())
    {
        auto modelNode = data.value<ModelNode>();
        if (modelNode.modelPtr != nullptr && modelNode.modelType != ModelType::None)
        {
            storage.push(curModel);
            curModel = modelNode.modelPtr;
            tableView->setModel(curModel);
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
                curModel = storage.top();
                storage.pop();
                tableView->setModel(curModel);
            }
        }
    }
}
