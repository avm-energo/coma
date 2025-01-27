#pragma once

#include <datablocks/datablock.h>

class Bda21 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float sin[8];
    };

    Bda21(u8 blockNum, QObject *parent = nullptr);
    void setupValuesDesc() override;

private:
    std::unique_ptr<BlockData> m_blockData;

public:
    BlockData *data();
};
