#ifndef FILES_H
#define FILES_H

#include <QWidget>
#include <QString>
#include <QByteArray>

class Files
{
public:
    enum FileErrorTypes
    {
        ER_NOERROR, // нет ошибок
        ER_FILEWRITE, // ошибка записи файла
        ER_FILENAMEEMP, // пустое имя файла
        ER_FILEOPEN // ошибка открытия файла
    };

    Files();

    static QString ChooseFileForOpen(QWidget *parent, QString mask);
    static int LoadFromFile(const QString &filename, QByteArray &ba);
    static QString ChooseFileForSave(QWidget *parent, const QString &mask, const QString &ext);
    static int SaveToFile(const QString &filename, QByteArray &src, unsigned int numbytes);
};

#endif // FILES_H
