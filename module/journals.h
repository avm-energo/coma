#ifndef JOURNALS_H
#define JOURNALS_H

#include "../gen/datatypes.h"
#include "../models/etablemodel.h"

#include <QSortFilterProxyModel>

class Journals : public QObject
{
    Q_OBJECT

public:
    explicit Journals(QObject *parent = nullptr);
    ~Journals();

    void SetProxyModels(QSortFilterProxyModel *workmdl, QSortFilterProxyModel *sysmdl, QSortFilterProxyModel *measmdl);
    void SetJourType(DataTypes::FilesEnum jourtype);
    void SetJourFile(const QString &jourfile);
    //    void SetParentWidget(QWidget *w);

    virtual int measureSize() = 0;
    virtual int workJournalID() = 0;

signals:
    void Done(QString, int);
    void resendResult(int);
    void resendMaxResult(int);
    // void Fill(QVector<QVector<QVariant>>);
    void Error(QString);
    // void ModelReady(ETableModel *);
    void ReadJour(char);

protected:
    QStringList m_workJourDescription, m_measJourHeaders;
    virtual QVector<QVariant> createMeasRecord(const char *file) = 0;
    virtual void setWorkJourDescription() = 0;
    virtual void setMeasJourHeaders() = 0;

private:
    // QMutex _mutex;

    ETableModel *m_sysModel, *m_workModel, *_measModel;
    QSortFilterProxyModel *_proxySysModel, *_proxyWorkModel, *_proxyMeasModel;

    DataTypes::FilesEnum m_jourType;
    QString m_jourFile;

    void FillEventsTable(const QByteArray &ba);
    void FillMeasTable(const QByteArray &ba);
    void ResultReady();

    // void prepareJour(QByteArray &ba, int JourType);

public slots:
    void FillJour(const DataTypes::FileStruct &fs);
    //    void FillSysJour(QByteArray ba);
    //    void FillMeasJour(QByteArray ba);
    //    void FillWorkJour(QByteArray ba);
    // void ReadJourFileAndProcessIt();
    //    void StartGetJour();
    void StartSaveJour(int jtype, QAbstractItemModel *mdl, QString filename);
};

#endif // JOURNALS_H
