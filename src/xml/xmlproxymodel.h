#ifndef XMLPROXYMODEL_H
#define XMLPROXYMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <QtXml>

class XmlProxyModel : QObject
{
private:
    QStandardItemModel *proxyModel;

    void ParseXmlToProxyModel(QDomNode &node, int index, QStandardItem *parent);
    int ParseXmlFindAllAttributes(QDomNode &node, QStandardItem *element);

public:
    XmlProxyModel(QObject *parent = nullptr);
    void ParseDocument(QDomDocument &doc);
    QStandardItem *GetRootOfModel();
    bool HasAttributes(QStandardItem *node);
    int CountAttributes(QStandardItem *node);
};

#endif // XMLPROXYMODEL_H
