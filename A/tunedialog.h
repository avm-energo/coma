#ifndef A_TUNEDIALOG_H
#define A_TUNEDIALOG_H

#include <QDialog>
#include "canal.h"

#define Z   64 // количество точек, по которым выдаются значения в блоке Bda

class a_tunedialog : public QDialog
{
    Q_OBJECT
public:
    explicit a_tunedialog(QWidget *parent = 0);

signals:

public slots:

private:
    canal *cn;

    struct Bda
    {
        quint32 sin[16];
    };

    Bda Bda0, Bda5, Bda20;

    struct Bac
    {
        float fbin;
        float fkuin;
        float fkiin;
    };

    Bac Bac_block[16];

    void SetupUI();
    void tune(int);
    void ShowErrMsg(int);
    void CalcNewTuneCoefs();
    void RefreshTuneCoefs();

private slots:
    void tune0();
    void tune5();
    void tune20();
    void CheckAndShowTune0();
    void CheckAndShowTune5();
    void CheckAndShowTune20();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
};

#endif // A_TUNEDIALOG_H
