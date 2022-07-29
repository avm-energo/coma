#pragma once

#include "../datablock.h"

class BdaIn82 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    { // Структура блока выходных данных
        // в масштабах входных сигналов (для настройки)
        float Frequency;       // Частота в сети, Гц
        float IUefNat_filt[6]; // Истинные действующие значения сигналов (в вольтах или мА на входе)
        float IUeff_filtered[6]; // действующие значения сигналов по 1-й гармонике
        float phi_next_f[6]; // Углы сдвига сигналов по 1-й гармонике относительно Ua в градусах
    };

    BdaIn82(QObject *parent = nullptr);
    void setupValuesDesc() override;

    BlockData *data();

private:
    std::unique_ptr<BlockData> m_blockData;
};
