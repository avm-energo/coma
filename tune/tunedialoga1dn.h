#ifndef TUNEDIALOGA1DN_H
#define TUNEDIALOGA1DN_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "eabstracttunedialoga1dn.h"
#include "eabstracttunedialog.h"

class TuneDialogA1DN : public EAbstractTuneDialogA1DN
{
    Q_OBJECT

public:
    explicit TuneDialogA1DN(QWidget *parent = nullptr);
    ~TuneDialogA1DN();

private:
    void SetupUI();
    QWidget *CoefUI(int bac2num);
    QWidget *CoefUI3(int bac3num);

#if PROGSIZE != PROGSIZE_EMUL
    void SetLbls();
    void SetPf();
    int Start7_2_3();
    int Start7_2_5();
    int Start7_2_6();
    int Start7_2_7_1();
    int Start7_2_7_2();
    int Start7_2_7_3();
    int Start7_2_7_4();
    int Start7_2_7_5();
    int Start7_2_78910(int counter);
    int Start7_2_11();
    int Start7_2_12();
    int Start7_2_13_1();
    int Start7_2_13_2();
    int Start7_2_13_3();
    int Start7_2_13_4();
    int Start7_2_13_5();
    int Start7_2_13_6();
    int Start7_2_13_7();
    int Start7_2_13_8();
    int Start7_2_13(int counter);
    int Start7_2_14();
    int ReadAnalogMeasurements();
    int ShowScheme();
#endif
    void WriteBacBlock();
    void LoadSettings();
    void GenerateReport();
//    void FillHeaders();

private slots:
#if PROGSIZE != PROGSIZE_EMUL
/*    void FillBdOut();
    void FillBackBdOut();
    void FillBdIn();
    void FillBackBdIn();
    void FillMedian(int index); // заполнение значений по средним показателям - медианам и СКО */
//    void AcceptDNData();

#endif
};

#endif // TUNEDIALOGA1DN_H
