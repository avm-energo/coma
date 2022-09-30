#ifndef MIP_H
#define MIP_H

#include "../../gen/stdfunc.h"
#include "../../interfaces/iec104.h"
#include "../../module/modules.h"
#include "../../widgets/udialog.h"

class Mip : public QObject
{
    Q_OBJECT
public:
    struct MipDataStruct
    {
        float reserved;
        float f[3];          // 1-3
        float uPh[3];        // 4-6
        float iPh[3];        // 7-9
        float iN;            // 10
        float phyPh[3];      // 11-13
        float reserved2[3];  // 14-16
        float temp;          // 17
        float reserved3;     // 18
        float uLin[3];       // 19-21
        float pPh[3];        // 22-24
        float p;             // 25
        float qPh[3];        // 26-28
        float q;             // 29
        float sPh[3];        // 30-32
        float s;             // 33
        float reserved4[13]; // 34-46
    };

    using MType = Modules::MezzanineBoard;

    Mip(bool withGUI = true, MType ModuleType = MType::MTM_82, QWidget *parent = nullptr);

    void updateData(const DataTypes::FloatStruct &fl);
    MipDataStruct getData();
    bool start();
    Error::Msg check();
    void setModuleType(MType type);
    void setNominalCurrent(float inom);
    UWidget *widget();
    MipDataStruct takeOneMeasurement(float i2nom);

private:
    IEC104 *m_device;
    MipDataStruct m_mipData;
    MType m_moduleType;
    float iNom;
    UWidget *m_widget;
    QWidget *m_parent;
    bool m_withGUI;
    ModuleDataUpdater *m_updater;
    QTimer *m_updateTimer;

    void setupWidget();

signals:
    void finished();

public slots:
    void stop();
};

#endif // MIP_H
