#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "../gen/eoscillogram.h"
#include "../widgets/etablemodel.h"
#include "../publicclass.h"

#define MAXOSCBUFSIZE   262144 // максимальный размер буфера для осциллограмм

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

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
    struct GBoStruct
    {
        quint32 FileNum; // номер файла осциллограмм
        quint32 FileLength; // длина файла за исключением FileHeader (16 байт)
        quint32 ID; // Тип файла - осциллограмма и количество осциллограмм в файле (10000, 10001 ...)
        quint64 UnixTime; // Время начала записи осциллограммы
        quint32 IDo1; // ID первой осциллограммы в файле (определяет структуру точки и номер канала)
    };
#pragma pack(pop)   /* restore original alignment from stack */

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
