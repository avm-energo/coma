#ifndef MIP_H
#define MIP_H

#include "../module/modules.h"
#include "../widgets/udialog.h"

#include <gen/stdfunc.h>
#include <interfaces/types/settingstypes.h>

class Mip : public QObject
{
    Q_OBJECT
public:
    struct MipDataStruct
    {
        float reserved1;
        float freq[3];           // 1-3
        float uPhase[3];         // 4-6
        float iPhase[3];         // 7-9
        float iZero;             // 10
        float loadAnglePhase[3]; // 11-13
        float uAnglePhase[3];    // 14-16
        float temp;              // 17
        float reserved2;         // 18
        float uLinearPhase[3];   // 19-21
        float pPhase[3];         // 22-24
        float p;                 // 25
        float qPhase[3];         // 26-28
        float q;                 // 29
        float sPhhase[3];        // 30-32
        float s;                 // 33
        float phase3LoadAngle;   // 34
        float reserved3;         // 35
        float uSequence[3];      // 36-38
        float iSequence[3];      // 39-41
    };

    using MType = Modules::MezzanineBoard;

    Mip(bool withGUI = true, MType ModuleType = MType::MTM_82, QWidget *parent = nullptr);
    ~Mip() noexcept;

    void updateData(const DataTypes::FloatStruct &fl);
    MipDataStruct getData();
    bool start();
    Error::Msg check();
    void setModuleType(MType type);
    void setNominalCurrent(float inom);
    UWidget *widget();
    MipDataStruct takeOneMeasurement(float i2nom);

private:
    BaseInterface *m_iface;
    MipDataStruct m_mipData;
    MType m_moduleType;
    float iNom;
    UWidget *m_widget;
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

#endif // MIP_H
