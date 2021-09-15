#pragma once
#include "../gen/stdfunc.h"
#include "trendviewdialog.h"

#include <QObject>
class OscManager : public QObject
{
public:
    OscManager(QObject *parent = nullptr);
    void loadOscFromFile(const QString &filename);
    void loadOsc(const QByteArray &buffer);
    void loadSwjFromFile(const QString &filename);

private:
    UniquePointer<TrendViewDialog> trendDialog;
};
