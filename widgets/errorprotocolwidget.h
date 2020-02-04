#ifndef ERRORPROTOCOLWIDGET_H
#define ERRORPROTOCOLWIDGET_H

#include <QAbstractTableModel>
#include <QWidget>
#include "../gen/error.h"

#define MAX_MSG     1000

class ErrorProtocolModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ErrorProtocolModel(QObject *parent = 0);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);
    QVariant data (const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &index = QModelIndex()) const;
    int columnCount(const QModelIndex &index = QModelIndex()) const;
    void AddRow(Error::ErMsg ermsg);
    void InitModel();

private:
    QList<QStringList> erdata;
    QList<Error::ErMsgType> ertypedata;
    QStringList hdr;
    int MsgCount;

private slots:
};

class ErrorProtocolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ErrorProtocolWidget(QWidget *parent = 0);
    ~ErrorProtocolWidget();

    void AddRowToProt(Error::ErMsg ermsg);
    void InitModel();

signals:

public slots:

private:
    ErrorProtocolModel *Model;
};

#endif // ERRORPROTOCOLWIDGET_H
