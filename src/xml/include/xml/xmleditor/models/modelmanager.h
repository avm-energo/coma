#pragma once

#include <stack>
#include <xml/xmleditor/models/xmlmodel.h>

/// \brief Class for XML models managing.
/// \details This class contains members for items selection handling
/// and inner structure for managing displayed XML model in table view.
class ModelManager : public QObject
{
    Q_OBJECT
private:
    XmlModel *root, *curModel;
    std::stack<XmlModel *> storage;
    QString curPath;

protected:
    void ClearStorage();
    void ChangeModel(XmlModel *model);

public:
    explicit ModelManager() = delete;
    explicit ModelManager(QObject *parent = nullptr);
    XmlModel *GetRootModel() const;

public slots:
    void ViewModelItemClicked(const QModelIndex &index);
    void SetDocument(QDomNode &doc);

signals:
    /// \details This signal is emitted when current XML model is changed.
    void ModelChanged(XmlModel *model);
    /// \details This signal is emitted when current path in XML model is changed.
    void PathChanged(const QString &path);
    /// \details This signal is emitted when user double-clicked at item in current XML model.
    void EditQuery();
    /// \details This signal is emitted when user try to open a new file, but the current file is changed.
    void SaveModule();
};
