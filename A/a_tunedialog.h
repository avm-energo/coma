#ifndef A_TUNEDIALOG_H
#define A_TUNEDIALOG_H

#include <QDialog>
//#include "canal.h"

#define ATUNENUMPOINTS  64 // количество точек, по которым выдаются значения в блоке Bda

#define TUNEFILELENGTH  256

#define ATUNEER(a)       ERMSG(publicclass::ER_ATUNE,__LINE__,a)
#define ATUNEDBG         DBGMSG(publicclass::ER_ATUNE,__LINE__)
#define ATUNEWARN(a)    WARNMSG(publicclass::ER_ATUNE,__LINE__,a)
#define ATUNEINFO(a)     INFOMSG(publicclass::ER_ATUNE,__LINE__,a)

class a_tunedialog : public QDialog
{
    Q_OBJECT
public:
    explicit a_tunedialog(QWidget *parent = 0);

signals:

public slots:

private:
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
    bool tune(int Type, int ChNum);
    void ShowErrMsg(int);
    bool CalcNewTuneCoef(int ChNum);
    bool RefreshTuneCoef(int ChNum);
    void RefreshTuneCoefs();
    bool CheckAndShowTune0(int ChNum);
    bool CheckAndShowTune5(int ChNum);
    bool CheckAndShowTune20(int ChNum);
    bool CheckTuneCoefs();

private slots:
    void StartTune();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
};

#endif // A_TUNEDIALOG_H
