#pragma once

#include "../models/edynamictablemodel.h"
#include "../module/modulesettings.h"
#include "../widgets/etableview.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <QTimeZone>
#include <gen/stdfunc.h>

class BaseJournal : public QObject
{
    Q_OBJECT
protected:
    QString jourName;
    QString viewName;
    QStringList headers;
    QStringList description;
    QTimeZone timezone;
    UniquePointer<EDynamicTableModel> dataModel;
    UniquePointer<QSortFilterProxyModel> proxyModel;

    // const QStringList jourListToStringList(const QList<ModuleTypes::Journal> &jourList) const;
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
