#pragma once

#include "../datablock.h"

class Bda : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float Ueff_ADC[6];
        float Frequency;
        float Pt100;
    };

    Bda(QObject *parent = nullptr);
    void setupValuesDesc() override;

    BlockData *data();

private:
    std::unique_ptr<BlockData> m_blockData;
};
