#ifndef MASTERMODEL_H
#define MASTERMODEL_H

#include "modelmanager.h"

#include <QStandardItemModel>
#include <QtXml>

class MasterModel : public QStandardItemModel
{
private:
    ModelManager *manager;
    QTableView *slaveView;

    void readModulesToModel();
    void parseXmlNode(const QDomNode &node, const QString &filename, int &index);

public:
    MasterModel(QObject *parent = nullptr);
    void setHorizontalHeaderLabels(const QStringList &labels);
    void setSlaveView(QTableView *sView);

public slots:
    void masterItemSelected(const QModelIndex &index);
};

#endif // MASTERMODEL_H
