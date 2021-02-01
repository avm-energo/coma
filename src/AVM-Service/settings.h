#pragma once
#include "../config.h"
#include "../gen/stdfunc.h"

#include <QObject>
#include <QSettings>
#define MAXREGISTRYINTERFACECOUNT 5 // how much entries can we have for interfaces of each type in registry

class Settings
{
public:
    static bool isKeyExist(const QString &type, const QString &chstr);
    static void rotateSettings(const QString &type, const QString &name);
    static void saveEth(QString name, QString ipstr, int bs);
    static void saveRs(QMap<QString, QString> values, QString name);
    static QStringList loadSettings(QString name);
};
