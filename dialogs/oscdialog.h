#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "../gen/eoscillogram.h"
#include "../widgets/etablemodel.h"
#include "../gen/publicclass.h"

#define MT_FT_XLSX      0x01
#define MT_FT_COMTRADE  0x02
#define MT_FT_NONE      0x04

class OscDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OscDialog(EOscillogram *osc, QWidget *parent=0);
    ~OscDialog();

private:
    void SetupUI();
//    int InputFileType();

    ETableModel *tm;
//    int OscFileType;
//    bool AcceptedOscFileType;
    EOscillogram *OscFunc;

signals:

public slots:

private slots:
    void GetAndProcessOscInfo();
//    void ProcessOscInfo();
//    void EndExtractOsc(quint32 id, OscHeader_Data &OHD, const QString &fn); // id - ИД осциллограммы (по табл. Приложения 2 "АВ-ТУК. Файлы")
    void GetOsc(QModelIndex);
    void EraseOsc();
//    void Accept();
//    void Cancel();
};

#endif // OSCDIALOG_H
