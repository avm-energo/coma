#ifndef JOURNALS_H
#define JOURNALS_H

#include "../gen/datatypes.h"
#include "../gen/timefunc.h"
#include "../models/edynamictablemodel.h"
#include "../module/modules.h"
#include "../s2/filestruct.h"

#include <QDebug>
#include <QSortFilterProxyModel>
#include <QTimeZone>

class Journals : public QObject
{
    Q_OBJECT

    struct CommonEvent
    {
        int counter;
        quint64 time;
        QString desc;
        QString direction;
    };

public:
    explicit Journals(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent = nullptr);
    ~Journals();

    void SetProxyModels(QSortFilterProxyModel *workmdl, QSortFilterProxyModel *sysmdl, QSortFilterProxyModel *measmdl);
    void SetJourType(DataTypes::FilesEnum jourtype);
    void SetJourFile(const QString &jourfile);

    virtual int measureSize() = 0;
    int workJournalID();

public slots:
    void FillJour(const QVariant &data);
    void saveJour(DataTypes::FilesEnum jtype, QString filename);

signals:
    void Done(QString, DataTypes::FilesEnum);
    void resendResult(int);
    void resendMaxResult(int);
    void Error(QString);
    void ReadJour(char);

protected:
    QStringList m_workJourDescription, m_measJourHeaders;
    QTimeZone m_timezone;

    virtual QVector<QVariant> createMeasRecord(const char *file) = 0;
    virtual QVector<QVector<QVariant>> createMeas(const QByteArray &array) = 0;
    QVector<QVector<QVariant>> createCommon(const QByteArray &array, const int eventid, const QStringList &desc);

private:
    EDynamicTableModel *m_sysModel, *m_workModel, *m_measModel;
    QSortFilterProxyModel *_proxySysModel, *_proxyWorkModel, *_proxyMeasModel;
    DataTypes::FilesEnum m_jourType;
    QString m_jourFile;
    const QMap<Modules::JournalType, DataTypes::JournalDesc> m_jourMap;

    void FillEventsTable(const QByteArray &ba);
    void FillMeasTable(const QByteArray &ba);
    void resultReady(EDynamicTableModel *model);
};

#endif // JOURNALS_H
