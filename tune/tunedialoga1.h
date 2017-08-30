#ifndef TUNEDIALOGA1_H
#define TUNEDIALOGA1_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "abstracttunedialog.h"
#include "../config/configa1.h"
#include "../check/checka1.h"

// определения для процедуры проверки CheckBdaValues
#define CHECK_VOLT  1
#define CHECK_PT100 2
#define CHECK_MA1   3
#define CHECK_MA2   4

class TuneDialogA1 : public AbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialogA1(QWidget *parent = 0);

signals:
    void Finished();

public slots:

private:
    ConfigA1 *CA1;
    CheckA1 *ChA1;

    struct Bac
    {
        float KmU[2];		// калибровочные коэффициенты по напряжению в 2 каналах
        float K_freq;		// коррекция частоты
        float DPhy;         // коррекция фазы в приборе (в градусах)
        float Art;          // коэффициент в канале Pt100, ед.АЦП/Ом
        float Brt;          // смещение в канале Pt100, ед.АЦП
        float Ama1; 		// коэффициент в канале A1, ед./мА
        float Bma1; 		// смещение в канале A1, ед.
        float Ama2; 		// коэффициент в канале A2, ед./мА
        float Bma2; 		// смещение в канале A2, ед.
    };

    Bac Bac_block;

    struct EMData
    {
        float u;
        float freq;
    };

    EMData RealData;
    float RegData;

    void SetLbls();
    void SetPf();
    void SetupUI();

    int Start6_3_1();
    int Start6_3_2_1();
    int Start6_3_2_2();
    int Start6_3_2_3();
    int Start6_3_3_1();
    int Start6_3_3_2();
    int Start6_3_3_3();
    int Start6_3_3_4();
    int Start6_3_4();
    int Start6_3_5_1();
    int Start6_3_5_2();
    int Start6_3_5_3();
    int Start6_3_6();
    int Start6_3_7_1();
    int Start6_3_7_2();
    int Start6_3_7_3();
    int Start6_3_8();
    int Start6_3_9();

    int ShowScheme();
    int CheckBdaValues(int checktype);
    int CheckAnalogValues(bool isPrecise); // проверка Bda_in на корректность
    int GetExternalData(); // ввод данных в зависимости от выбранного режима и номера опыта
    void GetBdAndFillMTT();

private slots:
    int ReadAnalogMeasurements();
    void FillBac();
    void FillBackBac();
    void SetDefCoefs();
    void SetExtData();
    void CancelExtData();

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // TUNEDIALOGA1_H
