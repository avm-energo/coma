#pragma once

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
    ResEmpty = -2, // WARNING ResEmpty hardcoded as -2
    GeneralError,  // WARNING GeneralError hardcoded as -1
    NoError,       // WARNING NoError hardcoded as 0
    WrongCommandError,
    SizeError,
    DescError,
    CrcError,
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
    FileNameError
};

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
