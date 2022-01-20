#ifndef MIP_H
#define MIP_H

#include "../gen/stdfunc.h"
#include "../interfaces/iec104.h"
#include "../widgets/uwidget.h"

class Mip : public UWidget
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

    Mip(bool withGUI = true, QWidget *parent = nullptr);

    void updateFloatData(const DataTypes::FloatStruct &fl) override;
    MipDataStruct GetData();

private:
    UniquePointer<IEC104> m_device;
    MipDataStruct m_mipData;

    void setupUI();
    void Start();
    void Stop();
};

#endif // MIP_H
