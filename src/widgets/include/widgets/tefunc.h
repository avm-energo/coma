#pragma once

#include <QTextEdit>

class TEFunc
{
public:
    TEFunc();

    static bool AppendTEData(QWidget *parent, const QString &tename, const QString &tetext);
    static bool SetTEData(QWidget *parent, const QString &tename, const QString &tetext);
    static bool TEData(QWidget *parent, const QString &tename, QString &tevalue);
};
