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
    explicit TuneDialogA1DN(QWidget *parent = nullptr);
    ~TuneDialogA1DN();

private:
    enum UModes
    {
        MODE_ALTERNATIVE = 0,
        MODE_DIRECT = 1
    };

    enum UModesBlocks
    {
        BLOCK_ALTERNATIVE = 1,
        BLOCK_DIRECT = 2
    };

    struct Baci
    {
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений 0 и вблизи 12, 30, 48, 60 и 72 В
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

    struct Baci3
    {
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений 0 и вблизи 20, 50, 80, 100, 120 В
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя
        float dU_cor[5];    // относительная ампл. погрешность установки после коррекции, в %
        float ddU_cor[5];	// среднеквадратичное отклонение амплитудной погрешности
        float K_DN;         // номинальный коэффициент деления ДН
    };

    struct Bac3
    {
        Baci Bac_block3[TUNEVARIANTSNUM];
        quint32 DNFNum;
    };

    Bac3 Bac_block3;

    ConfigA1 *CA1;
    CheckA1 *ChA1;
    bool Accepted;
    int PovNumPoints;
    int Mode; // 0 - переменный, 1 - постоянный ток

    struct DdStruct
    {
        float dUrms;
        float Phy;
        float sU;
        float sPhy;
    };

    DdStruct Dd_Block[TUNEA1LEVELS];

    void SetupUI();
    QWidget *CoefUI(int bac2num);
    QWidget *CoefUI3(int bac3num);

#if PROGSIZE != PROGSIZE_EMUL
    void SetLbls();
    void SetPf();
    int Start7_2_3();
    int Start7_2_5();
    void InputTuneVariant(int varnum);
    int Start7_2_6();
    int Start7_2_7_1();
    int Start7_2_7_2();
    int Start7_2_7_3();
    int Start7_2_7_4();
    int Start7_2_7_5();
    int Start7_2_78910(int counter);
    int Start7_2_11();
    int Start7_2_12();

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
#endif
    void WriteBacBlock();
    void GetBdAndFillMTT();
    void LoadSettings();

private slots:
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);
    void SetDefCoefs(int bacnum);
#if PROGSIZE != PROGSIZE_EMUL
    void AcceptDNData();
    void FillBdOut();
    void FillBackBdOut();
    void FillBdIn();
    void FillBackBdIn();
    void FillMedian(int index); // заполнение значений по средним показателям - медианам и СКО
    void SetTuneVariant();
#endif
signals:
    void DNDataIsSet();
    void StartPercents(int Percent);
    void SetPercent(int Percent);
};

#endif // TUNEDIALOGA1DN_H
