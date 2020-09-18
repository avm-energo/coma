#include "logger.h"

#include "stdfunc.h"

#define LZMA_API_STATIC
#ifdef _WIN32
#include "lzma/lzma.h"
#endif
#ifdef __linux__
#include "lzma.h"
#endif

#define LOG_MAX_SIZE 1048576
void Logging::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QStringList buffer = QString(context.file).split("\\");
    QString sourceFile;
    QString folderName;
    if (!buffer.isEmpty())
    {
        sourceFile = buffer.takeLast();
        if (!buffer.isEmpty())
            folderName = buffer.last();
    }

    QString fileName;
    QStringList folderList { "alarm", "check", "config", "dialogs", "gen", "iec104", "modbus", "models", "usb",
        "widgets" };
    QFile logFile;
    if (folderName.isEmpty() || !folderList.contains(folderName))
        fileName = ("log.txt");
    else
        fileName = (folderName + ".txt");

    QTextStream out;

    // Detect type of msg
    // qWarning && qDebug пишем в одно место и удаляем их перед каждым запуском
    switch (type)
    {
    case QtInfoMsg:
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    case QtDebugMsg:
        break;
    case QtWarningMsg:
        break;
    case QtCriticalMsg:
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    case QtFatalMsg:
        fileName.prepend(StdFunc::GetSystemHomeDir());
        break;
    }
    logFile.setFileName(fileName);
    out.setDevice(&logFile);
    logFile.open(QFile::Append | QFile::Text);
    // Log datetime
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    // Write category and msg
    out << context.category << " " << sourceFile << ": " << msg << Qt::endl;

    out.flush(); // Flush buffer
    logFile.close();
    checkNGzip(fileName);
}

void Logging::checkNGzip(QString &fileName)
{
    QSharedPointer<QFile> logFile = QSharedPointer<QFile>(new QFile(fileName));
    QString GZippedLogFile = fileName;
    if (fileName.size() >= LOG_MAX_SIZE)
    {
        int i;
        // rotating
        for (i = 9; i > 0; --i)
        {
            QString tmpsnew = GZippedLogFile + "." + QString::number(i) + ".xz";
            QString tmpsold = GZippedLogFile + "." + QString::number(i - 1) + ".xz";
            QFile fn;
            fn.setFileName(tmpsnew);
            if (fn.exists())
                fn.remove();
            fn.setFileName(tmpsold);
            if (fn.exists())
            {
                if (fn.rename(tmpsnew) == false) // error
                {
                    qCritical("Cannot rename file");
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
            qCritical("Something wrong with lzma_easy_encoder");
            return;
        }
        uint8_t inbuf[BUFSIZ];
        uint8_t outbuf[BUFSIZ];
        strm.next_in = NULL;
        strm.avail_in = 0;
        strm.next_out = outbuf;
        strm.avail_out = sizeof(outbuf);
        QFile fr;
        // fd.setFileName(fileName);
        if (!logFile->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qCritical() << "Cannot open the file" << fileName;
            return;
        }
        fr.setFileName(GZippedLogFile);
        if (!fr.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            qCritical() << "Cannot open the file" << GZippedLogFile;
            return;
        }
        lzma_action action = LZMA_RUN;
        while (true)
        {
            // Fill the input buffer if it is empty.
            if ((strm.avail_in == 0) && !logFile->atEnd())
            {
                strm.next_in = inbuf;
                strm.avail_in = logFile->read(reinterpret_cast<char *>(&(inbuf[0])), sizeof(inbuf));
                if (logFile->atEnd())
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
                    qCritical("Write error");
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
                    logFile->close();
                    logFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
                    break;
                }
            }
        }
    }
}

Q_LOGGING_CATEGORY(logDebug, "Debug")
Q_LOGGING_CATEGORY(logInfo, "Info")
Q_LOGGING_CATEGORY(logWarning, "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")
