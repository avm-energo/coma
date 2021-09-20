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
    void loadOsc(std::unique_ptr<TrendViewModel> &&model);
    std::unique_ptr<TrendViewModel> load(const OscHeader &header, const FileStruct &fs);

    void loadSwjFromFile(const QString &filename);
    QString generateFilename(quint32 id, quint64 timestamp);

private:
    UniquePointer<TrendViewDialog> trendDialog;

    const QStringList phases { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
};
