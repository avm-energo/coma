#include "xmlsgroupmodel.h"

XmlSGroupModel::XmlSGroupModel(int rows, int cols, ModelType type, QObject *parent) : XmlModel(rows, cols, type, parent)
{
}

/// \brief Slot for receiveing data request from dialog window and sending response.
void XmlSGroupModel::getDialogRequest(const int &row)
{
    if (row >= 0 && row < rowCount())
    {
        QStringList retList;
        auto cols = columnCount();
        retList.reserve(cols);
        // Собираем открытые данные
        for (auto column = 0; column < cols; column++)
        {
            auto itemIndex = index(row, column);
            auto item = data(itemIndex);
            if (item.isValid() && item.canConvert<QString>())
            {
                auto itemStr = item.value<QString>();
                retList.append(itemStr);
            }
            else
                retList.append("");
        }
        // Собираем скрытые данные
        auto hiding = data(index(row, 0), SGroupDataRole);
        if (hiding.isValid() && hiding.canConvert<SGroupHideData>())
        {
            auto hidingVal = hiding.value<SGroupHideData>();
            retList.append(QString::number(hidingVal.count));
            retList.append(hidingVal.tooltip);
            retList.append(hidingVal.array.join(','));
        }
        // Отправляем сигнал с ответом
        emit sendDialogResponse(retList);
    }
}

/// \brief Slot for creating data in model.
void XmlSGroupModel::create(const QStringList &saved, int *row)
{
    Q_ASSERT(saved.count() == 5);
    IEditorModel::create({ saved[0], saved[1] }, row);
    if (*row >= 0 && *row < rowCount())
    {
        auto newHide = convertHideData({ saved[2], saved[3], saved[4] });
        setData(index(*row, 0), QVariant::fromValue(newHide), SGroupDataRole);
    }
}

/// \brief Slot for updating an item's data in the model (including hiding data).
void XmlSGroupModel::update(const QStringList &saved, const int &row)
{
    Q_ASSERT(saved.count() == 5);
    IEditorModel::update({ saved[0], saved[1] }, row);
    if (row >= 0 && row < rowCount())
    {
        auto newHide = convertHideData({ saved[2], saved[3], saved[4] });
        setData(index(row, 0), QVariant::fromValue(newHide), SGroupDataRole);
    }
    emit modelChanged();
}

/// \brief Parsing input XML nodes of file in model items.
void XmlSGroupModel::parseNode(QDomNode &node, int &row)
{
    parseTag(node, tags::start_addr, row, 0); // Адрес
    parseAttribute(node, tags::desc, row, 1); // Заголовок
    setData(index(row, 0), QVariant::fromValue(parseHideData(node)), SGroupDataRole);
}

/// \brief Creates XML DOM node representation of this model.
QDomElement XmlSGroupModel::toNode(QDomDocument &doc)
{
    auto sgroupNode = makeElement(doc, tags::sgroup);
    for (auto row = 0; row < rowCount(); row++)
    {
        // TODO: костыль
        if (data(index(row, 0)).value<QString>() != "..")
        {
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
    return sgroupNode;
}

/// \brief Parsing XML DOM node to SGroupHideData structure.
SGroupHideData XmlSGroupModel::parseHideData(QDomNode &node)
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

/// \brief Converting input string list to SGroupHideData structure.
SGroupHideData XmlSGroupModel::convertHideData(const QStringList &input)
{
    SGroupHideData hiding;
    auto state = false;
    auto count = input[0].toInt(&state);
    if (state)
        hiding.count = count;
    hiding.tooltip = input[1];
    hiding.array = input[2].split(',');
    return hiding;
}
