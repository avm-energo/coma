#ifndef XMLPROXYMODEL_H
#define XMLPROXYMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <QtXml>
#include <map>

constexpr int GroupTypeRole = 0x0105;

enum GroupTypes : quint16
{
    Resources = 0,
    Check,
    Groups,
    Signals,
    Records
};

/*!
 * \brief The XmlProxyModel class.
 * \details That class used for creating proxy item model and storing all Dom Nodes of XML file.
 */
class XmlProxyModel : QObject
{
private:
    QStandardItemModel *proxyModel;
    ///< Types Map with enumeration, key = name of group type
    static const std::map<QString, GroupTypes> types;
    ///< Settings Map, key = group type enumeration
    static const std::map<GroupTypes, QStringList> settings;

    /// \brief Main parser algorithm.
    void ParseXmlToProxyModel(QDomNode &node, int index, QStandardItem *parent);

    /// \brief Parser for finding attributes in nodes.
    int ParseXmlFindAllAttributes(QDomNode &node, QStandardItem *element);

    /// \brief TODO
    void ItemAdjuster(QStandardItem *item);

public:
    /// \brief Default constructor
    XmlProxyModel(QObject *parent = nullptr);

    /// \brief Public member function for parsing QDomDocument object and creating proxy item model.
    void ParseDocument(QDomDocument &doc);

    /// \brief Returns root item of proxy model.
    QStandardItem *GetRootOfModel();

    /// \brief Static member function, returns true if item has
    /// attributes in child items, otherwise returns false.
    static bool HasAttributes(QStandardItem *node);

    /// \brief Static member function, returns count of attributes in node.
    static int CountAttributes(QStandardItem *node);
};

#endif // XMLPROXYMODEL_H
