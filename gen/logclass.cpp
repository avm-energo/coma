#include "logclass.h"

#include "s2.h"
#include "stdfunc.h"

#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QMutexLocker>

LogClass::LogClass(QObject *parent) : QObject(parent)
{
    fp = nullptr;
    mtx = new QMutex;
}

LogClass::~LogClass()
{
    if (fp != nullptr)
    {
        fp->flush();
        fp->close();
        delete fp;
    }
}

void LogClass::Init(const QString &Filename)
{
    LogFile = StdFunc::GetSystemHomeDir() + Filename;
    // тестовая проверка открытия файла на запись
    fp = new QFile(LogFile);
    if (!fp->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        CanLog = false;
        ERMSG("Ошибка открытия файла");
        return;
    }
    CanLog = true;
}

void LogClass::error(const QString &str)
{
    if (CanLog)
        WriteFile("Error", str);
}

void LogClass::info(const QString &str)
{
    if (CanLog)
        WriteFile("Info", str);
}

void LogClass::warning(const QString &str)
{
    if (CanLog)
        WriteFile("Warning", str);
}

void LogClass::intvarvalue(const QString &var, int value)
{
    if (CanLog)
        WriteFile(var, QString::number(value));
}

void LogClass::WriteFile(const QString &Prepend, const QString &msg)
{
    if (fp != nullptr)
    {
        QMutexLocker locker(mtx);
        QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "]";
        fp->write(tmps.toLocal8Bit());
        tmps = "[" + Prepend + "] ";
        fp->write(tmps.toLocal8Bit());
        fp->write(msg.toLocal8Bit());
        fp->write("\n");
        fp->flush();
        CheckAndCompress();
    }
}

// thread-safe function

void LogClass::WriteRaw(const QByteArray &ba)
{
    if (fp != nullptr)
    {
        QMutexLocker locker(mtx);
        QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "]";
        int writtenSize;
        writtenSize = fp->write(tmps.toLocal8Bit());
        if (writtenSize == -1)
            return;
        writtenSize = fp->write(ba);
        if (writtenSize == -1)
            return;
        if (!fp->flush())
            return;
        CheckAndCompress();
    }
}

bool check(const QString &path)
{
    // rotating
    for (int i = 9; i > 0; --i)
    {
        QString tmpsnew = path + "." + QString::number(i) + ".gz";
        QString tmpsold = path + "." + QString::number(i - 1) + ".gz";
        QFile fn;
        fn.setFileName(tmpsnew);
        if (fn.exists())
            fn.remove();
        fn.setFileName(tmpsold);
        if (fn.exists())
        {
            if (fn.rename(tmpsnew) == false) // error
            {
                ERMSG("Cannot rename file");
                return false;
            }
        }
    }
    return true;
}

QByteArray compress(const QByteArray &data)
{
    // Compress the buffer (using zlib) in a compression rate at 9
    auto compressedData = qCompress(data, 9);
    //  Strip the first six bytes (a 4-byte length put on by qCompress and a 2-byte zlib header)
    // (see RFC 1950)
    compressedData.remove(0, 6);
    // Remove ADLER-32
    compressedData.chop(4);

    QByteArray header;
    QDataStream headerStream(&header, QIODevice::WriteOnly);
    // (see RFC 1952)
    /* Prepend |ID1|ID2|CM|FLG|
    ID1 (IDentification 1)
    ID2 (IDentification 2)
    These have the fixed values ID1 = 31 (0x1f, \037), ID2 = 139
    (0x8b, \213), to identify the file as being in gzip format.
    CM (Compression Method)
    This identifies the compression method used in the file.
    CM = 8 denotes the "deflate" compression method.
    FLG (FLaGs) do not used here so 0*/
    headerStream << quint16(0x1f8b) << quint16(0x0800);
    headerStream.setByteOrder(QDataStream::LittleEndian);
    /*
     * MTIME (Modification TIME)
    This gives the most recent modification time of the original
    file being compressed.  The time is in Unix format, i.e.,
    seconds since 00:00:00 GMT, Jan.  1, 1970.  (Note that this
    may cause problems for MS-DOS and other systems that use
    local rather than Universal time.)  If the compressed data
    did not come from a file, MTIME is set to the time at which
    compression started.  MTIME = 0 means no time stamp is
    available.
    */
    // Current time
    headerStream << S2::getTime32();
    headerStream.setByteOrder(QDataStream::BigEndian);
    //  |XFL|OS|
    // XFL = 0x02 as maximum compression used
    headerStream << quint16(0x0200);
    QByteArray footer;
    QDataStream footerStream(&footer, QIODevice::WriteOnly);
    footerStream.setByteOrder(QDataStream::LittleEndian);
    footerStream << quint32(S2::crc32buf(data)) // Append a four-byte CRC-32 of the uncompressed data
                 << quint32(data.size());       // Append 4 bytes uncompressed input size modulo 2^32
    return header + compressedData + footer;
}

void LogClass::CheckAndCompress()
{
    QString GZippedLogFile = LogFile;
    if (fp->size() < LOG_MAX_SIZE)
        return;
    if (!check(GZippedLogFile))
        return;
    GZippedLogFile += ".0.gz";

    QFile fileIn, fileOut;
    fileIn.setFileName(LogFile);
    if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ERMSG("Cannot open the file" + LogFile);
        return;
    }
    fileOut.setFileName(GZippedLogFile);
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        ERMSG("Cannot open the file" + GZippedLogFile);
        return;
    }
    fp->close();

    fileOut.write(compress(fileIn.readAll()));
    fileIn.close();
    fileOut.close();
    fp->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
}
