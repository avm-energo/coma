#include "tunesequencefile.h"

#include "../gen/board.h"
#include "../gen/stdfunc.h"

#include <QtDebug>

std::unique_ptr<QSettings> TuneSequenceFile::s_tsSettings;
QString TuneSequenceFile::s_cpuSerialNum;
bool TuneSequenceFile::s_tsfInitialized = false;

TuneSequenceFile::TuneSequenceFile()
{
}

void TuneSequenceFile::init()
{
    const QString tsFilename = StdFunc::GetSystemHomeDir() + TS_FILENAME;
    s_tsSettings = std::unique_ptr<QSettings>(new QSettings(tsFilename, QSettings::IniFormat));
    s_cpuSerialNum = Board::GetInstance().UID();
    s_tsfInitialized = true;
    saveTuneSequenceFile(1); // if there's no any steps for this module force writing it
}

void TuneSequenceFile::saveTuneSequenceFile(int step)
{
    if (!isInitialized())
        return;
    int calibrstep = s_tsSettings->value(s_cpuSerialNum + "/step", "0").toInt();
    if (step > calibrstep)
        s_tsSettings->setValue(s_cpuSerialNum + "/step", step);
}

QVariant TuneSequenceFile::value(const QString &key, const QVariant &defaultvalue)
{
    if (!isInitialized())
        return QVariant();
    if (contains(key))
        return s_tsSettings->value(s_cpuSerialNum + "/" + key);
    return defaultvalue;
}

bool TuneSequenceFile::contains(const QString &key)
{
    if (!isInitialized())
        return false;
    QString keytocheck = s_cpuSerialNum + "/" + key;
    return s_tsSettings->contains(keytocheck);
}

void TuneSequenceFile::setValue(const QString &key, const QVariant &value)
{
    if (!isInitialized())
        return;
    s_tsSettings->setValue(key, value);
}

bool TuneSequenceFile::isInitialized()
{
    if (!s_tsfInitialized)
    {
        qWarning() << "TuneSequenceFile uninitialized using!";
        return false;
    }
    return true;
}
