#ifndef PUBLICCLASS_H
#define PUBLICCLASS_H

#define Z   64 // количество точек, по которым выдаются значения в блоке Bda

// определение типов модулей
#define MT_C    1
#define MT_D    2
#define MT_A    3
#define MT_E    4

// определение подтипов модулей
#define MTE_2T0N    0
#define MTE_1T1N    1
#define MTE_0T2N    2

// S2: Определение типов элементов данных
#define u8_TYPE		1  //беззнаковый байт
#define s8_TYPE		2  //знаковый байт
#define u16_TYPE	3  //беззнаковое слово
#define s16_TYPE	4	//знаковое слово
#define u32_TYPE	5  //32-х битовое беззнаковое
#define s32_TYPE	6  //32-х битовое знаковое
#define u64_TYPE	7  //64-х битовое беззнаковое
#define s64_TYPE	8  //64-х битовое знаковое
#define float_TYPE	9  //32-х битовое плавающее
#define double_TYPE	10 //64-х битовое плавающее
#define string_TYPE	11 //строка, ограниченная нулём
#define binary_TYPE	12 //любые двоичные данные

// Канал связи с модулем: определение ошибок
#define CN_OK               0 // ошибок нет
#define CN_PORTOPENERROR    1 // ошибка открытия порта
#define CN_NOPORTSERROR     2 // нет портов в системе
#define CN_SEGFAULTERROR    3 // ошибка при приёме сегмента данных на стороне модуля
#define CN_RCVDATAERROR     4 // ошибка при приёме данных (несовпадение длины, неправильный начальный символ и т.п.)
#define CN_TIMEOUTERROR     5 // таймаут при приёме данных
#define CN_RCVLENGTHERROR   6 // некорректная длина блока (принято больше, чем может вместить в себя приёмная переменная)
#define CN_UNKNOWNCMDERROR  7 // неизвестная команда
#define CN_S2SIZEERROR      8 // ошибка длины при работе с форматом S2
#define CN_S2DESCERROR      9 // несовпадение описания прочитанного элемента с ожидаемым при работе с форматом S2
#define CN_S2CRCERROR       10 // несовпадение контрольной суммы при работе с форматом S2
#define CN_S2DHSZERROR      11 // некорректная длина в DataHeader при разборе формата S2
#define CN_NULLDATAERROR    12 // в канальную процедуру переданы некорректные данные

#define ER_FILEOPENERROR    15 // ошибка при открытии файла
#define ER_NOCONF           20 // нет конфигурации в модуле
#define ER_NOTUNECOEF       21 // нет настроечных параметров в модуле

#define HTH_CONFIG          0x00000080 // в модуле нет конфигурации
#define HTH_REGPARS         0x00000040 // в модуле нет настроечных параметров
#define HTH_ADC             0x00000020 // проблема с АЦП
#define HTH_1PPS            0x00000010 // нет сигнала 1PPS
#define HTH_REL             0x00000008 // неисправность выходных реле
#define HTH_TUPP            0x00000004 // перегрев модуля

#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include "serialthread.h"

class publicclass
{
public:
    publicclass();

    QSerialPort serial;
    qint32 MType, MType1, SerNum;
    quint32 CpuIdHigh, CpuIdMid, CpuIdLow;

    QStringList AMTypes, DMTypes, EMTypes;
    SerialThread *SThread;
    quint16 MIPASDU;
    QString MIPIP;

    // S2: Определение типа заголовка
    typedef struct
    {
      quint32 size;
      quint32 crc32;
      quint32 thetime;
    } DataHeader;

    // S2: Определение типа записи

    typedef struct DataRec
    {
      quint16 id;
      quint16 data_type;
      quint32 elem_size;
      quint32 num_elem;
      void *thedata;
    } DataRec;

    typedef struct
    {
        qint32 MType;
        qint32 MType1;
        qint32 HWver;
        qint32 FWver;
        qint32 Cfcrc;
        qint32 Rst;
        qint32 RstCount;
        qint32 Hth;
        qint32 CpuIdLow;
        qint32 CpuIdMid;
        qint32 CpuIdHigh;
        qint32 SerNum;
    } Bsi;

    QString VerToStr(quint32);
    quint32 ANumD();
    quint32 ANumCh1();
    quint32 ANumCh2();
    quint32 ATyp1();
    quint32 ATyp2();
    quint32 AMdf();
    QString ETyp1();

    // S2: Сборщик в память:
    int StoreDataMem(void *, DataRec *); //0 - успешно, иначе код ошибки
    // S2: получение размера:
    int StoreDataSize(DataHeader *, DataRec *); //>0 - успешно, иначе код ошибки
    // S2: Разборщик из памяти:
    int RestoreDataMem(void *, quint32, DataRec *); //0 - успешно, иначе код ошибки
    // S2: Поиск элемента в массиве описаний
    DataRec *FindElem(DataRec *, quint16);

    quint32 GetCRC32(char *, quint32);

    void updCRC32(const quint8 byte, quint32 *dwCRC32);

    quint32 getTime32();
private:

};

extern publicclass pc;
extern const QStringList errmsgs;

#endif // PUBLICCLASS_H
