#pragma once

#include "../datablock.h"

class Bda21 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float sin[8];
    };

    Bda21(QObject *parent = nullptr);
    void createWidget() override;
    void updateWidget() override;

private:
    BlockData *m_blockData;
    QWidget *m_widget;
};
