#ifndef MIP_H
#define MIP_H

#include "../../gen/stdfunc.h"
#include "../../interfaces/iec104.h"
#include "../../widgets/udialog.h"

class Mip : public UWidget
{
    Q_OBJECT
public:
    enum AvtukVariants
    {
        M81,
        M82,
        M83
    };

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

    Mip(bool withGUI = true, AvtukVariants ModuleType = M82, QWidget *parent = nullptr);

    void updateFloatData(const QVariant &data) override;
    MipDataStruct getData();
    void start();
    void stop();
    Error::Msg check();
    void setModuleType(AvtukVariants type);
    void setNominalCurrent(double inom);

private:
    UniquePointer<IEC104> m_device;
    MipDataStruct m_mipData;
    AvtukVariants m_moduleType;
    double iNom;
    //    bool m_withGUI;

    void setupUI();

signals:
    void finished();
};

#endif // MIP_H
