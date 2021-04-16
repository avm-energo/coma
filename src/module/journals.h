#ifndef JOURNALS_H
#define JOURNALS_H

#include "../gen/datatypes.h"
#include "../models/etablemodel.h"
#include "../module/modules.h"

#include <QSortFilterProxyModel>

class Journals : public QObject
{
    Q_OBJECT

public:
    explicit Journals(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent = nullptr);
    ~Journals();

    void SetProxyModels(QSortFilterProxyModel *workmdl, QSortFilterProxyModel *sysmdl, QSortFilterProxyModel *measmdl);
    void SetJourType(DataTypes::FilesEnum jourtype);
    void SetJourFile(const QString &jourfile);

    virtual int measureSize() = 0;
    int workJournalID();

signals:
    void Done(QString, DataTypes::FilesEnum);
    void resendResult(int);
    void resendMaxResult(int);
    void Error(QString);
    void ReadJour(char);

protected:
    QStringList m_workJourDescription, m_measJourHeaders;
    virtual QVector<QVariant> createMeasRecord(const char *file) = 0;

private:
    ETableModel *m_sysModel, *m_workModel, *_measModel;
    QSortFilterProxyModel *_proxySysModel, *_proxyWorkModel, *_proxyMeasModel;

    DataTypes::FilesEnum m_jourType;
    QString m_jourFile;
    const QMap<Modules::JournalType, DataTypes::JournalDesc> m_jourMap;
    void FillEventsTable(const QByteArray &ba);
    void FillMeasTable(const QByteArray &ba);
    void resultReady(ETableModel *model);

public slots:
    void FillJour(const DataTypes::FileStruct &fs);

    void saveJour(DataTypes::FilesEnum jtype, QAbstractItemModel *mdl, QString filename);
};

#endif // JOURNALS_H
