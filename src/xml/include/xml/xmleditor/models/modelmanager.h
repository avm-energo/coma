#pragma once

#include <stack>
#include <xml/xmleditor/models/xmlmodel.h>

/// \brief Class for XML models managing.
/// \details This class contains members for items selection handling
/// and inner structure for managing displayed XML model in table view.
class ModelManager final : public QObject
{
    Q_OBJECT
private:
    XmlModel *m_root, *m_currentModel;
    std::stack<XmlModel *> m_storage;
    QString m_currentPath;

protected:
    /// \brief Clears stack with models and label with current path.
    /// \see SetDocument
    void clearStorage();
    /// \brief Replaces the current XML model by given model.
    void updateModel(XmlModel *model);

public:
    /// \brief Main ctor for model manager.
    explicit ModelManager(QObject *parent = nullptr);
    /// \brief Returns current XML root model.
    XmlModel *getRootModel() const;

public slots:
    /// \brief Slot that is called when user selects item in current XML model.
    /// \details This function has 3 behaviours:
    /// - selected item in the current model contains child XML model:
    /// calling ChangeModel with child model as  a function argument;
    /// - selected item in the current model is a string ".." for go back:
    /// returns previous model from stack and call ChangeModel with this model as a function argument;
    /// - selected item in the current model is usual item: emits EditQuery signal.
    /// \see ChangeModel, PathChanged, EditQuery
    void viewModelItemClicked(const QModelIndex &index);

    /// \brief Slot that is called when user selects a file in the master model.
    /// \details Creates root XML model at base of given XML document
    /// and sets it as current model in table view.
    /// \see ClearStorage, ChangeModel
    void setDocument(QDomNode &doc);

signals:
    /// \details This signal is emitted when current XML model is changed.
    void modelChanged(XmlModel *model);
    /// \details This signal is emitted when current path in XML model is changed.
    void pathChanged(const QString &path);
    /// \details This signal is emitted when user double-clicked at item in current XML model.
    void editQuery();
    /// \details This signal is emitted when user try to open a new file, but the current file is changed.
    void saveModule();
};
