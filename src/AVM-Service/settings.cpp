#include "settings.h"

bool Settings::isKeyExist(const QString &type, const QString &chstr)
{
    std::unique_ptr<QSettings> settings = std::unique_ptr<QSettings>(new QSettings(SOFTDEVELOPER, PROGNAME));
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QString key = type + QString::number(i);
        if (settings->value(key, "").toString() == chstr)
            return true;
    }
    return false;
}

void Settings::rotateSettings(const QString &type, const QString &name)
{
    std::unique_ptr<QSettings> settings = std::unique_ptr<QSettings>(new QSettings(SOFTDEVELOPER, PROGNAME));
    QStringList sl;
    QString namename, oldnamename;
    // 1. get all type+'i' from registry (count)
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        namename = type + QString::number(i);
        QString value = settings->value(namename, "").toString();
        // QString value = sets->value(namename, "").toString();
        if (!value.isEmpty())
            sl << value;
    }
    // 2. if count < 5 then return count
    if (sl.size() < MAXREGISTRYINTERFACECOUNT)
        sl.append("");
    else
    {
        // 3. else delete group "type + sl.size()-1"
        namename = type + QString::number(sl.size() - 1);
        settings->remove(settings->value(namename, "").toString());
    }
    // and rotate it (1->0, 2->1 etc)
    for (int i = (sl.size() - 1); i > 0; --i)
    {
        oldnamename = type + QString::number(i);
        namename = type + QString::number(i - 1);
        settings->setValue(oldnamename, settings->value(namename, ""));
    }
    namename = type + "0";
}

void Settings::saveEth(QString name, QString ipstr, int bs)
{
    QString key = PROGNAME;
    key += "\\" + name;
    QSettings *sets = new QSettings(SOFTDEVELOPER, key);
    sets->setValue("ip", ipstr);

    sets->setValue("bs", QString::number(bs));
}

void Settings::saveRs(QMap<QString, QString> values, QString name)
{
    QString key = PROGNAME;
    key += "\\" + name;
    auto settings = UniquePointer<QSettings>(new QSettings(SOFTDEVELOPER, key));
    for (auto it = values.cbegin(); it != values.cend(); it++)
    {
        settings->setValue(it.key(), it.value());
    }
}

QStringList Settings::loadSettings(QString name)
{
    QStringList list;
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        std::unique_ptr<QSettings> sets = std::unique_ptr<QSettings>(new QSettings(SOFTDEVELOPER, PROGNAME));
        QString setName = name + QString::number(i);
        list << sets->value(setName, "").toString();
    }
    return list;
}
