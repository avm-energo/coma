#ifndef JOURNALDIALOG_H
#define JOURNALDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include "../gen/maindef.h"
#include "../widgets/etableview.h"
#include "../widgets/etablemodel.h"
#include "../gen/s2.h"


#define MAXSWJNUM   262144

class JournalDialog : public QDialog
{
    Q_OBJECT
public:
    JournalDialog();

    struct SystemWorkStruct{
    quint64 Time;
    quint8 EvType;
    quint8 EvNum[3];
    quint32 Reserv;
    };

    struct MeasureStruct{
    quint32	NUM;
    quint32 Time;
    float Ueff[3];
    float Ieff[3];
    float Frequency;
    float U0;
    float U1;
    float U2;
    float I0;
    float I1;
    float I2;
    float Cbush[3];
    float Tg_d[3];
    float dCbush[3];
    float dTg_d[3];
    float Iunb;
    float Phy_unb;
    float Tmk;
    float Tamb;
    quint32 res;
    };	// sizeof(Bda_trend_struct)=31*4=124

    quint8 start;
    quint64 LTime;
    int SaveI;

private:

    ETableModel *TableSysModel;
    ETableModel *TableWorkModel;
    ETableModel *TableMeasModel;
    void UpdateSysModel();
    void UpdateWorkModel();
    void UpdateMeasModel();

    void SetupUI();
    void ProcessSWJournal(QByteArray &ba);

signals:
    void ReadJour(char*);

private slots:
    void GetSystemJour();
    void GetWorkJour();
    void GetMeasJour();
    void FillSysJour(QVector<S2::DataRec>*);
    void FillWorkJour(QVector<S2::DataRec>*);
    void FillMeasJour(QVector<S2::DataRec>*);
};

#endif // JOURNALDIALOG_H
