#ifndef FWUPDIALOG_H
#define FWUPDIALOG_H

#define MAXSIZE 2000000

#include "../gen/error.h"
#include "../widgets/udialog.h"

class FWUploadDialog : public UDialog
{
    Q_OBJECT
public:
    explicit FWUploadDialog(QWidget *parent = nullptr);

    void SetupUI();
    bool ParseHexToS2(QByteArray ba);
    bool WriteCheckPassword();

    quint32 NN;

    typedef struct
    {
        quint32 id;      // id
        quint32 NumByte; // количество байт в TypeTheData
    } DataRecHeader;

    typedef struct
    {
        quint16 fname;
        quint16 service;
        quint32 size;
        quint32 crc32;
        quint32 thetime;
    } FileHeader;

    typedef struct
    {
        DataRecHeader TypeHeader; // заголовок записи
        quint8 TypeTheData[4];
        quint8 VerPO[4];
    } DataRecT;

    typedef struct
    {
        DataRecHeader FileDatHeader; // заголовок записи
        QByteArray Data;
    } DataRecF;

#pragma pack(push, 1)
    typedef struct
    {
        FileHeader File_xxx_header;   // Заголовок файла (Прил. 1)
        DataRecT Type;                // Тип группы плат
        DataRecF File;                // Файл ВПО в формате BIN
        DataRecHeader void_recHeader; // заголовок пустой записи
    } File_struct;
#pragma pack(pop)

    QByteArray *BaForSend;
    int MainSize;
    bool ok;

signals:
    //    void WritePasswordChecked();

public slots:
    bool LoadFW();
    void RunSoft();
    //    void WritePasswordCheck(QString psw);
};

#endif // FWUPDIALOG_H
