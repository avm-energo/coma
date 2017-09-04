#ifndef TUNEDIALOGA1DN_H
#define TUNEDIALOGA1DN_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "abstracttunedialog.h"
#include "../config/configa1.h"
#include "../check/checka1.h"

class TuneDialogA1DN : public AbstractTuneDialog
{
    Q_OBJECT

public:
    explicit TuneDialogA1DN(QWidget *parent = 0);

private:
    struct Bac
    {
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений вблизи 12, 30, 48, 60 и 72 В
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя
        float PhyDN[6]; 	// фазовый сдвиг ДН на частоте 50 Гц для значений напряжения U1kDN[6]
        float dU_cor[5];    // относительная ампл. погрешность установки после коррекции, в %
        float dPhy_cor[5];  // абс. фазовая погрешность установки после коррекции, срад
        float ddU_cor[5];	// среднеквадратичное отклонение амплитудной погрешности
        float ddPhy_cor[5]; // среднеквадратичное отклонение фазовой погрешности
        float K_DN;         // номинальный коэффициент деления ДН
        quint32 DNFNum;     // заводской номер делителя
    };

    Bac Bac_block;
    ConfigA1 *CA1;
    CheckA1 *ChA1;
    bool Accepted;

    void SetLbls();
    void SetPf();
    void SetupUI();

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
    int Start7_2_9(int counter);
    int ReadAnalogMeasurements();
    int ShowScheme();
    void GetBdAndFillMTT();

private slots:
    void FillBac();
    void FillBackBac();
    void FillBdOut();
    void FillBackBdOut();
    void FillBdIn();
    void FillBackBdIn();
    void SetDefCoefs();
    void AcceptDNData();

signals:
    void DNDataIsSet();
    void StartPercents(quint32 Percent);
    void SetPercent(quint32 Percent);
};

#endif // TUNEDIALOGA1DN_H
