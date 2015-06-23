#ifndef E_TUNEDIALOG_H
#define E_TUNEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QByteArray>
//#include "canal.h"

#define TUNEFILELENGTH  256
class e_tunedialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_tunedialog(QWidget *parent = 0);

signals:
    void stopall();
    void SendMip(QByteArray);

public slots:

private:
//    canal *cn;

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
    void ShowErrMsg(int);
    void StartTune();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
    void StartMip();
    void MipConnected();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // E_TUNEDIALOG_H
