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
        float U1kDN[7];     // измеренные при калибровке напряжения на выходе своего ДН для значений вблизи 12, 24, 36, 48, 60 и 72 В
        float U2kDN[7];     // и соответствующие им значения на выходе эталонного делителя.
        float PhyDN[7]; 	// фазовый сдвиг ДН на частоте 50 Гц для значений напряжения U1kDN[6]
    };

    Bac Bac_block;
    ConfigA1 *CA1;
    CheckA1 *ChA1;

    void SetLbls();
    void SetPf();
    void SetupUI();

    int Start7_2_2();
    int Start7_2_3_1();
    int Start7_2_3_2();
    int Start7_2_3_3();
    int Start7_2_3_4();
    int Start7_2_3_5();
    int Start7_2_3_6();
    int Start7_2_345(int counter);
    int Start7_2_67();
    int Start7_2_8();
    int ReadAnalogMeasurements();
    int ShowScheme();
    void GetBdAndFillMTT();

private slots:
    void FillBac();
    void FillBackBac();
    void FillBdOut();
    void FillBackBdOut();
    void SetDefCoefs();
};

#endif // TUNEDIALOGA1DN_H
