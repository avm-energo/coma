#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "modelfabric.h"

#include <QObject>
#include <QTableView>
#include <stack>

class ModelManager : public QObject
{
    Q_OBJECT
private:
    XmlModel *curModel;
    QTableView *tableView;
    std::stack<XmlModel *> storage;

    void ClearStorage();

public:
    explicit ModelManager(QTableView *view, QObject *parent = nullptr);
    void SetDocument(QDomNode &doc);

public slots:
    void ViewModelItemClicked(const QModelIndex &index);
};

#endif // MODELMANAGER_H
