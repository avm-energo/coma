#ifndef MASTERMODEL_H
#define MASTERMODEL_H

#include "ieditormodel.h"

#include <QtXml>

class MasterModel : public IEditorModel
{
    Q_OBJECT
private:
    void readModulesToModel();
    void parseXmlNode(const QDomNode &node, const QString &filename, int &row);

public:
    MasterModel(QObject *parent = nullptr);

public slots:
    void masterItemSelected(const QModelIndex &index);
    virtual void create(const QStringList &saved, int *row) override;
    virtual void update(const QStringList &saved, const int &row) override;
    virtual void remove(const int &row) override;

signals:
    void itemSelected(QDomNode &node);
};

#endif // MASTERMODEL_H
