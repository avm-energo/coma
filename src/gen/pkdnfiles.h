#pragma once

#include <QByteArray>
#include <QString>
#include <QWidget>

class PkdnFiles
{
public:
    enum FileErrorTypes
    {
        ER_NOERROR,     // нет ошибок
        ER_FILEWRITE,   // ошибка записи файла
        ER_FILENAMEEMP, // пустое имя файла
        ER_FILEOPEN     // ошибка открытия файла
    };

    PkdnFiles();

    static QString ChooseFileForOpen(QWidget *parent, QString mask);
    static int LoadFromFile(const QString &filename, QByteArray &ba);
    static QString ChooseFileForSave(QWidget *parent, const QString &mask, const QString &ext);
    static int SaveToFile(const QString &filename, QByteArray &src, unsigned int numbytes);
};
