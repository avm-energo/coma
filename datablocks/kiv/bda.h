#ifndef KIV_BDA_H
#define KIV_BDA_H

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

    void createWidget() override;
    void setDefBlock() override;
    void updateWidget() override;
    BlockData *data();

private:
    BlockData *m_blockData;
    QWidget *m_widget;
};

#endif // KIV_BDA_H
