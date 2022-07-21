#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "modelfabric.h"
#include "xmlsortproxymodel.h"

#include <QTableView>
#include <stack>

class ModelManager : public QObject
{
    Q_OBJECT
private:
    XmlSortProxyModel *curModel;
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
