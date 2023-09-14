#pragma once

#include <QDebug>
#include <QObject>
#include <gen/helper.h>
#include <interfaces/usbhidsettings.h>
#include <s2/filestruct.h>
#include <s2/s2datatypes.h>

class Console : public QObject
{
    Q_OBJECT
public:
    explicit Console(QObject *parent = nullptr);

    void print(const DataTypes::BitStringStruct &st)
    {
        qDebug() << st;
    }
    void print(const DataTypes::FloatWithTimeStruct &st)
    {
        qDebug() << st;
    }
    void print(const DataTypes::FloatStruct &st)
    {
        qDebug() << st;
    }
    void print(const DataTypes::SinglePointWithTimeStruct &st)
    {
        qDebug() << st;
    }
    void print(const S2::FileStruct &st)
    {
        qDebug() << st;
    }
    void print(const DataTypes::BlockStruct &st)
    {
        qDebug() << st;
    }
    void print(const DataTypes::SignalsStruct &st)
    {
        qDebug() << st;
    }
    void print(const DataTypes::Signal &st)
    {
        qDebug() << st;
    }
    void print(const DataTypes::GeneralResponseStruct &st)
    {
        qDebug() << st;
    }
    void print(const S2::OscInfo &st)
    {
        qDebug() << st;
    }
    void print(const UsbHidSettings &st)
    {
        qDebug() << st;
    }

signals:
};
