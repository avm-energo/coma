#include "filehelper.h"

#include "board.h"

#include <gen/files.h>
#include <gen/stdfunc.h>

QString FileHelper::ChooseFileForSave(const QString &ext)
{
    const auto &board = Board::GetInstance();
    QString MTypeM = (board.typeM() == 0) ? "00" : QString::number(board.typeM(), 16);
    QString res = StdFunc::GetHomeDir() + "/" + QString::number(board.typeB(), 16) + MTypeM + "-"
        + QString("%1").arg(board.serialNumber(Board::BaseMezz), 8, 10, QChar('0')) + "." + ext;
    return res;
}

bool FileHelper::isFileExist(const QString &filename)
{
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    return QFile::exists(dir.filePath(filename));
}

QDomDocument FileHelper::getFileContent(const QString &filename)
{
    QDomDocument doc;
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    auto file = new QFile(dir.filePath(filename));
    if (file->exists())
    {
        if (file->open(QIODevice::ReadOnly))
        {
            QString errMsg = "";
            auto line = 0, column = 0;
            if (!doc.setContent(file, &errMsg, &line, &column))
            {
                qCritical() << Error::WrongFileError << file->fileName();
                qCritical() << errMsg << " Line: " << line << " Column: " << column;
            }
            file->close();
        }
        else
            qCritical() << Error::FileOpenError << file->fileName();
    }
    else
        qCritical() << Error::DescError << file->fileName();

    file->deleteLater();
    return doc;
}
