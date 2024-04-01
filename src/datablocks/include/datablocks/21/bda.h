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

    Bda21(QObject *parent = nullptr);
    void setupValuesDesc() override;

private:
    BlockData *m_blockData;
};
