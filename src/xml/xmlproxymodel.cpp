#include "xmlproxymodel.h"

XmlProxyModel::XmlProxyModel(QObject *parent) : QObject(parent)
{
}

void XmlProxyModel::ParseXmlToProxyModel(QDomNode &node, int index, QStandardItem *parent)
{
    while (!node.isNull())
    {
        QStandardItem *element = nullptr;
        auto newIndex = 0;
        if (node.isElement() && !node.isComment())
        {
            auto name = node.nodeName();
            element = new QStandardItem(name);
            if (parent == nullptr)
            {
                parent = element;
                proxyModel->setItem(0, parent);
            }
            else
            {
                parent->setChild(index, element);
                index++;
            }
            newIndex = ParseXmlFindAllAttributes(node, element);
        }
        else if (node.isText())
        {
            auto text = node.toText();
            if (!text.isNull())
            {
                if (parent != nullptr)
                {
                    element = new QStandardItem(text.data());
                    parent->setChild(0, element);
                }
            }
            break;
        }
        auto cnode = node.firstChild();
        ParseXmlToProxyModel(cnode, newIndex, element);
        node = node.nextSibling();
    }
}

int XmlProxyModel::ParseXmlFindAllAttributes(QDomNode &node, QStandardItem *element)
{
    // Поиск аттрибутов
    auto domElement = node.toElement();
    if (!domElement.isNull())
    {
        auto attrs = domElement.attributes();
        if (attrs.count() > 0)
        {
            auto attributes = new QStandardItem("attributes");
            element->setChild(0, attributes);
            for (int i = 0; i < attrs.count(); i++)
            {
                auto attr = attrs.item(i).toAttr();
                auto aName = new QStandardItem(attr.name());
                attributes->setChild(i, aName);
                auto aValue = new QStandardItem(attr.value());
                aName->setChild(0, aValue);
            }
            return 1;
        }
    }
    return 0;
}

void XmlProxyModel::ParseDocument(QDomDocument &doc)
{
    proxyModel = new QStandardItemModel(this);
    proxyModel->setColumnCount(1);
    ParseXmlToProxyModel(doc, 0, nullptr);
}

QStandardItem *XmlProxyModel::GetRootOfModel()
{
    if (proxyModel != nullptr)
    {
        return proxyModel->item(0);
    }
    return nullptr;
}

bool XmlProxyModel::HasAttributes(QStandardItem *node)
{
    auto attr = node->child(0);
    if (attr != nullptr)
    {
        auto val = attr->data();
        if (val.isValid() && val.canConvert<QString>())
        {
            return val.value<QString>() == "attributes";
        }
    }
    return false;
}

int XmlProxyModel::CountAttributes(QStandardItem *node)
{
    if (HasAttributes(node))
        return node->child(0)->rowCount();
    else
        return 0;
}
