#include "errorprotocolmodel.h"

#include <QBrush>
#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QString>
#include <QStringList>
#include <gen/error.h>
#include <gen/errorqueue.h>
#include <gen/helper.h>

ErrorProtocolModel::ErrorProtocolModel(QObject *parent) : QAbstractTableModel(parent)
{
}

ErrorProtocolModel::~ErrorProtocolModel()
{
    qDeleteAll(m_dataptr);
    m_dataptr.clear();
}

QVariant ErrorProtocolModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole) && (section < m_hdr.size()))
        return m_hdr.at(section);
    if ((orientation == Qt::Vertical) && (role == Qt::DisplayRole) && (section < m_dataptr.size()))
        return section;
    return QVariant();
}

bool ErrorProtocolModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal)
    {
        m_hdr.insert(section, value.toString());
    }
    return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}

QVariant ErrorProtocolModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && (index.row() < m_dataptr.size()))
        return m_dataptr.at(index.row())->data(index.column(), role);
    return QVariant();
}

Qt::ItemFlags ErrorProtocolModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | QAbstractTableModel::flags(index);
}

int ErrorProtocolModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return m_dataptr.size();
}

int ErrorProtocolModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return m_hdr.size();
}

bool ErrorProtocolModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1)
        return false;
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; i++)
    {
        ErrorProtocolItem *item = new ErrorProtocolItem();
        if (i >= rowCount() && i <= rowCount())
            m_dataptr.append(item);
        else
            m_dataptr.insert(row, item);
    }
    endInsertRows();
    return true;
}

bool ErrorProtocolModel::insertRow(int row, ErrorProtocolItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    m_dataptr.insert(row, item);
    endInsertRows();
    return true;
}

bool ErrorProtocolModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && value.isValid())
    {
        if (m_dataptr.size() <= index.row())
            m_dataptr.push_back(new ErrorProtocolItem);
        auto &item = m_dataptr[(index.row())];
        item->setData(index.column(), value, role);
    }
    return false;
}

void ErrorProtocolModel::appendRows(const std::vector<ErrorMsg> &msgs, size_t newItems)
{
    constexpr int colorFactor = 190;
    size_t counter = 0;
    for (auto it = msgs.crbegin(); it != msgs.crend(); ++it)
    {
        QStringList tmpsl;
        tmpsl << (*it);
        auto newIndex = createIndex(int(counter), 0);
        if (!newIndex.isValid())
            continue;
        setData(newIndex, tmpsl, Qt::DisplayRole);
        switch (it->type)
        {

        case QtDebugMsg:
            setData(newIndex, QBrush(QColor(Qt::blue).darker(colorFactor)), Qt::ForegroundRole);
            setData(newIndex, QBrush(QColor(Qt::blue).lighter(colorFactor)), Qt::BackgroundRole);
            break;
        case QtWarningMsg:
            setData(newIndex, QBrush(QColor(Qt::magenta).darker(colorFactor)), Qt::ForegroundRole);
            setData(newIndex, QBrush(QColor(Qt::magenta).lighter(colorFactor)), Qt::BackgroundRole);
            break;
        case QtCriticalMsg:
            setData(newIndex, QBrush(QColor(Qt::red).darker(colorFactor)), Qt::ForegroundRole);
            setData(newIndex, QBrush(QColor(Qt::red).lighter(colorFactor)), Qt::BackgroundRole);
            break;
        case QtFatalMsg:
            break;
        case QtInfoMsg:
            setData(newIndex, QBrush(QColor(Qt::green).darker(colorFactor)), Qt::ForegroundRole);
            setData(newIndex, QBrush(QColor(Qt::green).lighter(colorFactor)), Qt::BackgroundRole);
            break;
        default:
            assert(false);
        }
        if (counter < newItems)
        {
            QFont font;
            font.setBold(true);
            setData(newIndex, font, Qt::FontRole);
        }
        ++counter;
    }
}

void ErrorProtocolModel::initModel()
{
    auto &queue = ErrorQueue::GetInstance();

    m_dataptr.clear();
    appendRows(*queue.errMsgPool(), queue.counter());
    queue.clearCounter();
}
