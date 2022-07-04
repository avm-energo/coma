#include "xmleditcheckparser.h"

XmlEditCheckParser::XmlEditCheckParser(QStandardItemModel *input_model, QObject *parent)
    : QObject(parent), model(input_model)
{
}

void XmlEditCheckParser::ParseXmlCheck(QDomNode &checkNode, int &index, QStandardItem *parent)
{
    // Парсим имя
    QString name = "";
    auto header = checkNode.attributes().namedItem("header");
    if (!header.isNull())
        name = header.toAttr().value();
    else
        name = checkNode.nodeName();

    if (parent != nullptr)
    {
        auto element = new QStandardItem(name);
        parent->setChild(index, element);
        index++;

        int newIndex = 0;
        ParseXmlCheckChildrens(checkNode, newIndex, element, "groups");
        ParseXmlCheckChildrens(checkNode, newIndex, element, "signals");
        ParseXmlCheckChildrens(checkNode, newIndex, element, "records");
    }
}

std::pair<QDomNode, QStandardItem *> XmlEditCheckParser::ParseXmlAddCheckSubgroups(
    QDomNode &checkNode, int &index, QStandardItem *parent, QString &groupName)
{
    auto checkSubgroup = checkNode.firstChildElement(groupName);
    if (!checkSubgroup.isNull() && parent != nullptr)
    {
        auto group = new QStandardItem(checkSubgroup.nodeName());
        parent->setChild(index, group);
        index++;
        return std::pair(checkSubgroup, group);
    }
    else
        return std::pair(QDomNode(), nullptr);
}

void XmlEditCheckParser::ParseXmlCheckChildrens(
    QDomNode &checkNode, int &index, QStandardItem *parent, QString groupName)
{
    auto result = ParseXmlAddCheckSubgroups(checkNode, index, parent, groupName);
    auto groupNode = result.first;
    auto newParent = result.second;
    if (groupNode.hasChildNodes() && newParent != nullptr)
    {
        int newIndex = 0;
        auto groupElement = groupNode.firstChild();

        // Выбираем исполнителя
        auto executor = &XmlEditCheckParser::ParseXmlCheckGroup;
        // Парсим элемент из groups
        if (groupName == "groups")
            executor = &XmlEditCheckParser::ParseXmlCheckGroup;
        // Парсим элемент из signals
        else if (groupName == "signals")
            executor = &XmlEditCheckParser::ParseXmlCheckSignal;
        // Парсим элемент из records
        else if (groupName == "records")
            executor = &XmlEditCheckParser::ParseXmlCheckRecord;
        // unexpected, null it
        else
            executor = nullptr;

        // Если указатель на функцию равен nullptr, то сразу выходим из цикла
        while (!groupElement.isNull() && executor != nullptr)
        {
            // Если элемент - комментарий
            if (groupElement.isComment())
            {
                groupElement = groupElement.nextSibling();
                continue;
            }
            (this->*executor)(groupElement, newIndex, newParent);
            newIndex++;
            groupElement = groupElement.nextSibling();
        }
    }
    return;
}

void XmlEditCheckParser::ParseXmlCheckGroup(QDomNode &groupNode, int &index, QStandardItem *parent)
{
    // Имя группы
    auto nameElement = groupNode.firstChildElement("name");
    if (!nameElement.isNull())
    {
        auto nameText = nameElement.firstChild().toText();
        if (!nameText.isNull())
        {
            auto name = new QStandardItem(nameText.data());
            parent->setChild(index, 0, name);
        }
    }
    // ID группы
    auto idElement = groupNode.firstChildElement("id");
    if (!idElement.isNull())
    {
        auto idText = idElement.firstChild().toText();
        if (!idText.isNull())
        {
            auto id = new QStandardItem(idText.data());
            parent->setChild(index, 1, id);
        }
    }
}

void XmlEditCheckParser::ParseXmlCheckSignal(QDomNode &signalNode, int &index, QStandardItem *parent)
{
    // Тип сигнала
    auto typeElement = signalNode.firstChildElement("type");
    if (!typeElement.isNull())
    {
        auto typeText = typeElement.firstChild().toText();
        if (!typeText.isNull())
        {
            auto type = new QStandardItem(typeText.data());
            parent->setChild(index, 0, type);
        }
    }
    // Стартовый адрес
    auto addrElement = signalNode.firstChildElement("start-addr");
    if (!addrElement.isNull())
    {
        auto addrText = addrElement.firstChild().toText();
        if (!addrText.isNull())
        {
            auto addr = new QStandardItem(addrText.data());
            parent->setChild(index, 1, addr);
        }
    }
    // Count
    auto countElement = signalNode.firstChildElement("count");
    if (!countElement.isNull())
    {
        auto countText = countElement.firstChild().toText();
        if (!countText.isNull())
        {
            auto count = new QStandardItem(countText.data());
            parent->setChild(index, 2, count);
        }
    }
}

void XmlEditCheckParser::ParseXmlCheckRecord(QDomNode &recordNode, int &index, QStandardItem *parent)
{
    // Имя записи
    auto attrs = recordNode.attributes();
    if (attrs.count() > 0)
    {
        auto headerNode = attrs.namedItem("header");
        if (!headerNode.isNull())
        {
            auto header = headerNode.toAttr().value();
            auto descNode = attrs.namedItem("desc");
            if (!descNode.isNull())
            {
                auto desc = descNode.toAttr().value();
                header = header + " (" + desc + ")";
            }
            auto head = new QStandardItem(header);
            parent->setChild(index, 0, head);
        }
    }
    // Группа
    auto groupElement = recordNode.firstChildElement("group");
    if (!groupElement.isNull())
    {
        auto groupText = groupElement.firstChild().toText();
        if (!groupText.isNull())
        {
            auto group = new QStandardItem(groupText.data());
            parent->setChild(index, 1, group);
        }
    }
    // Стартовый адрес
    auto addrElement = recordNode.firstChildElement("start-addr");
    if (!addrElement.isNull())
    {
        auto addrText = addrElement.firstChild().toText();
        if (!addrText.isNull())
        {
            auto addr = new QStandardItem(addrText.data());
            parent->setChild(index, 2, addr);
        }
    }
    // Count
    auto countElement = recordNode.firstChildElement("count");
    if (!countElement.isNull())
    {
        auto countText = countElement.firstChild().toText();
        if (!countText.isNull())
        {
            auto count = new QStandardItem(countText.data());
            parent->setChild(index, 3, count);
        }
    }
}
