#pragma once

#include <datablocks/datablock.h>

class Bda84 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float Ueff_ADC[6];
        float Frequency;
        float Pt100;
    };

    Bda84(QObject *parent = nullptr);
    void setupValuesDesc() override;

    BlockData *data();

private:
    std::unique_ptr<BlockData> m_blockData;
};
