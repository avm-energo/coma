#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "modelfabric.h"

#include <QSortFilterProxyModel>
#include <QTableView>
#include <stack>

typedef QSortFilterProxyModel XmlSortProxyModel;

class ModelManager : public QObject
{
    Q_OBJECT
private:
    XmlSortProxyModel *curModel;
    // QTableView *tableView;
    std::stack<XmlSortProxyModel *> storage;
    QString curPath;

    void ClearStorage();
    void ChangeModel(XmlModel *model);

public:
    explicit ModelManager(QObject *parent = nullptr);

public slots:
    void ViewModelItemClicked(const QModelIndex &index);
    void SetDocument(QDomNode &doc);

signals:
    void ModelChanged(XmlSortProxyModel *model);
    void PathChanged(const QString &path);
};

#endif // MODELMANAGER_H
