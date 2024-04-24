#include "xml/xmleditor/models/xmlhidedatamodel.h"

#include <xml/xmltags.h>

XmlHideDataModel::XmlHideDataModel(int rows, int cols, ModelType type, QObject *parent)
    : XmlModel(rows, cols, type, parent)
{
}

QStringList XmlHideDataModel::getRowData(const int row)
{
    // Собираем открытые данные
    QStringList retList = BaseEditorModel::getRowData(row);
    // Собираем скрытые данные
    if (row >= 0 && row < rowCount())
    {
        switch (m_type)
        {
        case ModelType::SGroup:
        {
            auto hiding = data(index(row, 0), SGroupDataRole);
            if (hiding.isValid() && hiding.canConvert<SGroupHideData>())
            {
                auto hidingVal = hiding.value<SGroupHideData>();
                retList.append(QString::number(hidingVal.count));
                retList.append(hidingVal.tooltip);
                retList.append(hidingVal.array.join(','));
                retList.append(hidingVal.view);
            }
            break;
        }
        default:
            break;
        }
    }
    return retList;
}

void XmlHideDataModel::create(const QStringList &saved, int *row)
{
    switch (m_type)
    {
    case ModelType::SGroup:
        Q_ASSERT(saved.count() == 6);
        BaseEditorModel::create({ saved[0], saved[1] }, row);
        if (*row >= 0 && *row < rowCount())
        {
            auto newHide = convertToSGroupData({ saved[2], saved[3], saved[5], saved[4] });
            setData(index(*row, 0), QVariant::fromValue(newHide), SGroupDataRole);
        }
        emit modelChanged();
        break;
    default:
        break;
    }
}

void XmlHideDataModel::update(const QStringList &saved, const int row)
{
    switch (m_type)
    {
    case ModelType::SGroup:
        BaseEditorModel::update({ saved[0], saved[1] }, row);
        if (row >= 0 && row < rowCount())
        {
            auto newHide = convertToSGroupData({ saved[2], saved[3], saved[5], saved[4] });
            setData(index(row, 0), QVariant::fromValue(newHide), SGroupDataRole);
        }
        emit modelChanged();
        break;
    default:
        break;
    }
}

void XmlHideDataModel::parseNode(QDomNode &node, int &row)
{
    switch (m_type)
    {
    case ModelType::SGroup:
        parseTag(node, tags::start_addr, row, 0); // Адрес
        parseAttribute(node, tags::desc, row, 1); // Заголовок
        setData(index(row, 0), QVariant::fromValue(parseSGroupData(node)), SGroupDataRole);
        break;
    default:
        break;
    }
}

QDomElement XmlHideDataModel::toNode(QDomDocument &doc)
{
    switch (m_type)
    {
    case ModelType::SGroup:
    {
        auto sgroupNode = makeElement(doc, tags::sgroup);
        fillSGroupNode(doc, sgroupNode);
        return sgroupNode;
    }
    default:
        return makeElement(doc, "undefined");
    }
}

SGroupHideData XmlHideDataModel::parseSGroupData(QDomNode &node)
{
    SGroupHideData retVal;
    auto state = false;
    // Парсим тег count
    auto countNode = node.firstChildElement(tags::count);
    if (!countNode.isNull())
    {
        auto countText = countNode.firstChild().toText().data();
        auto count = countText.toInt(&state);
        if (state)
            retVal.count = count;
    }
    // Парсим тег toolTip
    auto tooltipNode = node.firstChildElement(tags::tooltip);
    if (!tooltipNode.isNull())
    {
        auto tooltip = tooltipNode.firstChild().toText().data();
        retVal.tooltip = tooltip;
    }
    // Парсим аттрибут view
    retVal.view = node.toElement().attribute(tags::view, "float");
    // Парсим тег string-array
    auto strArrayNode = node.firstChildElement(tags::str_array);
    if (!strArrayNode.isNull())
    {
        auto strItemNode = strArrayNode.firstChild();
        while (!strItemNode.isNull())
        {
            if (!strItemNode.isComment())
            {
                auto strItem = strItemNode.firstChild().toText().data();
                retVal.array.append(strItem);
            }
            strItemNode = strItemNode.nextSibling();
        }
    }
    return retVal;
}

SGroupHideData XmlHideDataModel::convertToSGroupData(const QStringList &input)
{
    SGroupHideData hiding;
    auto state = false;
    auto count = input[0].toInt(&state);
    if (state)
        hiding.count = count;
    hiding.tooltip = input[1];
    hiding.view = input[2];
    if (!input[3].isEmpty())
        hiding.array = input[3].split(',');
    return hiding;
}

void XmlHideDataModel::fillSGroupNode(QDomDocument &doc, QDomElement &sgroupNode)
{
    for (auto row = 0; row < rowCount(); row++)
    {
        // Обходим элемент для возвращения назад стороной
        if (data(index(row, 0)).value<QString>() == "..")
            continue;

        // Видимые данные
        auto mwidget = makeElement(doc, tags::mwidget);
        makeElement(doc, mwidget, tags::start_addr, data(index(row, 0)));
        setAttribute(doc, mwidget, tags::desc, data(index(row, 1)));
        // Скрытые данные
        auto hideDataVar = data(index(row, 0), SGroupDataRole);
        if (hideDataVar.isValid() && hideDataVar.canConvert<SGroupHideData>())
        {
            auto hideData = hideDataVar.value<SGroupHideData>();
            if (hideData.count != 1)
                makeElement(doc, mwidget, tags::count, QString::number(hideData.count));

            if (!hideData.tooltip.isEmpty())
                makeElement(doc, mwidget, tags::tooltip, hideData.tooltip);

            if (!hideData.view.isEmpty())
                setAttribute(doc, mwidget, tags::view, hideData.view);

            if (!(hideData.array.isEmpty() || (hideData.array.size() == 1 && hideData.array.first().isEmpty())))
            {
                auto strArray = makeElement(doc, tags::str_array);
                for (const auto &str : qAsConst(hideData.array))
                    makeElement(doc, strArray, tags::item, str);
                mwidget.appendChild(strArray);
            }
        }
        sgroupNode.appendChild(mwidget);
    }
}
