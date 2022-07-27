#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "xmlsortproxymodel.h"

#include <QTableView>
#include <stack>

/// \brief Class for XML models managing.
/// \details This class contains members for items selection handling
/// and inner structure for managing displayed XML model in table view.
class ModelManager : public QObject
{
    Q_OBJECT
private:
    XmlSortProxyModel *curModel;
    std::stack<XmlSortProxyModel *> storage;
    QString curPath;

protected:
    void ClearStorage();
    void ChangeModel(XmlModel *model);

public:
    explicit ModelManager() = delete;
    explicit ModelManager(QObject *parent = nullptr);

public slots:
    void ViewModelItemClicked(const QModelIndex &index);
    void SetDocument(QDomNode &doc);

signals:
    /// \details This signal is emitted when current XML model is changed.
    void ModelChanged(XmlSortProxyModel *model);
    /// \details This signal is emitted when current path in XML model is changed.
    void PathChanged(const QString &path);
    /// \details This signal is emitted when user double-clicked at item in current XML model.
    void EditQuery();
};

#endif // MODELMANAGER_H
