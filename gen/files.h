#ifndef FILES_H
#define FILES_H
#include "../gen/error.h"

#include <QByteArray>
#include <QString>
#include <QWidget>

class Files
{
public:
    enum FileErrorTypes
    {
        ER_NOERROR,     // нет ошибок
        ER_FILEWRITE,   // ошибка записи файла
        ER_FILENAMEEMP, // пустое имя файла
        ER_FILEOPEN     // ошибка открытия файла
    };

    Files();

    static QString ChooseFileForOpen(QWidget *parent, QString mask);
    static Error::Msg LoadFromFile(const QString &filename, QByteArray &ba);
    static QString ChooseFileForSave(
        QWidget *parent, const QString &mask, const QString &ext, const QString &filenamestr = "");
    static Error::Msg SaveToFile(const QString &filename, QByteArray &src, unsigned int numbytes);
    static QStringList Drives();
    static QStringList SearchForFile(QStringList &di, const QString &filename, bool subdirs = false);
    static QString GetFirstDriveWithLabel(QStringList &filepaths, const QString &label);
};

#endif // FILES_H
