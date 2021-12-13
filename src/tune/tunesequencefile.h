#ifndef TUNESEQUENCEFILE_H
#define TUNESEQUENCEFILE_H

#define TS_FILENAME "calibr.ini"
#include <QSettings>
#include <QString>
#include <memory>

class TuneSequenceFile
{
public:
    struct TuneDescrStruct
    {
        QString parametername;
        std::unique_ptr<float> parameter;
    };

    TuneSequenceFile();

    static void init();
    static void saveTuneSequenceFile(int step);
    static QVariant value(const QString &key, const QVariant &defaultvalue = QVariant());
    static bool contains(const QString &key);
    static void setValue(const QString &key, const QVariant &value);
    static bool isInitialized();
    static void clearTuneDescrVector();
    static void addItemToTuneDescrVector(const QString &descr, float &value);
    static void loadItemsFromFile();
    static void saveItemsToFile();

private:
    static std::map<QString, std::unique_ptr<float>> s_tuneDescrMap;
    static std::unique_ptr<QSettings> s_tsSettings;
    static QString s_cpuSerialNum;
    static bool s_tsfInitialized;
};

#endif // TUNESEQUENCEFILE_H
