#ifndef MASTERMODEL_H
#define MASTERMODEL_H

#include <QStandardItemModel>
#include <QtXml>

class MasterModel : public QStandardItemModel
{
    Q_OBJECT
private:
    void readModulesToModel();
    void parseXmlNode(const QDomNode &node, const QString &filename, int &index);

public:
    MasterModel(QObject *parent = nullptr);
    void setHorizontalHeaderLabels(const QStringList &labels);

public slots:
    void masterItemSelected(const QModelIndex &index);

signals:
    void itemSelected(QDomNode &node);
};

#endif // MASTERMODEL_H
