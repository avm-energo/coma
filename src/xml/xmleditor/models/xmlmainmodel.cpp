#include "xmlmainmodel.h"

XmlMainModel::XmlMainModel(int rows, int cols, QObject *parent) : XmlModel(rows, cols, parent)
{
}

void XmlMainModel::setDataNode(GroupTypes type, QDomNode &root)
{
    if (!root.isNull())
    {
        auto rootName = root.nodeName();
        if (rootName == "modules")
        {
            auto module = root.firstChildElement("module");
            if (!module.isNull())
            {
                auto res = module.firstChildElement("resources");
                if (!res.isNull())
                {
                    setHorizontalHeaderLabels(settings.find(type)->second);
                    auto child = res.firstChild();
                    while (!child.isNull())
                    {
                        CheckChilds(child);
                        child = child.nextSibling();
                    }
                }
            }
        }
        else if (rootName == "s2files")
        {
            // TODO: сделать особый обработчик для s2files ноды
        }
    }
}

void XmlMainModel::CheckChilds(QDomNode &child)
{
    int row = 0;
    auto childNodeName = child.nodeName();
    auto type = types.find(childNodeName);
    if (type != types.cend())
    {
        auto groupType = type->second;
        auto itemIndex = index(row, 0);
        auto itemHeaderIndex = index(row, 1);
        setData(itemIndex, childNodeName);
        setData(itemHeaderIndex, child.toElement().attribute("header", ""));
        row++;
    }
}
