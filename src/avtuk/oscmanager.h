#pragma once
#include "../gen/stdfunc.h"
#include "filemanager.h"
#include "trendviewdialog.h"

#include <QObject>

class TrendViewDialog;

class OscManager : public FileManager
{
public:
    OscManager() = default;
    OscManager(const QByteArray &ba);
    void loadOscFromFile(const QString &filename);
    void loadOsc(const QByteArray &buffer);
    QVariant load(const DataTypes::FileStruct &fs) override;
    void loadSwjFromFile(const QString &filename);
    QString generateFilename(quint32 id, quint64 timestamp);

private:
    UniquePointer<TrendViewDialog> trendDialog;

    const QStringList phases { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
};
