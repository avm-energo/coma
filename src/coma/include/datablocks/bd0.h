#pragma once

#include <datablocks/datablock.h>

class Bd0 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float Tmk;  // Температура кристалла микроконтроллера, °С
        float Vbat; // Напряжение аккумуляторной батареи, В
    };

    Bd0(QObject *parent = nullptr);

    BlockData *data();
    void setupValuesDesc() override;

private:
    std::unique_ptr<BlockData> m_blockData;
};
