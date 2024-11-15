#include "datablocks/21/bac.h"

Bac21::Bac21(u8 numblock, QObject *parent) : DataBlock(parent)
{
    m_blockData = std::unique_ptr<BlockData>(new BlockData);
    setBlock({ numblock, "Настроечные параметры", DataTypes::DataBlockTypes::BacBlock, //
        m_blockData.get(), sizeof(BlockData), true });
    for (int i = 0; i < 8; ++i)
    {
        m_blockDataW.fbin_I[i] = &data()->bac1[i].fbin_I;
        m_blockDataW.fbin_U[i] = &data()->bac2[i].fbin_U;
        m_blockDataW.fkiin[i] = &data()->bac1[i].fkiin;
        m_blockDataW.fkuin[i] = &data()->bac1[i].fkuin;
    }
}

void Bac21::setupValuesDesc()
{
    for (int i = 0; i < 8; ++i)
    {
        addNewGroup("Настроечные коэффициенты смещения по току", "fbini", 0, 8, m_blockDataW.fbin_I[0], 5);
        addNewGroup("Настроечные коэффициенты смещения по напряжению", "fbinu", 0, 8, m_blockDataW.fbin_U[0], 5);
        addNewGroup("Настроечные коэффициенты наклона по току", "fkiin", 0, 8, m_blockDataW.fkiin[0], 5);
        addNewGroup("Настроечные коэффициенты наклона по напряжению", "fkuin", 0, 8, m_blockDataW.fkuin[0], 5);
    }
}

void Bac21::setDefBlock()
{
    for (int i = 0; i < 8; ++i)
    {
        m_blockData->bac1[i].fbin_I = 0;
        m_blockData->bac1[i].fkiin = 1;
        m_blockData->bac1[i].fkuin = 1;
        m_blockData->bac2[i].fbin_U = 0;
    }
}

Bac21::BlockData *Bac21::data()
{
    return m_blockData.get();
}
