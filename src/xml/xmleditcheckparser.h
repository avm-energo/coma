#ifndef XMLEDITCHECKPARSER_H
#define XMLEDITCHECKPARSER_H

#include <QObject>
#include <QStandardItemModel>
#include <QtXml>
#include <utility>

class XmlEditCheckParser : public QObject
{
    Q_OBJECT
private:
    QStandardItemModel *model;

    std::pair<QDomNode, QStandardItem *> ParseXmlAddCheckSubgroups(
        QDomNode &checkNode, int &index, QStandardItem *parent, QString &groupName);
    void ParseXmlCheckChildrens(QDomNode &checkNode, int &index, QStandardItem *parent, QString groupName);
    void ParseXmlCheckGroup(QDomNode &groupNode, int &index, QStandardItem *parent);
    void ParseXmlCheckSignal(QDomNode &signalNode, int &index, QStandardItem *parent);
    void ParseXmlCheckRecord(QDomNode &recordNode, int &index, QStandardItem *parent);

public:
    explicit XmlEditCheckParser(QStandardItemModel *input_model, QObject *parent = nullptr);
    void ParseXmlCheck(QDomNode &checkNode, int &index, QStandardItem *parent);
};

#endif // XMLEDITCHECKPARSER_H
