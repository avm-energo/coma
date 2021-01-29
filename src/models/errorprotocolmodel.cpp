#include "errorprotocolmodel.h"

#include "../gen/errorqueue.h"
#include "../gen/logger.h"

#include <../gen/error.h>
#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QString>
#include <QStringList>

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
    {

        const ErrorPair *item = m_dataptr.at(index.row());
        switch (role)
        {
        case Qt::ForegroundRole:
        {
            switch (item->second)
            {
            case QtCriticalMsg:
                return QVariant::fromValue(QColor(255, 0, 0));
            case QtWarningMsg:
                return QVariant::fromValue(QColor(255, 0, 255));
            case QtInfoMsg:
                return QVariant::fromValue(QColor(0, 150, 67));
            case QtDebugMsg:
                return QVariant::fromValue(QColor(0, 0, 255));
            default:
                return QVariant();
            }
        }
        case Qt::BackgroundRole:
        {
            switch (item->second)
            {
            case QtCriticalMsg:
                return QVariant::fromValue(QColor(255, 233, 255)); // red
            case QtWarningMsg:
                return QVariant::fromValue(QColor(255, 190, 255)); // magenta
            case QtInfoMsg:
                return QVariant::fromValue(QColor(204, 253, 243)); // green
            case QtDebugMsg:
                return QVariant::fromValue(QColor(190, 255, 255)); // blue
            default:
                return QVariant();
            }
        }
        case Qt::DecorationRole:
        {
            if (index.column() == 3)
                switch (item->second)
                {
                case QtCriticalMsg:
                    return QVariant::fromValue(QIcon("images/er_msg.png"));
                case QtWarningMsg:
                    return QVariant::fromValue(QIcon("images/warn_msg.png"));
                case QtInfoMsg:
                    return QVariant::fromValue(QIcon("images/info_msg.png"));
                case QtDebugMsg:
                    return QVariant::fromValue(QIcon("images/dbg_msg.png"));
                default:
                    return QVariant();
                }
        }

        case Qt::DisplayRole:
            if (!item->first.isEmpty() && index.column() < item->first.size())
                return item->first.at(index.column());
        }
    }
    return QVariant();
}

Qt::ItemFlags ErrorProtocolModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
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
        ErrorPair *item = new ErrorPair();
        if (i >= rowCount() && i <= rowCount())
            m_dataptr.append(item);
        else
            m_dataptr.insert(row, item);
    }
    endInsertRows();
    return true;
}

bool ErrorProtocolModel::insertRow(int row, ErrorPair *item)
{
    beginInsertRows(QModelIndex(), row, row);
    m_dataptr.insert(row, item);
    endInsertRows();
    return true;
}

bool ErrorProtocolModel::appendRows(const ErrorList &list)
{
    for (ErrorPair item : list)
    {
        if (!appendRow(&item))
            return false;
    }
    return true;
}

bool ErrorProtocolModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && value.isValid())
    {
        switch (role)
        {
        case Qt::EditRole:
            if (index.column() < m_hdr.size())
            {
                m_dataptr.at(index.row())->first.insert(index.column(), value.toString());
                emit dataChanged(index, index);
                return true;
            }
            break;
        case Qt::ForegroundRole:

            return true;
        case Qt::FontRole:

            return true;
        case Qt::DecorationRole:

            return true;
        case Qt::TextAlignmentRole:

            return true;
        case Qt::UserRole:

            return true;
        }
    }
    return false;
}

void ErrorProtocolModel::appendRow(ErrorMsg msg)
{
    QStringList tmpsl;
    tmpsl << msg;
    ErrorPair *item = new ErrorPair(tmpsl, msg.type);
    appendRow(item);
}

void ErrorProtocolModel::initModel()
{
    auto &queue = ErrorQueue::GetInstance();

    m_dataptr.clear();
    for (const auto &error : *queue.errMsgPool())
    {
        appendRow(error);
    }
    queue.clearCounter();
}
