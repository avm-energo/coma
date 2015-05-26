#ifndef E_TUNEDIALOG_H
#define E_TUNEDIALOG_H

#include <QDialog>
#include "canal.h"

#define TUNEFILELENGTH  256
class e_tunedialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_tunedialog(QWidget *parent = 0);

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
    void ShowErrMsg(int);
    void CalcNewTuneCoefs();
    void RefreshTuneCoefs();

private slots:
    void StartTune();
    void ReadTuneCoefs();
    void ReadCompleted();
    void WriteTuneCoefs();
    void WriteCompleted();
    void SaveToFile();
    void LoadFromFile();
};

#endif // E_TUNEDIALOG_H
