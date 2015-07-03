#ifndef E_TUNEDIALOG_H
#define E_TUNEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QByteArray>
#include "../iec104/iec104.h"

#define TUNEFILELENGTH  256

class e_tunedialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_tunedialog(QWidget *parent = 0);

signals:
    void stopall();
    void SendMip(QByteArray);
    void dataready(QByteArray);
    void error(int);

public slots:

private:
    iec104 *mipcanal;

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
    void CalcNewTuneCoefs();
    void RefreshTuneCoefs();

private slots:
    void StartTune();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
    void StartMip();
    void EthConnected();
    void MipConnected();
    void MipDataRcv(QByteArray);
    void MipDataXmit(QByteArray);
    void MipData();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // E_TUNEDIALOG_H