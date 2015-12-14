#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "widgets/s_tablemodel.h"
#include "publicclass.h"

#define MT_E_OSCTYPE    0x00c9 // ID осциллограммы в модуле Э равен 201
#define MT_A_OSCTYPE    0x03e8 // ID осциллограммы в модуле А равен 1000

#define OSCER(a)       ERMSG(publicclass::ER_OSC,__LINE__,a)
#define OSCDBG         DBGMSG(publicclass::ER_OSC,__LINE__)
#define OSCWARN        WARNMSG(publicclass::ER_OSC,__LINE__)
#define OSCINFO(a)     INFOMSG(publicclass::ER_OSC,__LINE__,a)

class oscdialog : public QDialog
{
    Q_OBJECT

public:
    explicit oscdialog(QWidget *parent=0);
    ~oscdialog();

private:
    void SetupUI();

    s_tablemodel *tm;
    QByteArray *OscInfo;
    QString GivenFilename;
    QString OscDateTime;
    publicclass::DataRec Config[2];
    typedef struct
    {
        quint32 UnixTime;
        quint32 NsTime;
        float PtPer;
        quint32 Len;
        quint32 Qty;
        quint32 ChNum;
    } OscHeader_type;

    OscHeader_type OscHeader;

signals:

private slots:
    void GetOscInfo();
    void ProcessOscInfo();
    void EndExtractOsc();
    void GetOsc(QModelIndex);
};

#endif // OSCDIALOG_H
