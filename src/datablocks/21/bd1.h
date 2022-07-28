#pragma once

#include "../datablock.h"

class Bd21_1 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float ain[8]; // тек. значение по каналу (уср., инж.)
        float inI[8]; // тек. значение по каналу (уср., в мА)
        float inU[8]; // тек. значение по каналу (уср., в В)
        //        float at; // тек. значение температуры кристалла, °С
        //        float ainn[AIN21_NUMCH]; // минимальное значение по каналу (инж.)
        //        float ainx[AIN21_NUMCH]; // максимальное значение по каналу (инж.)
    };

    Bd21_1(QObject *parent = nullptr);
    void createWidget() override;
    void updateWidget() override;

private:
    BlockData *m_blockData;
    QWidget *m_widget;
};
