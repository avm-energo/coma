#pragma once

#include <datablocks/datablock.h>

class Bd21_1 : public DataBlock
{
    Q_OBJECT
public:
    struct BlockData
    {
        float ain[8];  // тек. значение по каналу (уср., инж.)
        float inI[8];  // тек. значение по каналу (уср., в мА)
        float inU[8];  // тек. значение по каналу (уср., в В)
        float ainn[8]; // минимальное значение по каналу (инж.)
        float ainx[8]; // максимальное значение по каналу (инж.)
    };

    Bd21_1(QObject *parent = nullptr);

    void setupValuesDesc() override;

private:
    BlockData *m_blockData;
};
