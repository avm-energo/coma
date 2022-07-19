#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "modelfabric.h"

#include <QTableView>
#include <stack>

class ModelManager : public QObject
{
    Q_OBJECT
private:
    XmlModel *curModel;
    // QTableView *tableView;
    std::stack<XmlModel *> storage;
    QString curPath;

    void ClearStorage();

public:
    explicit ModelManager(QObject *parent = nullptr);

public slots:
    void ViewModelItemClicked(const QModelIndex &index);
    void SetDocument(QDomNode &doc);

signals:
    void ModelChanged(XmlModel *model);
    void PathChanged(const QString &path);
};

#endif // MODELMANAGER_H
