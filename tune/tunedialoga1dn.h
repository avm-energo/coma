#ifndef TUNEDIALOGA1DN_H
#define TUNEDIALOGA1DN_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "eabstracttunedialog.h"
#include "../config/configa1.h"
#include "../check/checka1.h"

#define TUNEA1LEVELS    8
#define TUNEVARIANTSNUM 3

class TuneDialogA1DN : public EAbstractTuneDialog
{
    Q_OBJECT

public:
    explicit TuneDialogA1DN(QWidget *parent = 0);
    ~TuneDialogA1DN();

private:
    struct Baci
    {
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений вблизи 12, 30, 48, 60 и 72 В
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя
        float PhyDN[6]; 	// фазовый сдвиг ДН на частоте 50 Гц для значений напряжения U1kDN[6]
        float dU_cor[5];    // относительная ампл. погрешность установки после коррекции, в %
        float dPhy_cor[5];  // абс. фазовая погрешность установки после коррекции, мин
        float ddU_cor[5];	// среднеквадратичное отклонение амплитудной погрешности
        float ddPhy_cor[5]; // среднеквадратичное отклонение фазовой погрешности
        float K_DN;         // номинальный коэффициент деления ДН
    };

    struct Bac
    {
        Baci Bac_block[TUNEVARIANTSNUM];
        quint32 DNFNum;     // заводской номер делителя
    };

    Bac Bac_block;

    struct DdStruct
    {
        float dUrms;
        float Phy;
        float sU;
        float sPhy;
    };

    DdStruct Dd_Block[TUNEA1LEVELS];

    ConfigA1 *CA1;
    CheckA1 *ChA1;
    bool Accepted;
    int PovNumPoints;

    void SetLbls();
    void SetPf();
    void SetupUI();

    QWidget *CoefUI(int bac2num);
    int InputDNData();
    int Start7_2_2();
    int Start7_2_3_1();
    int Start7_2_3_2();
    int Start7_2_3_3();
    int Start7_2_3_4();
    int Start7_2_3_5();
    int Start7_2_345(int counter);
    int Start7_2_67();
    int Start7_2_8();
    int Start7_2_9_1();
    int Start7_2_9_2();
    int Start7_2_9_3();
    int Start7_2_9_4();
    int Start7_2_9_5();
    int Start7_2_9_6();
    int Start7_2_9_7();
    int Start7_2_9_8();
    int Start7_2_9(int counter);
    int ReadAnalogMeasurements();
    int ShowScheme();
    void GetBdAndFillMTT();
    void LoadSettings();

private slots:
    void FillBac();
    void FillBackBac();
    void FillBdOut();
    void FillBackBdOut();
    void FillBdIn();
    void FillBackBdIn();
    void FillMedian(int index); // заполнение значений по средним показателям - медианам и СКО
    void SetDefCoefs();
    void AcceptDNData();

signals:
    void DNDataIsSet();
    void StartPercents(quint32 Percent);
    void SetPercent(quint32 Percent);
};

#endif // TUNEDIALOGA1DN_H
