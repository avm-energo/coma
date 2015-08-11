#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "widgets/s_tablemodel.h"
#include "publicclass.h"

#define MT_A_OSCTYPE    0x3e8

class oscdialog : public QDialog
{
    Q_OBJECT

public:
    explicit oscdialog(QWidget *parent=0);

private:
    void SetupUI();

    s_tablemodel *tm;
    QByteArray *OscInfo;
    QString GivenFilename;
    publicclass::DataRec Config[2];
    typedef struct
    {
        quint32 ChNum;
        quint32 UnixTime;
        quint32 NsTime;
        quint32 PtPer;
    } OscHeader_type;

    OscHeader_type OscHeader;

signals:
    void endprogressbar();

private slots:
    void GetOscInfo();
    void ProcessOscInfo();
    void SetProgressBarSize(quint32);
    void SetProgressBar(quint32);
    void EndExtractOsc();
    void GetOsc(QModelIndex);
};

#endif // OSCDIALOG_H
