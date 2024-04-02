#pragma once

#include "../../widgets/udialog.h"

#include <device/device_list.h>
#include <gen/error.h>
#include <gen/stdfunc.h>
#include <interfaces/types/settingstypes.h>

class BaseInterface;

struct MipDataStruct
{
    float reserved1;
    float freqUPhase[3];   // 1-3
    float uPhase[3];       // 4-6
    float iPhase[3];       // 7-9
    float iZero;           // 10
    float phiLoadPhase[3]; // 11-13
    float phiUPhase[3];    // 14-16
    float temp;            // 17
    float reserved2;       // 18
    float uLinearPhase[3]; // 19-21
    float pPhase[3];       // 22-24
    float p;               // 25
    float qPhase[3];       // 26-28
    float q;               // 29
    float sPhase[3];       // 30-32
    float s;               // 33
    float phiLoad;         // 34
    float reserved3;       // 35
    float uZeroSeq[3];     // 36-38
    float iZeroSeq[3];     // 39-41
    float freq;            // 42
    float phiUab;          // 43
    float phiUbc;          // 44
    float phiUca;          // 45
};

class Mip : public QObject
{
    Q_OBJECT
public:
    using MType = Device::MezzanineBoard;

    Mip(bool withGUI = true, MType ModuleType = MType::MTM_82, QWidget *parent = nullptr);
    ~Mip() noexcept;

    void updateData(const DataTypes::FloatStruct &fl);
    MipDataStruct getData();
    bool start();
    Error::Msg check();
    void setModuleType(MType type);
    void setNominalCurrent(float inom);
    QWidget *widget();
    MipDataStruct takeOneMeasurement(float i2nom);

private:
    BaseInterface *m_iface;
    MipDataStruct m_mipData;
    MType m_moduleType;
    float iNom;
    QWidget *m_widget;
    QWidget *m_parent;
    bool m_withGUI;
    ModuleDataUpdater *m_updater;
    QTimer *m_updateTimer;

    bool initConnection(const IEC104Settings &settings);
    void setupWidget();

signals:
    void finished();
    void oneMeasurementReceived();

public slots:
    void stop();
};
