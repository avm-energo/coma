#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "widgets/s_tablemodel.h"
#include "publicclass.h"

#define MT_HEAD_ID      1000 // ID шапки осциллограммы равен 1000
#define MT_START_OSC    1001 // первый ID осциллограммы
#define MT_END_OSC      1033 // последний ID осциллограммы

#define MAXOSCBUFSIZE   40000 // максимальный размер буфера для осциллограмм

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
    publicclass::DataRec Config[32];

    typedef struct
    {
        quint32 UnixTime;
        quint32 NsTime;
        float Step;
        quint32 Len;
    } OscHeader_Data;

    OscHeader_Data OscHeader;

signals:

private slots:
    void GetOscInfo();
    void ProcessOscInfo();
    void EndExtractOsc();
    void GetOsc(QModelIndex);
    void ErMsg(int ermsg);
    void EraseOsc();
    void SetProgressBar(quint32 cursize);
    void SetProgressBarSize(quint32 size);
};

#endif // OSCDIALOG_H
