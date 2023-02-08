#pragma once

#include "../datablock.h"
#include "../module/modules.h"

class Bda82 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float IUeff_ADC[6]; // данные в единицах АЦП
        float Frequency; // 2 частота сигналов, Гц,
    };

    Bda82(QObject *parent = nullptr);

    void setupValuesDesc() override;

    Error::Msg checkValues(Modules::MezzanineBoard mtypeM, DataTypes::FLOAT_6t i2Noms); // deg - угол в градусах между токами и напряжениями одной фазы, tol - 0: начальная точность,
                          // 1 - повышенная
    BlockData *data();

private:
    std::unique_ptr<BlockData> m_blockData;
    Error::Msg checkAnalogValues(QList<float>, QList<float> tolerances);
};
