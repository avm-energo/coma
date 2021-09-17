#ifndef EOSCILLOGRAM_H
#define EOSCILLOGRAM_H

#include "../gen/datatypes.h"
#include "trendviewmodel.h"

#include <QByteArray>
#include <QObject>
#include <QVector>

class EOscillogram : public QObject
{
    Q_OBJECT
public:
    EOscillogram(QObject *parent = nullptr);
    ~EOscillogram();

    int BASize, BSize, Pos;
    QByteArray BA;
    S2DataTypes::SwitchJourRecord SWJRecord;

    bool PosPlusPlus(void *dst, int size);

    bool ProcessOsc(TrendViewModel *mdl);

private:
    QVector<QVector<double>> AnalogMainData, DigitalMainData;
    QVector<double> MainPoints;
    int PointsNum;
    QString Filename;

public slots:

private slots:
    void SaveToExcel();
    void SaveToComtrade();
};

#endif // EOSCILLOGRAM_H
