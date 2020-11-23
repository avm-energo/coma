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
    /*!
    Приложение 3. Номера файлов

    № файла	Описание
        1	Конфигурация
        3	Встроенное ПО (Firmware)
            4	Системный журнал
        5	Рабочий журнал
        6	Журнал измерений
        17	Журнал переключений
            */
    enum FilesEnum
    {
        Config = 1,
        Firmware = 3,
        JourSys = 4,
        JourWork = 5,
        JourMeas = 6,
        JourSw = 17,
        JourEv = 18,   // events journal (12->62)
        FileOsc = 1000 // oscilloscope info
    };
    Files();
#ifdef QT_GUI_LIB
    static QString ChooseFileForOpen(QWidget *parent, QString mask);
    static Error::Msg LoadFromFile(const QString &filename, QByteArray &ba);
    static QString ChooseFileForSave(
        QWidget *parent, const QString &mask, const QString &ext, const QString &filenamestr = "");
    static Error::Msg SaveToFile(const QString &filename, QByteArray &src);
    static QStringList Drives();
    static QStringList SearchForFile(QStringList &di, const QString &filename, bool subdirs = false);
    static QString GetFirstDriveWithLabel(QStringList &filepaths, const QString &label);
#endif
};

#endif // FILES_H
