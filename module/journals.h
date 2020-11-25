#ifndef JOURNALS_H
#define JOURNALS_H

#include "../gen/datatypes.h"
#include "../models/etablemodel.h"
#include "modules.h"

#include <QSortFilterProxyModel>

class Journals : public QObject
{
    Q_OBJECT

public:
    //    /*!
    //    Приложение 3. Номера файлов

    //        № файла	Описание
    //        1	Конфигурация
    //        3	Встроенное ПО (Firmware)
    //        4	Системный журнал
    //        5	Рабочий журнал
    //        6	Журнал измерений
    //        17	Журнал переключений
    //    */
    //    enum JournalEnum
    //    {
    //        JOURSYS = 4,
    //        JOURWORK = 5,
    //        JOURMEAS = 6
    //    };

    //    struct Measure
    //    {
    //        quint32 NUM;
    //        quint32 Time;
    //        float Ueff[3];
    //        float Ieff[3];
    //        float Frequency;
    //        float U0;
    //        float U1;
    //        float U2;
    //        float I0;
    //        float I1;
    //        float I2;
    //    };

    //    struct MeasureStruct : Measure
    //    {
    //        MeasureStruct(Measure meas) : Measure(meas) { }
    //        MeasureStruct() = default;
    //        float Cbush[3];
    //        float Tg_d[3];
    //        float dCbush[3];
    //        float dTg_d[3];
    //        float Iunb;
    //        float Phy_unb;
    //        float Tmk;
    //        float Tamb;
    //        quint32 res;
    //    }; // sizeof(Bda_trend_struct)=31*4=124

    //    struct MeasureStructKTF : Measure
    //    {
    //        MeasureStructKTF(Measure meas) : Measure(meas) { }
    //        MeasureStructKTF() = default;
    //        float Pf[4];
    //        float Qf[4];
    //        float Sf[4];
    //        float Cosphi;
    //        float Twin;
    //        float Tmk;
    //        float Tamb;
    //        quint32 res;
    //    }; // sizeof(Bda_trend_struct)=31*4=124
    QStringList m_sysJourDescription, m_workJourDescription, m_measJourHeaders;

    explicit Journals(QObject *parent = nullptr);
    ~Journals();

    void SetProxyModels(QSortFilterProxyModel *workmdl, QSortFilterProxyModel *sysmdl, QSortFilterProxyModel *measmdl);
    void SetJourType(int jourtype);
    void SetJourFile(const QString &jourfile);
    //    void SetParentWidget(QWidget *w);
    virtual void setWorkJourDescription() = 0;
    virtual void setMeasJourHeaders() = 0;
    virtual void setMeasRecord(char *file, QVector<QVariant> &outvl) = 0;
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

private:
    // QMutex _mutex;
    ETableModel *m_sysModel, *m_workModel, *_measModel;
    QSortFilterProxyModel *_proxySysModel, *_proxyWorkModel, *_proxyMeasModel;

    int m_jourType;
    QString m_jourFile;
    const QStringList EventJourHeaders = AVM::eventJourHeaders;
    const QStringList SysJourDescription = AVM::sysJourDescription;

    void FillEventsTable(QByteArray &ba);
    void FillMeasTable(QByteArray &ba);
    void ResultReady();

    void prepareJour(QByteArray &ba, int JourType);

public slots:
    void FillJour(const DataTypes::FileStruct &fs);
    //    void FillSysJour(QByteArray ba);
    //    void FillMeasJour(QByteArray ba);
    //    void FillWorkJour(QByteArray ba);
    void ReadJourFileAndProcessIt();
    //    void StartGetJour();
    void StartSaveJour(int jtype, QAbstractItemModel *mdl, QString filename);
};

#endif // JOURNALS_H
