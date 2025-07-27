#include "xml/xmleditor/models/modelmanager.h"

#include <xml/xmleditor/models/modelfabric.h>

ModelManager::ModelManager(QObject *parent)
    : QObject(parent), m_root(nullptr), m_currentModel(nullptr), m_currentPath("")
{
}

void ModelManager::clearStorage()
{
    while (!m_storage.empty())
    {
        m_currentModel = m_storage.top();
        m_storage.pop();
    }
    m_currentModel->deleteLater();
    m_currentModel = nullptr;
    m_currentPath = "";
    emit pathChanged(m_currentPath);
}

void ModelManager::updateModel(XmlModel *model)
{
    m_currentModel = model;
    emit modelChanged(m_currentModel);
}

void ModelManager::setDocument(QDomNode &doc)
{
    // Если уже есть модель
    if (m_currentModel != nullptr)
    {
        clearStorage();
        emit saveModule();
    }
    m_root = ModelFabric::createRootModel(doc, this);
    updateModel(m_root);
}

XmlModel *ModelManager::getRootModel() const
{
    return m_root;
}

void ModelManager::viewModelItemClicked(const QModelIndex &index)
{
    auto pureIndex = m_currentModel->index(index.row(), 0);
    auto data = m_currentModel->data(pureIndex, ModelNodeRole);
    // Если кликнули по item-у, который содержит подмодель
    if (data.isValid() && data.canConvert<ChildModelNode>())
    {
        auto modelNode = data.value<ChildModelNode>();
        if (modelNode.m_model != nullptr && modelNode.m_type != ModelType::None)
        {
            auto name = m_currentModel->data(pureIndex).value<QString>();
            m_currentPath += "\\" + name;
            m_storage.push(m_currentModel);
            updateModel(modelNode.m_model);
            emit pathChanged(m_currentPath);
        }
    }
    else
    {
        data = m_currentModel->data(pureIndex);
        if (data.isValid() && data.canConvert<QString>())
        {
            auto str = data.value<QString>();
            // Если пользователь хочет вернуться назад
            if (str == "..")
            {
                m_currentPath = m_currentPath.left(m_currentPath.lastIndexOf('\\'));
                m_currentModel = m_storage.top();
                m_storage.pop();
                emit modelChanged(m_currentModel);
                emit pathChanged(m_currentPath);
            }
            else
                emit editQuery();
        }
    }
}
