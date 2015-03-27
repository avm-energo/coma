#ifndef PUBLICCLASS_H
#define PUBLICCLASS_H

#define MT_C    1
#define MT_D    2
#define MT_A    3
#define MT_E    4

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

#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include "serialthread.h"

class publicclass
{
public:
    publicclass();

    QSerialPort serial;
    qint32 MType, MType1;
    QStringList AMTypes, DMTypes, EMTypes;
    SerialThread *SThread;

    // S2: Определение типа заголовка
    typedef struct
    {
      qint32 size;
      qint32 crc32;
      qint32 thetime;
    } DataHeader;

    // S2: Определение типа записи

    typedef struct
    {
      qint16 id;
      qint16 data_type;
      qint32 elem_size;
      qint32 num_elem;
      void *thedata;
    } DataRec;

    QString VerToStr(qint32);
    qint32 ANumD();
    qint32 ANumCh1();
    qint32 ANumCh2();
    qint32 ATyp1();
    qint32 ATyp2();
    qint32 AMdf();

    // S2: Сборщик в память:
    int StoreDataMem(void *, DataRec *); //0 - успешно, иначе код ошибки
    // S2: получение размера:
    int StoreDataSize(DataHeader *, DataRec *); //>0 - успешно, иначе код ошибки
    // S2: Разборщик из памяти:
    int RestoreDataMem(void *, DataRec *); //0 - успешно, иначе код ошибки
    // S2: Поиск элемента в массиве описаний
    DataRec *FindElem(DataRec *, qint16);

    qint32 GetCRC32(char *, qint32);

    void updCRC32(const qint8 byte, qint32 *dwCRC32);

    qint32 getTime32();
private:

};

extern publicclass pc;

#endif // PUBLICCLASS_H
