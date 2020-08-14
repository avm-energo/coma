#include <QDateTime>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>

#include "../gen/colors.h"
#include "errorprotocolwidget.h"
#include "etableview.h"

ErrorProtocolWidget::ErrorProtocolWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QWidget *w = new QWidget;
    QString ErrWss = "QWidget {background-color: " + QString(ERPROTCLR) + ";}";
    w->setStyleSheet(ErrWss);
    ETableView *tv = new ETableView;
    Model = new ErrorProtocolModel;
    Model->setHeaderData(0, Qt::Horizontal, "Номер", Qt::EditRole);
    Model->setHeaderData(1, Qt::Horizontal, "Дата/время", Qt::EditRole);
    Model->setHeaderData(2, Qt::Horizontal, "Номер сообщения", Qt::EditRole);
    Model->setHeaderData(3, Qt::Horizontal, "Тип сообщения", Qt::EditRole);
    Model->setHeaderData(4, Qt::Horizontal, "Сообщение", Qt::EditRole);
    tv->setModel(Model);
    QString Tvss = "QHeaderView::section {background-color: " + QString(ERPROTCLR) + ";}";
    tv->horizontalHeader()->setStyleSheet(Tvss);
    tv->resizeColumnsToContents();
    connect(Model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), tv, SLOT(resizeColumnsToContents()));
    tv->horizontalHeader()->setStretchLastSection(true);
    tv->horizontalHeader()->setEnabled(false);
    tv->verticalHeader()->setVisible(false);
    tv->verticalHeader()->setDefaultSectionSize(tv->verticalHeader()->fontMetrics().height() + 2);
    tv->setGridStyle(Qt::SolidLine);
    tv->setShowGrid(true);
    tv->setObjectName("ertv");
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    QHBoxLayout *upperLayout = new QHBoxLayout;
    upperLayout->addWidget(line, 2);
    QLabel *lbl = new QLabel("Протокол работы");
    lbl->setEnabled(false);
    upperLayout->addWidget(lbl, 0);
    upperLayout->setAlignment(lbl, Qt::AlignCenter);
    line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    upperLayout->addWidget(line, 2);
    lyout->addLayout(upperLayout, 0);
    lyout->addWidget(tv, 5);
    w->setLayout(lyout);
    QVBoxLayout *l = new QVBoxLayout;
    l->addWidget(w);
    setLayout(l);
}

ErrorProtocolWidget::~ErrorProtocolWidget()
{
}

void ErrorProtocolWidget::AddRowToProt(Error::ErMsg ermsg)
{
    Model->AddRow(ermsg);
}

void ErrorProtocolWidget::InitModel()
{
    Model->InitModel();
}

ErrorProtocolModel::ErrorProtocolModel(QObject *parent) : QAbstractTableModel(parent)
{
    MsgCount = 1;
}

QVariant ErrorProtocolModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole || role == Qt::EditRole) && (section < hdr.size()))
        return hdr.at(section);
    return QVariant();
}

bool ErrorProtocolModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal)
    {
        if (section >= hdr.size())
        {
            while (hdr.size() <= section)
                hdr.append("");
        }
        hdr.replace(section, value.toString());
    }
    return QAbstractTableModel::setHeaderData(section, orientation, value, role);
}

QVariant ErrorProtocolModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        if (index.row() < erdata.size())
        {
            if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
                return erdata.at(index.row()).at(index.column());
            else if (role == Qt::ForegroundRole)
            {
                if (ertypedata.at(index.row()) == Error::ER_MSG)
                    return QVariant::fromValue(QColor(255, 0, 0));
                else if (ertypedata.at(index.row()) == Error::WARN_MSG)
                    return QVariant::fromValue(QColor(255, 0, 255));
                else if (ertypedata.at(index.row()) == Error::INFO_MSG)
                    return QVariant::fromValue(QColor(0, 150, 67));
                else if (ertypedata.at(index.row()) == Error::DBG_MSG)
                    return QVariant::fromValue(QColor(0, 0, 255));
                else
                    return QVariant();
            }
            else if (role == Qt::BackgroundRole)
            {
                if (ertypedata.at(index.row()) == Error::ER_MSG)
                    return QVariant::fromValue(QColor(255, 233, 255)); // red
                else if (ertypedata.at(index.row()) == Error::WARN_MSG)
                    return QVariant::fromValue(QColor(255, 190, 255)); // magenta
                else if (ertypedata.at(index.row()) == Error::INFO_MSG)
                    return QVariant::fromValue(QColor(204, 253, 243)); // green
                else if (ertypedata.at(index.row()) == Error::DBG_MSG)
                    return QVariant::fromValue(QColor(190, 255, 255)); // blue
                else
                    return QVariant();
            }
            else if ((role == Qt::DecorationRole) && (index.column() == 4))
            {
                if (ertypedata.at(index.row()) == Error::ER_MSG)
                    return QVariant::fromValue(QIcon("images/er_msg.png"));
                else if (ertypedata.at(index.row()) == Error::WARN_MSG)
                    return QVariant::fromValue(QIcon("images/warn_msg.png"));
                else if (ertypedata.at(index.row()) == Error::INFO_MSG)
                    return QVariant::fromValue(QIcon("images/info_msg.png"));
                else if (ertypedata.at(index.row()) == Error::DBG_MSG)
                    return QVariant::fromValue(QIcon("images/dbg_msg.png"));
                else
                    return QVariant();
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags ErrorProtocolModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int ErrorProtocolModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return erdata.size();
}

int ErrorProtocolModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return 5;
}

void ErrorProtocolModel::AddRow(Error::ErMsg msg)
{
    //    beginResetModel();
    if (rowCount() < MAX_MSG)
        insertRows(0, 1, QModelIndex());
    QStringList tmpsl = QStringList() << "#" + QString::number(MsgCount)
                                      << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") << msg.file
                                      << QString::number(msg.line, 10) << msg.msg;
    MsgCount++;
    if (rowCount() >= MAX_MSG)
    {
        erdata.removeLast();
        ertypedata.removeLast();
    }
    erdata.insert(0, tmpsl);
    ertypedata.insert(0, msg.type);
    //    endResetModel();
    emit dataChanged(index(0, 0), index(rowCount(), columnCount()));
}

void ErrorProtocolModel::InitModel()
{
    beginResetModel();
    int beg = Error::ErMsgPoolSize() - 1;
    int end = (beg > MAX_MSG) ? (beg - MAX_MSG) : 0;
    insertRows(0, (beg - end + 1), QModelIndex());
    for (int i = beg; i >= end; --i)
    {
        Error::ErMsg msg = Error::ErMsgAt(i);
        QStringList tmpsl = QStringList()
            << "#" + QString::number(MsgCount) << msg.DateTime << msg.file << QString::number(msg.line, 10) << msg.msg;
        ++MsgCount;
        erdata.append(tmpsl);
        ertypedata.append(msg.type);
    }
    endResetModel();
    emit dataChanged(index(0, 0), index(rowCount(), columnCount()));
}
