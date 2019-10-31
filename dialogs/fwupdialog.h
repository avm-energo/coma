#ifndef FWUPDIALOG_H
#define FWUPDIALOG_H

#include <QDialog>

class fwupdialog : public QDialog
{
    Q_OBJECT
public:
    explicit fwupdialog(QWidget *parent = nullptr);

    void SetupUI();
    int ParseHexToS2(QByteArray ba);

    quint32 NN;

    typedef struct
    {
      quint32 id; 		// id
      quint32 NumByte;			// количество байт в TypeTheData
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
      DataRecHeader TypeHeader;			// заголовок записи
      quint8 TypeTheData[4];
      quint8 VerPO[4];
    }DataRecT;

    typedef struct
    {
      DataRecHeader FileDatHeader;			// заголовок записи
      QByteArray Data;
    }DataRecF;

    typedef struct
    {
      FileHeader	File_xxx_header;	// Заголовок файла (Прил. 1)
      DataRecT		Type;		   	// Тип группы плат
      DataRecF		File;		   	// Файл ВПО в формате BIN
      DataRecHeader	void_recHeader;	// заголовок пустой записи
    }File_struct;

    QByteArray BaForSend;

signals:

public slots:
    int LoadFW();

};

#endif // FWUPDIALOG_H
