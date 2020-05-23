#include <QDateTime>
#include <QDir>
#define LZMA_API_STATIC
#include "lzma/lzma.h"

#include "log.h"
#include "stdfunc.h"
#include "error.h"

Log::Log(QObject *parent) : QObject(parent)
{
    fp = nullptr;
    mtx = new QMutex;
}

Log::~Log()
{
    if (fp != nullptr)
    {
        fp->flush();
        fp->close();
        delete fp;
    }
}

void Log::Init(const QString &Filename)
{
    LogFile = StdFunc::GetSystemHomeDir() + Filename;
    // тестовая проверка открытия файла на запись
    fp = new QFile(LogFile);
    if (!fp->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        CanLog = false;
        return;
    }
    CanLog = true;
}

void Log::error(const QString &str)
{
    if (CanLog)
        WriteFile("Error", str);
}

void Log::info(const QString &str)
{
    if (CanLog)
        WriteFile("Info", str);
}

void Log::warning(const QString &str)
{
    if (CanLog)
        WriteFile("Warning",str);
}

void Log::intvarvalue(const QString &var, int value)
{
    if (CanLog)
        WriteFile(var, QString::number(value));
}

void Log::WriteFile(const QString &Prepend, const QString &msg)
{
    QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "]";
    fp->write(tmps.toLocal8Bit());
    tmps = "["+Prepend+"] ";
    fp->write(tmps.toLocal8Bit());
    fp->write(msg.toLocal8Bit());
    fp->write("\n");
    fp->flush();
    CheckAndGz();
}

// thread-safe function

void Log::WriteRaw(const QByteArray &ba)
{
    QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "]";
    mtx->lock();
    fp->write(tmps.toLocal8Bit());
    fp->write(ba);
    fp->flush();
    CheckAndGz();
    mtx->unlock();
}

void Log::CheckAndGz()
{
    QString GZippedLogFile = LogFile;
    if (fp->size() >= LOG_MAX_SIZE)
    {
        int i;
        // rotating
        for (i=9; i>0; --i)
        {
            QString tmpsnew = GZippedLogFile + "." + QString::number(i) + ".xz";
            QString tmpsold = GZippedLogFile + "." + QString::number(i-1) + ".xz";
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
                    return;
                }
            }
        }
        GZippedLogFile += ".0.xz";
        // gzip log file and clearing current one
        lzma_stream strm = LZMA_STREAM_INIT;
        lzma_ret ret = lzma_easy_encoder(&strm, 6, LZMA_CHECK_CRC64);
        if (ret != LZMA_OK)
        {
            ERMSG("Something wrong with lzma_easy_encoder");
            return;
        }
        uint8_t inbuf[BUFSIZ];
        uint8_t outbuf[BUFSIZ];
        strm.next_in = NULL;
        strm.avail_in = 0;
        strm.next_out = outbuf;
        strm.avail_out = sizeof(outbuf);
        QFile fd, fr;
        fd.setFileName(LogFile);
        if (!fd.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ERMSG("Cannot open the file"+LogFile);
            return;
        }
        fr.setFileName(GZippedLogFile);
        if (!fr.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            ERMSG("Cannot open the file"+GZippedLogFile);
            return;
        }
        lzma_action action = LZMA_RUN;
        while (true)
        {
            // Fill the input buffer if it is empty.
            if ((strm.avail_in == 0) && !fd.atEnd())
            {
                strm.next_in = inbuf;
                strm.avail_in = fd.read(reinterpret_cast<char *>(&(inbuf[0])), sizeof(inbuf));
                if (fd.atEnd())
                    action = LZMA_FINISH;
            }
            lzma_ret ret = lzma_code(&strm, action);
            if ((strm.avail_out == 0) || (ret == LZMA_STREAM_END))
            {
                // When lzma_code() has returned LZMA_STREAM_END,
                // the output buffer is likely to be only partially
                // full. Calculate how much new data there is to
                // be written to the output file.
                size_t write_size = sizeof(outbuf) - strm.avail_out;
                size_t written_size = fr.write(reinterpret_cast<char *>(&(outbuf[0])), write_size);
                if (written_size != write_size)
                {
                    ERMSG("Write error");
                    return;
                }
                strm.next_out = outbuf;
                strm.avail_out = sizeof(outbuf);
            }
            if (ret != LZMA_OK)
            {
                // Once everything has been encoded successfully, the
                // return value of lzma_code() will be LZMA_STREAM_END.
                //
                // It is important to check for LZMA_STREAM_END. Do not
                // assume that getting ret != LZMA_OK would mean that
                // everything has gone well.
                if (ret == LZMA_STREAM_END)
                {
                    fr.flush();
                    fr.close();
                    fd.close();
                    fp->close();
                    fp->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
                    break;
                }
            }
        }
    }
}
