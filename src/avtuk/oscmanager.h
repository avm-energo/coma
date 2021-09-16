#pragma once
#include "../gen/stdfunc.h"
#include "trendviewdialog.h"

#include <QObject>

class TrendViewDialog;

class OscManager : public QObject
{
public:
    OscManager(QObject *parent = nullptr);
    void loadOscFromFile(const QString &filename);
    void loadOsc(const QByteArray &buffer);
    void loadSwjFromFile(const QString &filename);

private:
    UniquePointer<TrendViewDialog> trendDialog;

    const QStringList phases { "фазы А, В, С", "фаза А", "фаза В", "фаза С" };
};
