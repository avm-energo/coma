#ifndef TUNECLASS_H
#define TUNECLASS_H

#include "../../gen/s2.h"

#include <QWidget>

class TuneClass : public QObject
{
    Q_OBJECT
public:
    TuneClass(int bacnum, S2DataTypes::S2ConfigType *S2Config, QObject *parent = nullptr);
    virtual QWidget *BacWidget() = 0;
    S2DataTypes::S2ConfigType *getS2Config();

private:
    int m_BacNum;
    S2DataTypes::S2ConfigType *S2Config;

signals:

private slots:
    virtual void SetDefCoefs() = 0;
    void ReadTuneCoefs();
    bool WriteTuneCoefsSlot();
};

#endif // TUNECLASS_H
