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
    QStringList getNewList(const QStringList &saved);

public:
    MasterModel(QObject *parent = nullptr);
    virtual QDomDocument *toNode(const int row, QDomElement *elem);

public slots:
    void masterItemSelected(const QModelIndex &index);
    virtual void create(const QStringList &saved, int *row) override;
    virtual void update(const QStringList &saved, const int &row) override;
    virtual void remove(const int &row) override;

signals:
    void itemSelected(QDomNode &node);
    void createFile(const QStringList &creationData);
    void renameFile(const QString &oldName, const QString &newName);
    void removeFile(const QString &filename);
};

#endif // MASTERMODEL_H
