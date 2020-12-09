#ifndef BD0_H
#define BD0_H

#include "datablock.h"

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

    QWidget *widget() override;
    void setDefBlock() override;
    void updateWidget() override;
    BlockData *data();

private:
    BlockData *m_blockData;
    QWidget *m_widget;
};

#endif // BD0_H
