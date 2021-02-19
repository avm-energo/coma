#pragma once
#include "../gen/stdfunc.h"
#include "trendviewdialog.h"

#include <QObject>
class OscManager : public QObject
{
public:
    OscManager(QObject *parent = nullptr);
    void LoadOscFromFile(const QString &filename);
    void loadSwjFromFile(const QString &filename);

private:
    UniquePointer<TrendViewDialog> trendDialog;
};
