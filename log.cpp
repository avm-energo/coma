/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#define LZMA_API_STATIC
#include <lzma/lzma.h>

#include "log.h"
#include "publicclass.h"

Log::Log(QObject *parent) : QObject(parent)
{
    fp = 0;
}

Log::~Log()
{
    if (fp != 0)
    {
        fp->flush();
        fp->close();
        delete fp;
    }
}

void Log::Init(const QString &Filename)
{
/*    QString LogPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/log";
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(LogPath);
    if (!dir.exists())
        dir.mkpath(".");
    LogFile = LogPath + "/" + Filename; */
    LogFile = pc.HomeDir + "/log/" + Filename;
    // тестовая проверка открытия файла на запись
    fp = new QFile(LogFile);
    if (!fp->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        ERMSG("Невозможно создать log-файл: "+LogFile);
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
    QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") + "]";
    fp->write(tmps.toLocal8Bit());
    tmps = "["+Prepend+"] ";
    fp->write(tmps.toLocal8Bit());
    fp->write(msg.toLocal8Bit());
    fp->write("\n");
    fp->flush();
    CheckAndGz();
}

void Log::Info(QByteArray &ba)
{
    QString tmps = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss") + "]";
    fp->write(tmps.toLocal8Bit());
    tmps = "[Info] ";
    fp->write(tmps.toLocal8Bit());
    fp->write(ba);
    fp->write("\n");
    fp->flush();
    CheckAndGz();
}

void Log::CheckAndGz()
{
    QString GZippedLogFile = LogFile + ".xz";
    if (fp->size() >= LOG_MAX_SIZE)
    {
        int i;
        // rotating
        for (i=9; i>0; --i)
        {
            QString tmpsnew = GZippedLogFile + "." + QString::number(i);
            QString tmpsold = GZippedLogFile + "." + QString::number(i-1);
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
        GZippedLogFile += ".0";
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
