#pragma once

#include <QDebug>
#include <QMap>
#include <QObject>
#include <QString>

// Макросы для выдачи сообщений
#define ERMSG qCritical
#define DBGMSG qDebug
#define INFOMSG qInfo
#define WARNMSG qWarning

/// \brief Namespace for critical message logging
namespace Error
{
Q_NAMESPACE

/// Enumeration for possible errors in program runtime.
enum Msg
{
    ResEmpty = -2,            ///< \warning ResEmpty hardcoded as -2
    GeneralError = -1,        ///< \warning GeneralError hardcoded as -1
    NoError = 0,              ///< \warning NoError hardcoded as 0
    Timeout = 0x01,           ///< Others hardcoded as Protocom
    CrcError = 0x02,          ///< Test01
    FlashError = 0x03,        ///< Test02
    SizeError = 0x04,         ///< Test03
    WrongCommandError = 0x05, ///< Test04
    WrongFormatError = 0x06,  ///< Test05
    UnknownBlock = 0x07,      ///< Test06
    UnknownFileReq = 0x08,    ///< Test07
    UnknownFileSent = 0x09,   ///< Test08
    NoMezzanine = 0x0a,       ///< Test09
    WrongType = 0x0b,         ///< Test10
    FileNameError = 0x0c,     ///< Test11
    WrongBlockAC = 0x0d,      ///< Test12
    DescError,                ///< Test13
    HeaderSizeError,          ///< Test14
    NoIdError,                ///< Test15
    NoConfError,              ///< Test16
    NoTuneError,              ///< Test17
    WriteError,               ///< Test18
    NoDeviceError,            ///< Test19
    ReadError,                ///< Test20
    OpenError,                ///< Test21
    NullDataError,            ///< Test22
    WrongFileError,           ///< Test23
    FileOpenError,            ///< Test24
    FileWriteError,           ///< Test25
    UnknownError = 0xff       ///< Test26
};

/// \brief Map with keys as Msg and values as QString with description of error.
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

/// \brief Structure for storage error message data.
struct ErrorMsg
{
    QString DateTime; ///< Time and date of error happens
    QtMsgType type;   ///< Type of message for logging
    QString file;     ///< File with error
    int line;         ///< Line with error in file
    QString msg;      ///< String with message about error
};

/// \brief Output to string list operator overloading.
QStringList &operator<<(QStringList &l, const ErrorMsg &obj);

/// \brief Output to QDebug (stdout) operator overloading.
QDebug operator<<(QDebug debug, const ErrorMsg &error);
