#pragma once

#include "../models/edynamictablemodel.h"
#include "../module/modulesettings.h"
#include "../widgets/etableview.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <QTimeZone>
#include <gen/stdfunc.h>

namespace journals
{

enum JournalType
{
    System = 4,
    Work = 5,
    Meas = 6
};

class BaseJournal : public QObject
{
    Q_OBJECT
protected:
    QString jourName;
    QString viewName;
    QStringList headers;
    JournalType type;
    QTimeZone timezone;
    UniquePointer<EDynamicTableModel> dataModel;
    UniquePointer<QSortFilterProxyModel> proxyModel;

    void setUserTimezone(QStringList &data);
    virtual void fillTable(const QByteArray &ba) = 0;

public:
    explicit BaseJournal(QObject *parent = nullptr);
    UniquePointer<ETableView> createModelView(QWidget *parent);
    const QString &getName() const;

signals:
    void done(const QString &msg);
    void error(const QString &msg);

public slots:
    void fill(const QVariant &data);
    void save(const QString &filename);
};

}
