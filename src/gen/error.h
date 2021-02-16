#pragma once

#include <QMap>
#include <QObject>
#include <QString>
// Макросы для выдачи сообщений
#define ERMSG qCritical
#define DBGMSG qDebug
#define INFOMSG qInfo
#define WARNMSG qWarning

namespace Error
{
Q_NAMESPACE

enum Msg
{
    ResEmpty = -2,     // WARNING ResEmpty hardcoded as -2
    GeneralError = -1, // WARNING GeneralError hardcoded as -1
    NoError = 0,       // WARNING NoError hardcoded as 0
    Timeout = 0x01,    // Others hardcoded as Protocom
    CrcError = 0x02,
    FlashError = 0x03,
    SizeError = 0x04,
    WrongCommandError = 0x05,
    WrongFormatError = 0x06,
    UnknownBlock = 0x07,
    UnknownFileReq = 0x08,
    UnknownFileSent = 0x09,
    NoMezzanine = 0x0a,
    WrongType = 0x0b,
    FileNameError = 0x0c,
    WrongBlockAC = 0x0d,
    DescError,
    HeaderSizeError,
    NoIdError,
    NoConfError,
    NoTuneError,
    WriteError,
    NoDeviceError,
    ReadError,
    OpenError,
    NullDataError,
    WrongFileError,
    FileOpenError,
    FileWriteError,
    UnknownError = 0xff

};

const QMap<Msg, QString> MsgStr { { ResEmpty, "Пустой результат" }, { GeneralError, "Ошибка" }, { NoError, "Успешно" },
    { Timeout, "Таймаут операции" }, { CrcError, "Ошибка контрольной суммы" },
    { FlashError, "Ошибка записи во Flash-память" }, { SizeError, "Ошибка размера данных" },
    { WrongCommandError, "Некорректная команда" }, { WrongFormatError, "Формат неверен" },
    { UnknownBlock, "Неизвестный блок данных" }, { UnknownFileReq, "Запрошен неизвестный файл" },
    { UnknownFileSent, "Отправлен неизвестный файл" }, { NoMezzanine, "Нет мезонина" },
    { WrongType, "Неверный тип платы" }, { FileNameError, "Нет такого файла" }, { WrongBlockAC, "Ошибка блока Bac" },
    { DescError, "Ошибка описания S2" }, { HeaderSizeError, "Неверный размер заголовка S2" },
    { NoIdError, "Нет такого ИД S2" }, { NoConfError, "Нет конфигурации" },
    { NoTuneError, "Нет настроечных параметров" }, { WriteError, "Ошибка записи" },
    { NoDeviceError, "Нет такого устройства" }, { ReadError, "Ошибка чтения" }, { OpenError, "Ошибка открытия порта" },
    { NullDataError, "Приняты нулевые данные" }, { FileNameError, "Ошибка номера файла" },
    { FileOpenError, "Ошибка открытия файла" }, { FileWriteError, "Ошибка записи файла" },
    { UnknownError, "Неизвестная ошибка" } };

Q_ENUM_NS(Msg)

}

struct ErrorMsg
{
    QString DateTime;
    QtMsgType type;
    QString file;
    int line;
    QString msg;
};

QStringList &operator<<(QStringList &l, const ErrorMsg &obj);
QDebug operator<<(QDebug debug, const ErrorMsg &error);
