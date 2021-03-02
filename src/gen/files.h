#ifndef FILES_H
#define FILES_H
#include "../gen/error.h"

#include <QString>
#ifdef QT_GUI_LIB
#include <QWidget>
#endif
class Files
{
public:
    Files();
#ifdef QT_GUI_LIB
    static QString ChooseFileForOpen(QWidget *parent, QString mask);
    static QString ChooseFileForSave(
        QWidget *parent, const QString &mask, const QString &ext, const QString &filenamestr = "");
#endif
    static Error::Msg LoadFromFile(const QString &filename, QByteArray &ba);

    static Error::Msg SaveToFile(const QString &filename, QByteArray &src);
    static QStringList Drives();
    static QStringList SearchForFile(QStringList &di, const QString &filename, bool subdirs = false);
    static QString GetFirstDriveWithLabel(QStringList &filepaths, const QString &label);
};

#endif // FILES_H
