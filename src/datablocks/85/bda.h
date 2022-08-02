#pragma once

#include "../datablock.h"

class Bda85 : public DataBlock
{
public:
    struct BlockData
    {
        float sin[8];
    };

    Bda85(QObject *parent = nullptr);

private:
    BlockData *m_blockData;
    QWidget *m_widget;
};
