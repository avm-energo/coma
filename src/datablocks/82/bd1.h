#pragma once

#include "../datablock.h"
//#include "../module/modules.h"
#include <device/device_list.h>

class Bd182 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float Frequency; // 2 частота сигналов, Гц,
        float IUefNat_filt[6]; // 3-8 Истинные действующие значения сигналов в вольтах и амперах
        float IUeff_filtered[6]; // 9-14 Действующие значения сигналов по 1-й гармонике
        float phi_next_f[6]; // 15-20 Углы сдвига между сигналами по 1-й гармонике в 6 каналах в градусах,
        float PNatf[3];      // 21-23 Истинная активная мощность, по фазам
        float SNatf[3]; // 24-26 Кажущаяся полная мощность по эффективным  токам и напряжениям,
        float QNatf[3]; // 27-29 Реактивная мощность по кажущейся полной и истинной активной
        float CosPhiNat[3]; // 30-32 cos phi по истинной активной мощности
        float Pf[3];        // 33-35 Активная мощность по 1-й гармонике
        float Qf[3];        // 36-38 Реактивная мощность по 1-й гармонике
        float Sf[3];        // 39-41 Полная мощность по 1-й гармонике
        float CosPhi[3];    // 42-44 cos phi по 1-й гармонике
    };

    float PHI[3]; // угол между током и напряжением

    Bd182(Device::MezzanineBoard typem, QObject *parent = nullptr);

    void setupValuesDesc() override;
    void specificUpdateWidget() override;

    Error::Msg checkValues(float voltage, float current, float degree,
        float tolerance); // deg - угол в градусах между токами и напряжениями одной фазы, tol - 0: начальная точность,
                          // 1 - повышенная
    BlockData *data();

private:
    std::unique_ptr<BlockData> m_blockData;
    Device::MezzanineBoard m_typeM;

    Error::Msg checkAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi,
        double utol, double itol, double pht, double pt, double ct);
};
