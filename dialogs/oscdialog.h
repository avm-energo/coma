#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "../widgets/etablemodel.h"
#include "../publicclass.h"

#define MT_HEAD_ID      9000 // ID осциллограммы
#define MT_ID21         10001 // первый ID осциллограммы по модулю 21
#define MT_ID21E        10016 // последний ID осциллограммы по модулю 21
#define MT_ID81         10021 // ID осциллограммы по модулю 8081
#define MT_ID82         10022 // ID осциллограммы по модулю 8082
#define MT_ID83         10023 // ID осциллограммы по модулю 8083
#define MT_ID85         10030 // ID осциллограммы по модулю 8085

#define MAXOSCBUFSIZE   262144 // максимальный размер буфера для осциллограмм

#define MT_FT_XLSX      0x01
#define MT_FT_COMTRADE  0x02
#define MT_FT_NONE      0x04

class OscDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OscDialog(QWidget *parent=0);
    ~OscDialog();

private:
    void SetupUI();
    bool PosPlusPlus(void *dst, void *src, int size);
    int InputFileType();

    ETableModel *tm;
    QString GivenFilename;
    QString OscDateTime;
    int BASize, BSize, Pos;
    int OscFileType;
    bool AcceptedOscFileType;
    QByteArray BA;

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

    struct OscHeader_Data
    {
        quint64 unixtime; // время первой точки в Unix-формате
        float step; // шаг по времени в мс
        quint32 len; // длина осциллограммы в количестве точек по времени
    };

    OscHeader_Data OscHeader;

    struct Point85
    {
        quint32 Dis;
        float An[9];    // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
    };
                        // Dis:
                        // 31-29:	nOx – НО блок-контакты полюсов А, В, С (нефильтр.)
                        // 28-26:	nCx – НЗ блок-контакты полюсов А, В, С (нефильтр.)
                        // 25-23:	nFx – оперативное управление отключением полюсов А, В, С (нефильтр.)
                        // 22-20:	nNx – оперативное управление включением полюсов А, В, С (нефильтр.)
                        // 19-17:	CFx – выходные команды на отключение полюсов А, В, С
                        // 16-14:	CNx – выходные команды на включение полюсов А, В, С
                        // 13:      CSC – контроль цепи соленоида отключения
                        // 12:      CSO – контроль цепи соленоида включения
                        // 11-9:	BKOx – НО блок-контакты полюсов А, В, С
                        // 8-6:     BKCx – НЗ блок-контакты полюсов А, В, С
                        // 5-3:     OCFx – оперативное управление отключением полюсов А, В, С
                        // 2-0:     OCNx – оперативное управление включением полюсов А, В, С

    struct Point21
    {
        float An;
    };

    struct Point8x
    {
        float U1[3];
        float U2[3];
    };

    struct OscDataRec
    {
        quint32 id;
        quint32 numbytes;
    };

#pragma pack(pop)   /* restore original alignment from stack */

signals:

private slots:
    void GetAndProcessOscInfo();
//    void ProcessOscInfo();
    void EndExtractOsc(quint32 id, OscHeader_Data &OHD); // id - ИД осциллограммы (по табл. Приложения 2 "АВ-ТУК. Файлы")
    void GetOsc(QModelIndex);
    void EraseOsc();
    void Accept();
    void Cancel();
};

#endif // OSCDIALOG_H
