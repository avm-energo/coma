#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "../widgets/etablemodel.h"
#include "../publicclass.h"

#define MT_HEAD_ID      1000 // ID шапки осциллограммы равен 1000
#define MT_START_OSC    1001 // первый ID осциллограммы
#define MT_END_OSC      1033 // последний ID осциллограммы

#define MAXOSCBUFSIZE   262144 // максимальный размер буфера для осциллограмм

class oscdialog : public QDialog
{
    Q_OBJECT

public:
    explicit oscdialog(QWidget *parent=0);
    ~oscdialog();

private:
    void SetupUI();

    ETableModel *tm;
    QString GivenFilename;
    QString OscDateTime;
    publicclass::DataRec Config[32];

    struct GBoStruct
    {
        quint32 FileNum; // номер файла осциллограмм
        quint32 FileLength; // длина файла за исключением FileHeader (12 байт)
        quint32 ID; // Тип файла - осциллограмма и количество осциллограмм в файле (10000, 10001 ...)
        quint64 UnixTime; // Время начала записи осциллограммы
        quint32 IDo1; // ID первой осциллограммы в файле (определяет структуру точки и номер канала)
    };

    struct Point85
    {
        quint32 Discr;
        float Analog[9];
    };

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
    void GetAndProcessOscInfo();
//    void ProcessOscInfo();
    void EndExtractOsc();
    void GetOsc(QModelIndex);
    void EraseOsc();
};

#endif // OSCDIALOG_H
