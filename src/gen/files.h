#ifndef FILES_H
#define FILES_H
#include "../gen/error.h"

#include <QString>

class Files
{
public:
    Files();

    static QString ChooseFileForSave(const QString &ext, const QString &filenamestr = "");

    static Error::Msg LoadFromFile(const QString &filename, QByteArray &ba);

    static Error::Msg SaveToFile(const QString &filename, const QByteArray &src);
    static QStringList Drives();
    static QStringList SearchForFile(QStringList &di, const QString &filename, bool subdirs = false);
    static QString GetFirstDriveWithLabel(QStringList &filepaths, const QString &label);
};

#endif // FILES_H
