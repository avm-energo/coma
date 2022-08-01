#ifndef MASTERMODEL_H
#define MASTERMODEL_H

#include <QStandardItemModel>
#include <QtXml>

class MasterModel : public QStandardItemModel
{
    Q_OBJECT
private:
    void readModulesToModel();
    void parseXmlNode(const QDomNode &node, const QString &filename, int &row);

public:
    MasterModel(QObject *parent = nullptr);
    void setHorizontalHeaderLabels(const QStringList &labels);

public slots:
    void masterItemSelected(const QModelIndex &index);
    void getDialogRequest(const int &row);
    void create(const QStringList &saved, int *row);
    void update(const QStringList &saved, const int &row);
    void remove(const int &row);

signals:
    void itemSelected(QDomNode &node);
    void sendDialogResponse(const QStringList &response);
};

#endif // MASTERMODEL_H
